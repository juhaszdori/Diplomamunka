#include "BaseAlgorithm.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_map>

//kiszámolja gyártási intervallumokra  a mennyiségeket
std::vector<ProductionInterval> BaseAlgorithm::BuildProductionIntervals( const std::vector<ProductionEvent>& vProductionEvents, const std::vector<ProductionTimeData>& vProductionTimes )
{
	std::vector<ProductionInterval> vecProductionIntervals;

	for( const auto& timeData : vProductionTimes )
	{
		if( timeData.tTimeBeg > timeData.tTimeEnd )
		{
			std::cerr << "Warning: Invalid time data for task " << timeData.strTaskId
				<< ", operation " << timeData.strOperationId
				<< ", machine " << timeData.strMachineId
				<< ". Start time is after end time." << std::endl;
			continue;
		}

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

			if( event.tTimeStamp > sInterval.tTimeBeg && event.tTimeStamp <= sInterval.tTimeEnd )
			{
				if( event.eEventType == T_PRODUCT && event.strProductId == event.strMaterialId )
					sInterval.dProducedQuantity += event.dQuantity;

				else if( event.eEventType == T_SCRAP )
					sInterval.dScrapQuantity += event.dQuantity;

				if( sInterval.strQuantityUnitId.empty() )
				sInterval.strQuantityUnitId = event.strQuantityUnitId;
			}
		}
		vecProductionIntervals.push_back( sInterval );
	}

	return vecProductionIntervals;
}

/*int BaseAlgorithm::MostFrequentOrder(const std::vector<int>& vOrders)
{
	std::unordered_map<int, int> mapOrderCounts;

	// megszámoljuk hogy egy "sorrend" hányszor szerepel
	for( int iOrder : vOrders )
	{
		mapOrderCounts[iOrder]++;
	}

	int iMostFrequentOrder = -1;
	int iMaxCount = 0;

	for( const auto& pair : mapOrderCounts )
	{
		if( pair.second > iMaxCount )
		{
			iMaxCount = pair.second;
			iMostFrequentOrder = pair.first;
		}
	}
	return iMostFrequentOrder;
}*/

std::vector<std::string> BaseAlgorithm::MostFrequentOperationSequenceForProduct( const std::vector<std::vector<std::string>>& vSequences )
{
	std::map<std::vector<std::string>, int> mapSequenceCounts;
	
	for( const auto& sequence : vSequences )
	{
		mapSequenceCounts[sequence]++;
	}

	const std::vector<std::string>* pMostFrequentSequence = nullptr;
	int iMaxCount = 0;

	for( const auto& pair : mapSequenceCounts )
	{
		if( pair.second > iMaxCount )
		{
			iMaxCount = pair.second;
			pMostFrequentSequence = &pair.first;
		}
	}

	return pMostFrequentSequence ? *pMostFrequentSequence : std::vector<std::string>();	
}

std::vector<std::vector<std::string>> BaseAlgorithm::BuildOperationSequencesForProduct( const Product& sProduct, std::map<std::tuple<std::string, std::string>, Job>& mapJobs )
{
	std::vector<std::vector<std::string>> vOperationSequences;

	for( const auto& taskPair : sProduct.mapTasks )
	{
		const std::string& strTaskId = taskPair.first;
		const Task& sTask = taskPair.second;

		std::vector<Job*> vOperationsOrderedByTimestamp;

		for( const auto& operationPair : sTask.mapOperations )
		{
			const std::string& strOperationId = operationPair.first;
			const Operation& sOperation = operationPair.second;

			std::tuple<std::string, std::string> key = std::make_tuple( strTaskId, strOperationId );
			Job& sJob = mapJobs[key];

			vOperationsOrderedByTimestamp.push_back( &sJob );
		}

		std::sort( vOperationsOrderedByTimestamp.begin(), vOperationsOrderedByTimestamp.end(),
		[]( const Job* j1, const Job* j2 )
		{ 
			return j1->tEnd < j2->tEnd;
		} );

		std::vector<std::string> vOperationSequence;

		for( size_t i = 0; i < vOperationsOrderedByTimestamp.size(); ++i )
		{
			vOperationSequence.push_back( vOperationsOrderedByTimestamp[i]->strOperationId );
		}

		vOperationSequences.push_back( vOperationSequence );
	}

	return vOperationSequences;
}

