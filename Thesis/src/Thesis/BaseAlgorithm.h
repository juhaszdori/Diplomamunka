#pragma once

#include "Model.h"

class BaseAlgorithm
{
public:
	static Recipe GenerateRecipeForProduct( const Product& sProduct );
	static std::vector<ProductionInterval> BuildProductionIntervals( const std::vector<ProductionEvent>& vProductionEvents, const std::vector<ProductionTimeData>& vProductionTimes );
	static int MostFrequentOrder( const std::vector<int>& vOrders );
	static std::vector<std::string> MostFrequentOperationSequenceForProduct( const std::vector<std::vector<std::string>>& vSequences );
	static Recipe BuildRecipe( const Product& sProduct, const std::map<std::string, AggregatedOperationData>& mapOperations, std::vector<std::string>& vMostFrequentOperationSequence );
	static std::map<std::string, AggregatedOperationData> AggregateOperations( const std::map<std::tuple<std::string, std::string>, Job>& mapJobs );
	static std::map<std::tuple<std::string, std::string>, Job> BuildJobs( const Product& sProduct );
	static std::vector<std::vector<std::string>> BuildOperationSequencesForProduct( const Product& sProduct, std::map<std::tuple<std::string, std::string>, Job>& mapJobs );
};

