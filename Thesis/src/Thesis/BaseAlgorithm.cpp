#include "BaseAlgorithm.h"
#include <iostream>
#include <algorithm>
#include <cmath>

//kiszámolja gyártási intervallumokra  a mennyiségeket
std::vector<ProductionInterval> BaseAlgorithm::BuildProductionIntervals( const std::vector<ProductionEvent>& vProductionEvents, const std::vector<ProductionTimeData>& vProductionTimes )
{
	std::vector<ProductionInterval> vecProductionIntervals;

	for( const auto& timeData : vProductionTimes )
	{
		ProductionInterval sInterval;

		sInterval.strTaskId = timeData.strTaskId;
		sInterval.strOperationId = timeData.strOperationId;
		sInterval.strMachineId = timeData.strMachineId;
		sInterval.tTimeBeg = timeData.tTimeBeg;
		sInterval.tTimeEnd = timeData.tTimeEnd;
		sInterval.dOperationTime = timeData.dOperationTime;

		for( auto& event : vProductionEvents )
		{
			if( event.strTaskId != sInterval.strTaskId )
				continue;

			if( event.strOperationId != sInterval.strOperationId )
				continue;

			if( event.strMachineId != sInterval.strMachineId )
				continue;

			if( event.tTimeStamp >= sInterval.tTimeBeg && event.tTimeStamp <= sInterval.tTimeEnd )
			{
				if( event.eEventType == T_PRODUCT /* && event.strProductId == event.strMaterialId*/ )
					sInterval.dProducedQuantity += event.dQuantity;

				else if( event.eEventType == T_SCRAP )
					sInterval.dScrapQuantity += event.dQuantity;
			}
		}
		vecProductionIntervals.push_back( sInterval );
	}

	return vecProductionIntervals;
}

/*std::map<std::string, Operation> InferOperationOrder(const Product& sProduct)
{
	std::map<std::string, std::set<std::string>> mapOperationPredecessors;
	for( const auto& taskPair : sProduct.mapTasks )
	{
		const Task& sTask = taskPair.second;
		for( const auto& operationPair : sTask.mapOperations )
		{
			const Operation& sOperation = operationPair.second;
			for( const auto& event : sOperation.vEvents )
			{
				if( event.eEventType == T_PRODUCT && event.strProductId == event.strMaterialId )
				{
					mapOperationPredecessors[event.strOperationId].insert( event.strOperationId );
				}
			}
		}
	}
	return mapOperationPredecessors;
}*/

int BaseAlgorithm::MostFrequentOrder( const std::vector<int>& vOrders )
{
	std::map<int, int> mapOrderCounts;

	for( int order : vOrders )
	{
		mapOrderCounts[order]++;
	}

	int iMostFrequentOrder = -1;
	int iMaxCount = 0;

	for( const auto& pair : mapOrderCounts )
	{
		if( pair.second > iMaxCount)
		{
			iMaxCount = pair.second;
			iMostFrequentOrder = pair.first;
		}
	}
	return iMostFrequentOrder;
}

