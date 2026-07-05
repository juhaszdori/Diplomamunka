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

int main( int argc, char* argv[] )
{
	std::vector<ProductionEvent> vProductionEvents = DataLoader::LoadProductionEvents( "../../data/input/mf_march.csv" );

	PrintEvents( vProductionEvents );

	std::unordered_map<std::string, Product> mapProducts = DataLoader::BuildDataStructure( vProductionEvents );

	//egybbõl hasonlítsuk össze a recepteket amikor létrehoztuk vagy egyben az összeset egy függvényben
	std::unordered_map<std::string, Recipe> mapRecipes;

	for( const auto& pair : mapProducts )
	{
		Recipe sRecipe = BaseAlgorithm::GenerateRecipeForProduct( pair.second );
		mapRecipes[sRecipe.strProductId] = sRecipe;

		//cout << pair.first << endl;
	}

	std::unordered_map < std::string, Recipe> mapReferenceRecipes = DataLoader::LoadRecipeDatabase( "../../data/input" );

	Evaluation::CompareRecipes( mapRecipes, mapReferenceRecipes );

	return 0;
}
