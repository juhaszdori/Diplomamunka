#include "BaseAlgorithm.h"
#include <iostream>

Recipe BaseAlgorithm::GenerateRecipeForProduct( const Product& sProduct )
{
	Recipe sRecipe;

	sRecipe.strProductId = sProduct.strProductId;

	std::map<std::tuple<std::string, std::string>, Job> mapJobs;

	//végig megyünk a termék összes gyártásán
	for( const auto& task : sProduct.mapTasks )
	{
		std::string strTaskId = task.first;
		const Task& sTask = task.second;

		std::cout << "Task: " << strTaskId << std::endl;

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

				if( event.eEventType == T_PRODUCT && event.strProductId == event.strMaterialId )
					sJob.dPieceGood += event.dQuantity;
				else if( event.eEventType == T_SCRAP )
					sJob.dPieceScrap += event.dQuantity;
				else if( event.eEventType == T_INPUT && event.strProductId != event.strMaterialId )
					sJob.mapMaterialConsumptions[event.strMaterialId] += abs( event.dQuantity );
				
				//sJob.mMaterials.insert( { event.strMaterialId, event.dQuantity } );

				sJob.vUsedMachines.insert( event.strMachineId );
			}
		}
	}

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

		sAggregatedOperation.strOperationId = sJob.strOperationId;
		sAggregatedOperation.iJobCount++;

		sAggregatedOperation.dProducedQuantity += sJob.dPieceGood;
		sAggregatedOperation.dScrapQuantity += sJob.dPieceScrap;

		sAggregatedOperation.vMachines.insert( sJob.vUsedMachines.begin(), sJob.vUsedMachines.end() );

		for( const auto& materialPair : sJob.mapMaterialConsumptions )
		{
			const std::string& strMaterialId = materialPair.first;
			double dUsedQuantity = materialPair.second;

			sAggregatedOperation.mapMaterials[strMaterialId] += dUsedQuantity;
			sAggregatedOperation.mapMaterialCounts[strMaterialId]++;
		}
	}

	for( const auto& operationPair : mapOperations )
	{
		const AggregatedOperationData& sAggregatedOperation = operationPair.second;

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
		sRecipeItem.strOperationId = sAggregatedOperation.strOperationId;
		sRecipeItem.dBaseQuantity = 1.0;
		sRecipeItem.dRunningScrap = dAveragePieceScrap / dAveragePieceGood; // megnézni hogy arány vagy szám

		for( const auto& materialPair : sAggregatedOperation.mapMaterials )
		{
			double dUsedQuantity = materialPair.second;
			const std::string& strMaterialId = materialPair.first;
			int iNumMaterialOccurrences = sAggregatedOperation.mapMaterialCounts[strMaterialId];

			double dAverageUsedQuantity = dUsedQuantity / iNumMaterialOccurrences;
			
			MaterialDemand sMaterialDemand;
			sMaterialDemand.strMaterialId = strMaterialId;
			sMaterialDemand.eType = BIT_INPUT;
			sMaterialDemand.dBaseQuantity = 1.0;
			if( sAggregatedOperation.dProducedQuantity > 0 )
				sMaterialDemand.dPiece = dAverageUsedQuantity / dAveragePieceGood;
			sRecipeItem.vMaterialDemands.push_back( sMaterialDemand );
		}

		for( const auto& machineId : sAggregatedOperation.vMachines )
		{
			MachineDemand sMachineDemand;
			sMachineDemand.strMachineId = machineId;
			sMachineDemand.dBaseQuantity = 1.0;
			sRecipeItem.vMachineDemands.push_back( sMachineDemand );
		}

		sRecipe.vRecipeItems.push_back( sRecipeItem );
	}

	return sRecipe;
}