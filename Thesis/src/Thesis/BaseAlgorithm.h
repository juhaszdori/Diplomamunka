#pragma once

#include "Model.h"

class BaseAlgorithm
{
public:
	static Recipe GenerateRecipeForProduct( const Product& sProduct );
	static std::vector<ProductionInterval> BuildProductionIntervals( const std::vector<ProductionEvent>& vProductionEvents, const std::vector<ProductionTimeData>& vProductionTimes );
	static int MostFrequentOrder( const std::vector<int>& vOrders );
	static Recipe BuildRecipe( const Product& sProduct, const std::map<std::string, AggregatedOperationData>& mapOperations );
	static std::map<std::string, AggregatedOperationData> AggregateOperations( const std::map<std::tuple<std::string, std::string>, Job>& mapJobs );
	static std::map<std::tuple<std::string, std::string>, Job> BuildJobs( const Product& sProduct );
};

