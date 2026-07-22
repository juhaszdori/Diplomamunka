#pragma once

#include "Model.h"

class BaseAlgorithm
{
public:
	static Recipe GenerateRecipeForProduct( const Product& sProduct );
	static std::vector<ProductionInterval> BuildProductionIntervals( const std::vector<ProductionEvent>& vProductionEvents, const std::vector<ProductionTimeData>& vProductionTimes );
};

