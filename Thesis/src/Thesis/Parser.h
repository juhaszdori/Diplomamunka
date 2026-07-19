#pragma once

#include "Model.h"
#include "CSVReader.h"
#include <string>

class Parser
{
public:
	time_t             ParseTimestamp( const std::string& strTimestamp );

	ProductionEvent    ParseProductionEvent(    const CSVReader& csvReader, int iRow );
	ProductionTimeData ParseProductionTimeData( const CSVReader& csvReader, int iRow );
	Recipe             ParseRecipe(             const CSVReader& csvReader, int iRow );
	RecipeItem         ParseRecipeItem(         const CSVReader& csvReader, int iRow );
	MaterialDemand     ParseMaterialDemand(     const CSVReader& csvReader, int iRow );
	MachineDemand      ParseMachineDemand(      const CSVReader& csvReader, int iRow );
};