std::map<std::tuple<std::string, std::string>, Job> BaseAlgorithm::BuildJobs( const Product& sProduct )
{
	std::map<std::tuple<std::string, std::string>, Job> mapJobs;

	//végig megyünk a termék összes gyártásán
	for( const auto& task : sProduct.mapTasks )
	{
		std::string strTaskId = task.first;
		const Task& sTask = task.second;

		//std::cout << "Task: " << strTaskId << std::endl;

		//std::vector<Job*> vOperationsOrderedByTimestamp;
		//std::vector<std::string> vOperationSequence;

		// aztán az összes mûveleten
		for( const auto& operation : sTask.mapOperations )
		{
			std::string strOperationId = operation.first;
			const Operation& sOperation = operation.second;

			//std::cout << "Operation: " << strOperationId << std::endl;
			
			// minden mûvelethez létrehozunk egy munkát, ami összesíti az összes eseményt
			std::tuple<std::string, std::string> key = std::make_tuple( strTaskId, strOperationId );
			Job& sJob = mapJobs[key];

			sJob.strTaskId = strTaskId;
			sJob.strOperationId = strOperationId;
			
			//az összes event ugyanahhoz a taskhoz ugyanahhoz a munkához
			for( const auto& event : sOperation.vEvents )
			{
				//std::cout << "ProductionReportItem: " << event.strMaterialId << " " << event.dQuantity << std::endl << std::endl;

				if( event.eEventType == T_PRODUCT  && event.strProductId == event.strMaterialId ) // a termelési jelentés cikk mezõjében van a gyártott termék
				{
					sJob.dPieceGood += event.dQuantity;
					sJob.tEnd = std::max( sJob.tEnd, event.tTimeStamp );

					if( sJob.strQuantityUnitId.empty() )
						sJob.strQuantityUnitId = event.strQuantityUnitId;
					//else
						//convert
				}

				else if( event.eEventType == T_SCRAP )
				{
					sJob.dPieceScrap += event.dQuantity;
					sJob.tEnd = std::max( sJob.tEnd, event.tTimeStamp );

					if( sJob.strQuantityUnitId.empty() )
						sJob.strQuantityUnitId = event.strQuantityUnitId;
				}

				else if( event.eEventType == T_INPUT && event.strProductId != event.strMaterialId )
				{
					sJob.mapMaterialConsumptions[event.strMaterialId].dUsedQuantity += std::abs( event.dQuantity ); //
					sJob.mapMaterialConsumptions[event.strMaterialId].strQuantityUnitId = event.strQuantityUnitId;
				}

				sJob.vUsedMachines.insert( event.strMachineId );
			}

			//vOperationsOrderedByTimestamp.push_back( &sJob );

			sJob.vProductionIntervals = BuildProductionIntervals( sOperation.vEvents, sOperation.vProductionTimes );
			// ez most tartalmazza a terméknek a gyártási intervallumait, a mennyiségekkel együtt gépek szerint, de a gépekhez tartozó mûveleti idõt még nem számolja ki
		}

		/* std::sort( vOperationsOrderedByTimestamp.begin(), vOperationsOrderedByTimestamp.end(),
			[]( const Job* j1, const Job* j2 )
			{ 
				return j1->tEnd < j2->tEnd; 
			} );

		if (a->tEnd != b->tEnd)
			return a->tEnd < b->tEnd;

		return a->strOperationId < b->strOperationId;

		for( size_t i = 0; i < vOperationsOrderedByTimestamp.size(); ++i )
		{
			vOperationsOrderedByTimestamp[i]->iOrder = static_cast<int>(i + 1);
			vOperationSequence.push_back( vOperationsOrderedByTimestamp[i]->strOperationId );
		}

		vOperationSequences.push_back( vOperationSequence );*/
	}

	return mapJobs;
}

