#include "DataLoader.h"
#include "CSVReader.h"
#include "Parser.h"

std::vector<ProductionEvent> DataLoader::LoadProductionEvents( const std::string& strFileName )
{
	std::vector<ProductionEvent> vProductionEvents;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		ProductionEvent sProductionEvent = parser.ParseProductionEvent( csvReader, iRow );
		vProductionEvents.push_back( sProductionEvent );
	}

	return vProductionEvents;
}

std::map<std::string, Product> DataLoader::BuildDataStructure( const std::vector<ProductionEvent>& vProductionEvents )
{
	std::map<std::string, Product> mapProducts;

	for( const ProductionEvent& sProductionEvent : vProductionEvents )
	{
		mapProducts[sProductionEvent.strProductId].mapTasks[sProductionEvent.strTaskId].mapOperations[sProductionEvent.strOperationId].vEvents.push_back( sProductionEvent );
	}

	return mapProducts;
}

std::vector<Recipe> DataLoader::LoadRecipes( const std::string& strFileName )
{
	std::vector<Recipe> vRecipes;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		Recipe sRecipe = parser.ParseRecipe( csvReader, iRow );
		vRecipes.push_back( sRecipe );
	}

	return vRecipes;
}

std::vector<RecipeItem> DataLoader::LoadRecipeItems( const std::string& strFileName )
{
	std::vector<RecipeItem> vRecipeItems;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		RecipeItem sRecipeItem = parser.ParseRecipeItem( csvReader, iRow );
		vRecipeItems.push_back( sRecipeItem );
	}

	return vRecipeItems;
}

std::vector<MachineDemand> DataLoader::LoadMachineDemands( const std::string& strFileName )
{
	std::vector<MachineDemand> vMachineDemands;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		MachineDemand sMachineDemand = parser.ParseMachineDemand( csvReader, iRow );
		vMachineDemands.push_back( sMachineDemand );
	}

	return vMachineDemands;
}

std::vector<MaterialDemand> DataLoader::LoadMaterialDemands( const std::string& strFileName )
{
	std::vector<MaterialDemand> vMaterialDemands;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		MaterialDemand sMaterialDemand = parser.ParseMaterialDemand( csvReader, iRow );
		vMaterialDemands.push_back( sMaterialDemand );
	}

	return vMaterialDemands;
}