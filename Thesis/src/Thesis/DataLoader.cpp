#include "DataLoader.h"
#include "CSVReader.h"
#include "Parser.h"

//templatekkel megoldani a függvényeket és a builddatastructure-t is, hogy ne legyen sok kódismétlés

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

std::unordered_map<std::string, Product> DataLoader::BuildDataStructure( const std::vector<ProductionEvent>& vProductionEvents )
{
	std::unordered_map<std::string, Product> mapProducts;

	for( const ProductionEvent& sProductionEvent : vProductionEvents )
	{
		mapProducts[sProductionEvent.strProductId].mapTasks[sProductionEvent.strTaskId].mapOperations[sProductionEvent.strOperationId].vEvents.push_back( sProductionEvent );
	}

	return mapProducts;
}

std::unordered_map<std::string, Recipe> DataLoader::LoadRecipes( const std::string& strFileName )
{
	std::unordered_map<std::string, Recipe> mapRecipes;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		Recipe sRecipe = parser.ParseRecipe( csvReader, iRow );
		mapRecipes[sRecipe.strId] = sRecipe;
	}

	return mapRecipes;
}

std::unordered_map<std::string, RecipeItem> DataLoader::LoadRecipeItems( const std::string& strFileName )
{
	std::unordered_map<std::string, RecipeItem> mapRecipeItems;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		RecipeItem sRecipeItem = parser.ParseRecipeItem( csvReader, iRow );
		mapRecipeItems[sRecipeItem.strId] = sRecipeItem;
	}

	return mapRecipeItems;
}

std::unordered_map<std::string, MachineDemand> DataLoader::LoadMachineDemands( const std::string& strFileName )
{
	std::unordered_map<std::string, MachineDemand> mapMachineDemands;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		MachineDemand sMachineDemand = parser.ParseMachineDemand( csvReader, iRow );
		mapMachineDemands[sMachineDemand.strId] = sMachineDemand;
	}

	return mapMachineDemands;
}

std::unordered_map<std::string, MaterialDemand> DataLoader::LoadMaterialDemands( const std::string& strFileName )
{
	std::unordered_map<std::string, MaterialDemand> mapMaterialDemands;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		MaterialDemand sMaterialDemand = parser.ParseMaterialDemand( csvReader, iRow );
		mapMaterialDemands[sMaterialDemand.strId] = sMaterialDemand;
	}

	return mapMaterialDemands;
}

std::unordered_map<std::string, Recipe> DataLoader::LoadRecipeDatabase( const std::string& folder )
{
	const std::string recipesFile         = folder + "/mf_march_recipes.csv";
	const std::string recipeItemsFile     = folder + "/mf_march_recipeitems.csv";
	const std::string machineDemandsFile  = folder + "/mf_march_machinedemands.csv";
	const std::string materialDemandsFile = folder + "/mf_march_materialdemands.csv";

	std::unordered_map<std::string, Recipe>         mapRecipes         = DataLoader::LoadRecipes(         recipesFile         );
	std::unordered_map<std::string, RecipeItem>     mapRecipeItems     = DataLoader::LoadRecipeItems(     recipeItemsFile     );
	std::unordered_map<std::string, MachineDemand>  mapMachineDemands  = DataLoader::LoadMachineDemands(  machineDemandsFile  );
	std::unordered_map<std::string, MaterialDemand> mapMaterialDemands = DataLoader::LoadMaterialDemands( materialDemandsFile );

	LinkRecipeStructure( mapRecipes, mapRecipeItems, mapMachineDemands, mapMaterialDemands );

	return mapRecipes;
}

void DataLoader::LinkRecipeStructure(
	std::unordered_map<std::string, Recipe>& mapRecipes,
	std::unordered_map<std::string, RecipeItem>& mapRecipeItems,
	std::unordered_map<std::string, MachineDemand>& mapMachineDemands,
	std::unordered_map<std::string, MaterialDemand>& mapMaterialDemands )
{
	for( const auto& pair : mapMachineDemands )
	{
		const MachineDemand& machineDemand = pair.second;
		auto it = mapRecipeItems.find( machineDemand.strRecipeItemId );
		if( it != mapRecipeItems.end() )
			it->second.vMachineDemands.push_back(machineDemand);
	}

	for( const auto& pair : mapMaterialDemands )
	{
		const MaterialDemand& materialDemand = pair.second;
		auto it = mapRecipeItems.find( materialDemand.strRecipeItemId );
		if( it != mapRecipeItems.end() )
			it->second.vMaterialDemands.push_back( materialDemand );
	}

	for( const auto& pair : mapRecipeItems )
	{
		const RecipeItem& recipeItem = pair.second;
		//mapRecipes.at( recipeItem.strRecipeId ).vRecipeItems.push_back( recipeItem );
		auto it = mapRecipes.find( recipeItem.strRecipeId );
		if( it != mapRecipes.end() )
			it->second.vRecipeItems.push_back( recipeItem );
	}
}