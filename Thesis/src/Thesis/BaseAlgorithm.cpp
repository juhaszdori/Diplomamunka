#include "BaseAlgorithm.h"
#include <iostream>

struct MaterialConsumption // termelési jelentés felhasznált típusú tétel
{
	std::string strTaskId;
	std::string strMaterialId;
	double dUsedQuantity;
	double dProducedQuantity;
	std::string strProductId;
};

Recipe BaseAlgorithm::GenerateRecipeForProduct( const Product& sProduct )
{
	Recipe sRecipe;

	//routing

	for( const auto& task : sProduct.mapTasks )
	{
		std::string strTaskId = task.first;
		const Task& sTask = task.second;

		std::cout << "Task: " << strTaskId << std::endl;

		
		double dProducedQuantity = 0.0;

		std::map<std::string, Job> mapJobs;  // task structnak odaadni?

		for( const auto& operation : sTask.mapOperations )
		{
			std::string strOperationId = operation.first;
			const Operation& sOperation = operation.second;

			std::cout << "Operation: " << strOperationId << std::endl;

			Job sJob;
			
			//az összes productionevent ugyanahhoz a taskhoz ugyanahhoz a munkához  megnézni a dSuiteban hogy hogy össezsíti a munkára az adatokat
			for( const auto& event : sOperation.vEvents )
			{
				std::cout << "ProductionReportItem: " << event.strMaterialId << " " << event.dQuantity << std::endl << std::endl;

				if( event.eEventType == T_PRODUCT )
					sJob.dProducedQuantity += event.dQuantity; // scap?
				else if( event.eEventType == T_SCRAP )
					sJob.dScrapQuantity += event.dQuantity;
				else if( event.eEventType == T_INPUT )
					sJob.mMaterials.insert( { event.strMaterialId, event.dQuantity } );



				sJob.vMachines.insert( event.strMachineId );

				//if( event.tTimeStamp > dtBe g)
					//sJob->iIntervall += dtEnd.GetValue() - dtBeg.GetValue();
			}
		}
	}

	return sRecipe;
}