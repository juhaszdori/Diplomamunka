#pragma once

#include <string>
#include <map>
#include <vector>
#include "Model.h"

class DataLoader
{
public:
	static std::vector<ProductionEvent> LoadProductionEvents( const std::string& strFileName );
	static std::map<std::string, Product> BuildDataStructure( const std::vector<ProductionEvent>& vProductionEvents );

	static std::vector<Recipe> LoadRecipes( const std::string& strFileName );
	static std::vector<RecipeItem> LoadRecipeItems( const std::string& strFileName );
	static std::vector<MachineDemand> LoadMachineDemands( const std::string& strFileName );
	static std::vector<MaterialDemand> LoadMaterialDemands( const std::string& strFileName );
};

