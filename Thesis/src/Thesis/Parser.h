#pragma once

#include "Domain.h"
#include "CSVREader.h"

class Parser
{
public:
	ProductionEvent ParseProductionEvent( const CSVReader& csvReader, int iRow );
	Recipe          ParseRecipe(          const CSVReader& csvReader, int iRow );
	RecipeItem      ParseRecipeItem(      const CSVReader& csvReader, int iRow );
	MaterialDemand  ParseMaterialDemand(  const CSVReader& csvReader, int iRow );
	MachineDemand   ParseMachineDemand(   const CSVReader& csvReader, int iRow );
};