// összegzi a termék mûveleteinek paramétereit
std::map<std::string, AggregatedOperationData> BaseAlgorithm::AggregateOperations( const std::map<std::tuple<std::string, std::string>, Job>& mapJobs )
{
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

		//sAggregatedOperation.vOperationOrders.push_back( sJob.iOrder );

		sAggregatedOperation.strOperationId = sJob.strOperationId;
		sAggregatedOperation.iJobCount++;

		sAggregatedOperation.dProducedQuantity += sJob.dPieceGood;
		sAggregatedOperation.dScrapQuantity += sJob.dPieceScrap;

		sAggregatedOperation.strQuantityUnitId = sJob.strQuantityUnitId;

		// ez redundáns
		sAggregatedOperation.vMachines.insert( sJob.vUsedMachines.begin(), sJob.vUsedMachines.end() );

		for( const auto& interval : sJob.vProductionIntervals )
		{
			MachineInfo& sMachineInfo = sAggregatedOperation.mapMachineInfos[interval.strMachineId];

			sMachineInfo.dTotalOperationTime += interval.dOperationTime;
			sMachineInfo.dProducedQuantity += interval.dProducedQuantity;
			sMachineInfo.dScrapQuantity += interval.dScrapQuantity;
			sMachineInfo.strQuantityUnitId = interval.strQuantityUnitId;
			//sMachineInfo.iIntervalCount++;
		}

		for( const auto& materialPair : sJob.mapMaterialConsumptions )
		{
			const std::string& strMaterialId = materialPair.first;
			const MaterialConsumption& sMaterialConsumption = materialPair.second;

			sAggregatedOperation.mapMaterialConsumptions[strMaterialId].dUsedQuantity += sMaterialConsumption.dUsedQuantity;
			sAggregatedOperation.mapMaterialConsumptions[strMaterialId].strQuantityUnitId = sMaterialConsumption.strQuantityUnitId;

			// ezt át lehetne rakni a vuildjobba és a jobnak átadni attribútumként
			double dProducedQuantity = sJob.dPieceGood + sJob.dPieceScrap;
			double dMaterialRatio = sMaterialConsumption.dUsedQuantity / dProducedQuantity;

			sAggregatedOperation.mapMaterialConsumptions[strMaterialId].vRatios.push_back( dMaterialRatio );
			sAggregatedOperation.mapMaterialConsumptions[strMaterialId].iOccurrence += 1;

			sAggregatedOperation.mapMaterialCounts[strMaterialId]++;
		}
	}

	return mapOperations;
}

