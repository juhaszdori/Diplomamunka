#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Model.h"

class DataLoader
{
public:
	static std::vector<ProductionEvent> LoadProductionEvents(   const std::string& strFileName );
	static std::vector<ProductionEvent> LoadProductionTimeData( const std::string& strFileName );
	static std::unordered_map<std::string, Product> BuildDataStructure( const std::vector<ProductionEvent>& vProductionEvents );

	static std::unordered_map<std::string, Recipe>         LoadRecipes(         const std::string& strFileName );
	static std::unordered_map<std::string, RecipeItem>     LoadRecipeItems(     const std::string& strFileName );
	static std::unordered_map<std::string, MachineDemand>  LoadMachineDemands(  const std::string& strFileName );
	static std::unordered_map<std::string, MaterialDemand> LoadMaterialDemands( const std::string& strFileName );

	static std::unordered_map<std::string, Recipe> LoadRecipeDatabase( const std::string& folder );

	static void LinkRecipeStructure(
		std::unordered_map<std::string, Recipe>& mapRecipes,
		std::unordered_map<std::string, RecipeItem>& mapRecipeItems,
		std::unordered_map<std::string, MachineDemand>& mapMachineDemands,
		std::unordered_map<std::string, MaterialDemand>& mapMaterialDemands );
};

