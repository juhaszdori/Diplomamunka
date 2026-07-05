#include "evaluation.h"
#include <iostream>
#include <map>
#include "Model.h"

void CompareRecipes( const std::map<std::string, Recipe> mapGeneratedRecipes, std::map<std::string, Recipe> mapReferenceRecipes )
{
	for( const auto& r : mapGeneratedRecipes )
	{
		const std::string& strProductId = r.first;
		const Recipe& sGeneratedRecipe = r.second;
		auto itReferenceRecipe = mapReferenceRecipes.find( strProductId );
		if( itReferenceRecipe == mapReferenceRecipes.end() )
		{
			std::cout << "No reference recipe found for product: " << strProductId << std::endl;
			continue;
		}
		const Recipe& sReferenceRecipe = itReferenceRecipe->second;
		if( sGeneratedRecipe.vRecipeItems.size() != sReferenceRecipe.vRecipeItems.size() )
		{
			std::cout << "Mismatch in number of recipe items for product: " << strProductId << std::endl;
			continue;
		}
		for( size_t i = 0; i < sGeneratedRecipe.vRecipeItems.size(); ++i )
		{
			const RecipeItem& sGeneratedItem = sGeneratedRecipe.vRecipeItems[i];
			const RecipeItem& sReferenceItem = sReferenceRecipe.vRecipeItems[i];
			if( sGeneratedItem.strOperationId != sReferenceItem.strOperationId ||
				sGeneratedItem.dBaseQuantity != sReferenceItem.dBaseQuantity ||
				sGeneratedItem.strBaseQuantityUnitId != sReferenceItem.strBaseQuantityUnitId ||
				sGeneratedItem.dOperationTime != sReferenceItem.dOperationTime )
			{
				std::cout << "Mismatch in recipe item at index " << i << " for product: " << strProductId << std::endl;
			}
		}
	}
}