Recipe BaseAlgorithm::BuildRecipe( const Product& sProduct, const std::map<std::string, AggregatedOperationData>& mapOperations, std::vector<std::string>& vMostFrequentOperationSequence )
{
	Recipe sRecipe;
	sRecipe.bDefault = true;
	sRecipe.strProductId = sProduct.strProductId;
	sRecipe.strId = "R_" + sProduct.strProductId;

	//AddRecipeItems( sRecipe, mapOperations );
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
		sRecipeItem.strRecipeId = sRecipe.strId;
		sRecipeItem.strId = "RI_" + sProduct.strProductId + "_" + sAggregatedOperation.strOperationId;
		//sRecipeItem.iOrder = MostFrequentOrder( sAggregatedOperation.vOperationOrders );
		sRecipeItem.strOperationId = sAggregatedOperation.strOperationId;
		sRecipeItem.dBaseQuantity = 1.0;
		sRecipeItem.eOperationTimeUnit = UN_SECOND;
		sRecipeItem.eProductionMode = PM_OWN_PRODCUTION;
		sRecipeItem.strBaseQuantityUnitId = sAggregatedOperation.strQuantityUnitId;

		for( int i = 0; i < vMostFrequentOperationSequence.size(); i++ )
		{
			if( vMostFrequentOperationSequence[i] == sAggregatedOperation.strOperationId )
			{
				sRecipeItem.iOrder = i + 1;
				break;
			}
		}

		if( dAveragePieceGood > 0 )
			sRecipeItem.dRunningScrap = dAveragePieceScrap / dAveragePieceGood; // megnézni hogy arány vagy szám és mire használja a dSuite
		else
			sRecipeItem.dRunningScrap = 0.0;

		// AddMaterialDemands( sRecipeItem, sAggregatedOperation, sProduct );
		for( const auto& materialConsumptionPair : sAggregatedOperation.mapMaterialConsumptions )
		{
			const std::string& strMaterialId = materialConsumptionPair.first;
			const MaterialConsumption& sMaterialConsumption = materialConsumptionPair.second;

			int iNumMaterialOccurrences = sAggregatedOperation.mapMaterialCounts.at(strMaterialId);

			double dAverageUsedQuantity = sMaterialConsumption.dUsedQuantity / iNumMaterialOccurrences;
			
			MaterialDemand sMaterialDemand;
			sMaterialDemand.strRecipeItemId = sRecipeItem.strId;
			sMaterialDemand.strId = "MATD_" + sProduct.strProductId + "_" + strMaterialId + "_" + sAggregatedOperation.strOperationId;
			sMaterialDemand.strMaterialId = strMaterialId;
			sMaterialDemand.eType = BIT_INPUT;
			sMaterialDemand.dBaseQuantity = 1.0;
			sMaterialDemand.strBaseQuantityUnitId = sMaterialConsumption.strQuantityUnitId;

			//még átgondolni és megnézni a dSuite-ban
			//double dAverageProducedAll = sAggregatedOperation.dProducedQuantity + sAggregatedOperation.dScrapQuantity;
			//
			//if( dAverageProducedAll > 0 )
			//	sMaterialDemand.dPiece = dAverageUsedQuantity / dAverageProducedAll;

			if( dAveragePieceGood > 0 )
				sMaterialDemand.dPiece = dAverageUsedQuantity / dAveragePieceGood;

			// itt kellene a medián, átlag, szórás
			// CalculateMaterialConsumptionStatisztics()
			sRecipeItem.vMaterialDemands.push_back( sMaterialDemand );
		}

		// AddMachineDemands( sRecipeItem, sAggregatedOperation, sProduct );
		for( const auto& machineInfoPair : sAggregatedOperation.mapMachineInfos )
		{
			const std::string& strMachineId = machineInfoPair.first;
			const MachineInfo& sMachineInfo = machineInfoPair.second;

			MachineDemand sMachineDemand;
			sMachineDemand.strRecipeItemId = sRecipeItem.strId;
			sMachineDemand.strId = "MASD_" + sProduct.strProductId + "_" + strMachineId + "_" + sAggregatedOperation.strOperationId;
			sMachineDemand.strMachineId = strMachineId;
			sMachineDemand.dBaseQuantity = 1.0;
			sMachineDemand.strBaseQuantityUnitId = sMachineInfo.strQuantityUnitId;

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

	std::sort(
		sRecipe.vRecipeItems.begin(),
		sRecipe.vRecipeItems.end(),
		[]( const RecipeItem& a, const RecipeItem& b )
		{
			if( a.iOrder != b.iOrder )
				return a.iOrder < b.iOrder;

			// Holtverseny esetén determinisztikus sorrend
			return a.strOperationId < b.strOperationId;
		});

	return sRecipe;
}

Recipe BaseAlgorithm::GenerateRecipeForProduct( const Product& sProduct )
{
	std::map<std::tuple<std::string, std::string>, Job> mapJobs = BuildJobs( sProduct );
	// ezt át lehetne rakni a DFG-vel egy szintre
	std::vector<std::string> vMostFrequentOperationSequence = MostFrequentOperationSequenceForProduct( BuildOperationSequencesForProduct( sProduct, mapJobs ) );
	std::map<std::string, AggregatedOperationData> mapOperations = AggregateOperations( mapJobs );
	Recipe sRecipe = BuildRecipe( sProduct, mapOperations, vMostFrequentOperationSequence );

	return sRecipe;
}