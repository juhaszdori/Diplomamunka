#pragma once

#include "Model.h"
#include <utility>

// Kozvetlen-koveteses graf (directly-follows graph, DFG).
// A Karnok-Monostori (2011) fele muveleti utvonalgraf ezen alapul: a csucsok a muveletek,
// az elek az egy gyartason belul kozvetlenul egymast koveto muveletparok.
struct DirectlyFollowsGraph
{
	std::map<std::string, int>                            mapNodeCounts;   // muvelet -> hany gyartasban fordult elo
	std::map<std::pair<std::string, std::string>, int>    mapEdgeCounts;   // (elozo, kovetkezo) -> hanyszor kovette
	std::map<std::string, int>                            mapStartCounts;  // muvelet -> hanyszor volt elso a gyartasban
	std::map<std::string, int>                            mapEndCounts;    // muvelet -> hanyszor volt utolso a gyartasban
};

class LiteratureAlgorithm
{
public:
	static Recipe GenerateRecipeForProduct( const Product& sProduct );

	static DirectlyFollowsGraph     BuildDfg( const std::vector<std::vector<std::string>>& vTraces );
	static DirectlyFollowsGraph     PruneDfg( const DirectlyFollowsGraph& sDfg, double dMinRelativeOccurrence );
	static std::vector<std::string> OperationSequenceFromDfg( const DirectlyFollowsGraph& sDfg );
};
