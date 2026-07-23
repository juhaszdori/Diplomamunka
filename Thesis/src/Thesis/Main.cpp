#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Model.h"
#include "DataLoader.h"
#include "BaseAlgorithm.h"
#include "Evaluation.h"

void PrintEvents( const std::vector<ProductionEvent>& vProductionEvents )
{
	for( const auto& r : vProductionEvents )
	{
		std::tm tm;

	#ifdef _WIN32
			localtime_s(&tm, &r.tTimeStamp);
	#else
			localtime_r(&r.tTimeStamp, &tm);
	#endif
		std::cout << r.strTaskId      << " "
			 << r.strProductId  << " "
			 << r.strOperationId  << " "
			 << r.dQuantity  << " "
			 << r.strQuantityUnitId  << " "
			 << r.strMaterialId  << " "
			 << r.eEventType  << " "
			 << r.strMachineId  << " "
			 << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
			<< std::endl;
	}
}

void PrintTimes( const std::vector<ProductionTimeData>& vProductionTimes )
{
	for( const auto& r : vProductionTimes )
	{
		std::tm tmBeg, tmEnd;
	#ifdef _WIN32
			localtime_s(&tmBeg, &r.tTimeBeg);
			localtime_s(&tmEnd, &r.tTimeEnd);
	#else
			localtime_r(&r.tTimeBeg, &tmBeg);
			localtime_r(&r.tTimeEnd, &tmEnd);
	#endif
		std::cout << r.strTaskId      << " "
			 << r.strProductId  << " "
			 << r.strOperationId  << " "
			 << r.strMachineId  << " "
			 << std::put_time(&tmBeg, "%Y-%m-%d %H:%M:%S")  << " "
			 << std::put_time(&tmEnd, "%Y-%m-%d %H:%M:%S")  << " "
			 << r.dOperationTime
			<< std::endl;
	}
}

int main( int argc, char* argv[] )
{
	std::vector<ProductionEvent>    vProductionEvents = DataLoader::LoadProductionEvents(  "../../data/input/events3.csv" );
	std::vector<ProductionTimeData> vProductionTimes  = DataLoader::LoadProductionTimeData( "../../data/input/times3.csv" );

	PrintEvents( vProductionEvents );
	PrintTimes( vProductionTimes );

	std::unordered_map<std::string, Product> mapProducts = DataLoader::BuildDataStructure( vProductionEvents, vProductionTimes );

	//egybbõl hasonlítsuk össze a recepteket amikor létrehoztuk vagy egyben az összeset egy függvényben
	std::unordered_map<std::string, Recipe> mapRecipes;

	for( const auto& pair : mapProducts )
	{
		const Product& sProduct = pair.second;

		//BaseAlgorithm::AttachProductionTimesToEvents();
		Recipe sRecipe = BaseAlgorithm::GenerateRecipeForProduct( sProduct );
		mapRecipes[sRecipe.strProductId] = sRecipe;

		std::cout << "Generated recipe for product: " << sProduct.strProductId << std::endl;

	}

	//std::unordered_map < std::string, Recipe> mapReferenceRecipes = DataLoader::LoadRecipeDatabase( "../../data/reference" );

	//Evaluation::CompareRecipes( mapRecipes, mapReferenceRecipes );

	return 0;
}
