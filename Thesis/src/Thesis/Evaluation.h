#pragma once

#include <unordered_map>
#include <string>
#include "Model.h"

class Evaluation
{
public:
	static void CompareRecipes( const std::unordered_map<std::string, Recipe>& mapGeneratedRecipes, const std::unordered_map<std::string, Recipe>& mapReferenceRecipes );
};

