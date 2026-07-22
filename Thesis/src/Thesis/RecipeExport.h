#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Model.h"

class RecipeExport
{
public:
	RecipeExport();
	~RecipeExport();

	void SetDelimiter( char cDelimiter );
	char GetDelimiter() const;

	void Close();
	bool IsOpened() const;

	void OpenCSV( const std::string& strFileName );

	void WriteFields( const std::vector<std::string>& vFields );
	
	void WriteLine( const Recipe& sRecipe );
	void WriteLine( const RecipeItem& sRecipeItem );
	void WriteLine( const MachineDemand& sMachineDemand );
	void WriteLine( const MaterialDemand& sMaterialDemand );

	void WriteRecipes( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes );
	void WriteRecipeItems( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes );
	void WriteMachineDemands( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes );
	void WriteMaterialDemands( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes );

private:
	void WriteHeader( const std::vector<std::string>& vColumns );
	void WriteField( const std::string& strField );
	std::string EscapeField( const std::string& strField ) const;

private:
	char m_cDelimiter;
	std::ofstream m_file;
	bool m_bOpened;
};

