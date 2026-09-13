#include "LiteratureAlgorithm.h"


double tauvar;
double tauact;
double taudf;


// adott termékre megcsináljuk a DFG-t

void PreProcessEventLogs()
{

}

DirectlyFollowsGraph BuildDFG( vProductionEvents )
{

}

Recipe LiteratureAlgorithm::GenerateRecipeForProduct( const Product& sProduct )
{
	std::map<std::tuple<std::string, std::string>, Job> mapJobs = BaseAlgorithm::BuildJobs( sProduct );


}