Recipe BaseAlgorithm::GenerateRecipeForProduct( const Product& sProduct )
{
	Recipe sRecipe;
	sRecipe.strProductId = sProduct.strProductId;

	//BuildJobs(...)
	std::map<std::tuple<std::string, std::string>, Job> mapJobs;

	std::map<std::string, std::set<std::string>> mapOperationPredecessors;

	//végig megyünk a termék összes gyártásán
	for( const auto& task : sProduct.mapTasks )
	{
		std::string strTaskId = task.first;
		const Task& sTask = task.second;

		std::cout << "Task: " << strTaskId << std::endl;

		std::vector<Job*> vOperationsOrderedByTimestamp;

		// aztán az összes mûveleten
		for( const auto& operation : sTask.mapOperations )
		{
			std::string strOperationId = operation.first;
			const Operation& sOperation = operation.second;

			std::cout << "Operation: " << strOperationId << std::endl;
			
			// minden mûvelethez létrehozunk egy munkát, ami összesíti az összes eseményt
			std::tuple<std::string, std::string> key = std::make_tuple( strTaskId, strOperationId );
			Job& sJob = mapJobs[key];

			sJob.strTaskId = strTaskId;
			sJob.strOperationId = strOperationId;
			
			//az összes event ugyanahhoz a taskhoz ugyanahhoz a munkához
			for( const auto& event : sOperation.vEvents )
			{
				std::cout << "ProductionReportItem: " << event.strMaterialId << " " << event.dQuantity << std::endl << std::endl;

				if( event.eEventType == T_PRODUCT /* && event.strProductId == event.strMaterialId */ )
				{
					sJob.dPieceGood += event.dQuantity;
					sJob.tEnd = std::max( sJob.tEnd, event.tTimeStamp );
				}

				else if( event.eEventType == T_SCRAP )
				{
					sJob.dPieceScrap += event.dQuantity;
					sJob.tEnd = std::max( sJob.tEnd, event.tTimeStamp );
				}

				else if( event.eEventType == T_INPUT && event.strProductId != event.strMaterialId )
					sJob.mapMaterialConsumptions[event.strMaterialId] += std::abs( event.dQuantity );

				sJob.vUsedMachines.insert( event.strMachineId );
			}

			vOperationsOrderedByTimestamp.push_back( &sJob );

			sJob.vProductionIntervals = BuildProductionIntervals( sOperation.vEvents, sOperation.vProductionTimes );
			// ez most tartalmazza a terméknek a gyártási intervallumait, a mennyiségekkel együtt gépek szerint, de a gépekhez tartozó mûveleti idõt még nem számolja ki
		}

		std::sort( vOperationsOrderedByTimestamp.begin(), vOperationsOrderedByTimestamp.end(),
			[]( const Job* j1, const Job* j2 )
			{ 
				return j1->tEnd < j2->tEnd; 
			} );

		/*if (a->tEnd != b->tEnd)
			return a->tEnd < b->tEnd;

		return a->strOperationId < b->strOperationId;*/

		for( size_t i = 0; i < vOperationsOrderedByTimestamp.size(); ++i )
		{
			vOperationsOrderedByTimestamp[i]->iOrder = static_cast<int>(i + 1);
		}

		/*for (size_t i = 0; i < vOperationEndTimes.size(); ++i)
		{
			Job* pCurrentJob = vOperationEndTimes[i];
			if( i > 0 )
			{
				Job* pPreviousJob = vOperationEndTimes[i - 1];
				mapOperationPredecessors[pCurrentJob->strOperationId].insert( pPreviousJob->strOperationId );
			}
		}*/
	}

	//AggregateOperations(...)
	std::map<std::string, AggregatedOperationData> mapOperations;
	for( const auto& jobPair : mapJobs )
	{
		/*const std::tuple<std::string, std::string> key = jobPair.first;
		const std::string& strOperationId = std::get<1>(key);
		const std::string& strTaskId = std::get<0>(key);
		const Job& sJob = jobPair.second;

		std::cout << "Job for Operation: " << strOperationId << std::endl;
		std::cout << "Produced Quantity: " << sJob.dProducedQuantity << std::endl;
		std::cout << "Scrap Quantity: " << sJob.dScrapQuantity << std::endl;
		for( const auto& materialPair : sJob.mapMaterialConsumptions )
		{
			const std::string& strMaterialId = materialPair.first;
			double dUsedQuantity = materialPair.second;
			std::cout << "Material Used: " << strMaterialId << ", Quantity: " << dUsedQuantity << std::endl;

		}
		std::cout << "Machines used: ";
		for( const auto& machineId : sJob.vUsedMachines )
		{
			std::cout << machineId << " ";
		}
		std::cout << std::endl;*/

		const Job& sJob = jobPair.second;

		AggregatedOperationData& sAggregatedOperation = mapOperations[sJob.strOperationId];

		sAggregatedOperation.vOperationOrders.push_back( sJob.iOrder );

		sAggregatedOperation.strOperationId = sJob.strOperationId;
		sAggregatedOperation.iJobCount++;

		sAggregatedOperation.dProducedQuantity += sJob.dPieceGood;
		sAggregatedOperation.dScrapQuantity += sJob.dPieceScrap;

		//sAggregatedOperation.mapOperationTimesByMachine.insert( sJob.mapOperationTimesByMachine.begin(), sJob.mapOperationTimesByMachine.end() );

		sAggregatedOperation.vMachines.insert( sJob.vUsedMachines.begin(), sJob.vUsedMachines.end() );

		for( const auto& interval : sJob.vProductionIntervals )
		{
			MachineInfo& sMachineInfo = sAggregatedOperation.mapMachineInfos[interval.strMachineId];

			sMachineInfo.dTotalOperationTime += interval.dOperationTime;
			sMachineInfo.dProducedQuantity += interval.dProducedQuantity;
			sMachineInfo.dScrapQuantity += interval.dScrapQuantity;
			sMachineInfo.iIntervalCount++;
		}

		for( const auto& materialPair : sJob.mapMaterialConsumptions )
		{
			const std::string& strMaterialId = materialPair.first;
			double dUsedQuantity = materialPair.second;

			sAggregatedOperation.mapMaterials[strMaterialId] += dUsedQuantity;
			sAggregatedOperation.mapMaterialCounts[strMaterialId]++;
		}
	}

	//BuildRecipe(...)

	for( const auto& operationPair : mapOperations )
	{
		const AggregatedOperationData& sAggregatedOperation = operationPair.second;

		if( sAggregatedOperation.iJobCount == 0 )
		{
			std::cerr << "Warning: No jobs found for operation " << sAggregatedOperation.strOperationId << std::endl;
			continue;
		}

		double dAveragePieceGood = sAggregatedOperation.dProducedQuantity / sAggregatedOperation.iJobCount;
		double dAveragePieceScrap = sAggregatedOperation.dScrapQuantity / sAggregatedOperation.iJobCount;
		/*std::cout << "Aggregated Operation: " << sAggregatedOperation.strOperationId << std::endl;
		std::cout << "Produced Quantity: " << sAggregatedOperation.dProducedQuantity << std::endl;
		std::cout << "Scrap Quantity: " << sAggregatedOperation.dScrapQuantity << std::endl;

		for( const auto& materialPair : sAggregatedOperation.mapMaterials )
		{
			const std::string& strMaterialId = materialPair.first;
			double dUsedQuantity = materialPair.second;
			std::cout << "Material Used: " << strMaterialId << ", Quantity: " << dUsedQuantity << std::endl;
		}
		std::cout << "Machines used: ";
		for( const auto& machineId : sAggregatedOperation.vMachines )
		{
			std::cout << machineId << " ";
		}
		std::cout << std::endl;*/

		RecipeItem sRecipeItem;
		sRecipeItem.iOrder = MostFrequentOrder( sAggregatedOperation.vOperationOrders );
		sRecipeItem.strOperationId = sAggregatedOperation.strOperationId;
		sRecipeItem.dBaseQuantity = 1.0;

		if( dAveragePieceGood > 0 )
			sRecipeItem.dRunningScrap = dAveragePieceScrap / dAveragePieceGood; // megnézni hogy arány vagy szám
		else
			sRecipeItem.dRunningScrap = 0.0;

		for( const auto& materialPair : sAggregatedOperation.mapMaterials )
		{
			double dUsedQuantity = materialPair.second;
			const std::string& strMaterialId = materialPair.first;
			int iNumMaterialOccurrences = sAggregatedOperation.mapMaterialCounts.at(strMaterialId);

			double dAverageUsedQuantity = dUsedQuantity / iNumMaterialOccurrences;
			
			MaterialDemand sMaterialDemand;
			sMaterialDemand.strMaterialId = strMaterialId;
			sMaterialDemand.eType = BIT_INPUT;
			sMaterialDemand.dBaseQuantity = 1.0;

			//még átgondolni
			if( sAggregatedOperation.dProducedQuantity > 0 )
				sMaterialDemand.dPiece = dAverageUsedQuantity / dAveragePieceGood;
			sRecipeItem.vMaterialDemands.push_back( sMaterialDemand );
		}

		for( const auto& machineInfoPair : sAggregatedOperation.mapMachineInfos )
		{
			const std::string& strMachineId = machineInfoPair.first;
			const MachineInfo& sMachineInfo = machineInfoPair.second;

			MachineDemand sMachineDemand;

			sMachineDemand.strMachineId = strMachineId;
			sMachineDemand.dBaseQuantity = 1.0;

			double dTotalQuantity = sMachineInfo.dProducedQuantity + sMachineInfo.dScrapQuantity;

			if( dTotalQuantity > 0 )
				sMachineDemand.dOperationTime = sMachineInfo.dTotalOperationTime / dTotalQuantity; // átlagos mûveleti idõ

			sRecipeItem.vMachineDemands.push_back( sMachineDemand );
		}

		/*for (const auto& machineId : sAggregatedOperation.vMachines)
		{
			MachineDemand sMachineDemand;
			sMachineDemand.strMachineId = machineId;
			sMachineDemand.dBaseQuantity = 1.0;
			//sMachineDemand.dOperationTime = sAggregatedOperation.mapOperationTimesByMachine.at(machineId) / sAggregatedOperation.iJobCount; // átlagos mûveleti idõ
			sRecipeItem.vMachineDemands.push_back( sMachineDemand );
		}*/

		sRecipe.vRecipeItems.push_back( sRecipeItem );
	}

	return sRecipe;
}