#include "RecipeExport.h"
#include <sstream>

RecipeExport::RecipeExport()
	: m_cDelimiter( ',' )
	, m_bOpened( false )
{
}

RecipeExport::~RecipeExport()
{
	Close();
}

void RecipeExport::SetDelimiter( char cDelimiter )
{
	m_cDelimiter = cDelimiter;
}

char RecipeExport::GetDelimiter() const
{
	return m_cDelimiter;
}

bool RecipeExport::IsOpened() const
{
	return m_bOpened;
}

void RecipeExport::Close()
{
	if( m_file.is_open() )
	{
		m_file.close();
	}

	m_bOpened = false;
}

void RecipeExport::OpenCSV( const std::string& strFileName )
{
	Close();

	m_file.open( strFileName, std::ios::out | std::ios::trunc );

	if( !m_file.is_open() )
	{
		throw std::runtime_error( "Failed to open file: " + strFileName );
	}

	m_bOpened = true;
}

std::string RecipeExport::EscapeField( const std::string& strField ) const
{
	std::string strEscaped = strField;
	bool bNeedsQuotes = false;

	if( strEscaped.find( m_cDelimiter ) != std::string::npos ||
		strEscaped.find( '"' ) != std::string::npos ||
		strEscaped.find( '\n' ) != std::string::npos ||
		strEscaped.find( '\r' ) != std::string::npos )
	{
		bNeedsQuotes = true;
	}

	std::size_t pos = 0;
	while( ( pos = strEscaped.find( '"', pos ) ) != std::string::npos )
	{
		strEscaped.insert( pos, 1, '"' );
		pos += 2;
	}

	if( bNeedsQuotes )
	{
		strEscaped = '"' + strEscaped + '"';
	}

	return strEscaped;
}

void RecipeExport::WriteField( const std::string& strField )
{
	m_file << EscapeField( strField );
}

void RecipeExport::WriteHeader( const std::vector<std::string>& vColumns )
{
	for( std::size_t i = 0; i < vColumns.size(); ++i )
	{
		if( i > 0 )
			m_file << m_cDelimiter;
		
		WriteField( vColumns[i] );
	}

	m_file << '\n';
}

void RecipeExport::WriteFields( const std::vector<std::string>& vFields )
{
	for( std::size_t i = 0; i < vFields.size(); ++i )
	{
		if( i > 0 )
			m_file << m_cDelimiter;

		WriteField( vFields[i] );
	}

	m_file << '\n';
}

void RecipeExport::WriteLine( const Recipe& sRecipe )
{
	WriteFields(
	{
		sRecipe.strId,
		sRecipe.strProductId,
		sRecipe.bDefault ? "True" : "False"
	} );
}

void RecipeExport::WriteLine( const RecipeItem& sRecipeItem )
{
	WriteFields(
	{
		sRecipeItem.strId,
		std::to_string( sRecipeItem.iOrder ),
		sRecipeItem.strOperationId,
		sRecipeItem.strRecipeId,
		std::to_string( sRecipeItem.dOperationTime ),
		std::to_string( static_cast<int>( sRecipeItem.eOperationTimeUnit ) ),
		std::to_string( sRecipeItem.dBaseQuantity ),
		sRecipeItem.strBaseQuantityUnitId,
		std::to_string( static_cast<int>( sRecipeItem.eProductionMode ) ),
		std::to_string( sRecipeItem.dRunningScrap )
	} );
}

void RecipeExport::WriteLine( const MachineDemand& sMachineDemand )
{
	WriteFields(
	{
		sMachineDemand.strId,
		sMachineDemand.strRecipeItemId,
		sMachineDemand.strMachineId,
		std::to_string( sMachineDemand.dOperationTime ),
		std::to_string( sMachineDemand.dBaseQuantity ),
		sMachineDemand.strBaseQuantityUnitId
	} );
}

void RecipeExport::WriteLine( const MaterialDemand& sMaterialDemand )
{
	WriteFields(
	{
		sMaterialDemand.strId,
		sMaterialDemand.strRecipeItemId,
		sMaterialDemand.strMaterialId,
		std::to_string( sMaterialDemand.dPiece ),
		std::to_string( sMaterialDemand.dBaseQuantity ),
		sMaterialDemand.strBaseQuantityUnitId,
		std::to_string( static_cast<int>( sMaterialDemand.eType ) )
	} );
}

void RecipeExport::WriteRecipes( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes )
{
	OpenCSV( strFileName );

	WriteHeader( { "id", "product", "default" } );

	for( const auto& pair : mapRecipes )
	{
		WriteLine( pair.second );
	}

	Close();
}

void RecipeExport::WriteRecipeItems( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes )
{
	OpenCSV( strFileName );

	WriteHeader( { "id", "order", "operation", "recipe", "operationtime", "operationtimeunit", "basequantity", "basequantityunit", "productionmode", "runningscrap" } );

	for( const auto& pair : mapRecipes )
	{
		for( const RecipeItem& sRecipeItem : pair.second.vRecipeItems )
		{
			WriteLine( sRecipeItem );
		}
	}

	Close();
}

void RecipeExport::WriteMachineDemands( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes )
{
	OpenCSV( strFileName );

	WriteHeader( { "id", "recipeitem", "machine", "operationtime", "basequantity", "basequantityunit" } );

	for( const auto& pair : mapRecipes )
	{
		for( const RecipeItem& sRecipeItem : pair.second.vRecipeItems )
		{
			for( const MachineDemand& sMachineDemand : sRecipeItem.vMachineDemands )
			{
				WriteLine( sMachineDemand );
			}
		}
	}

	Close();
}

void RecipeExport::WriteMaterialDemands( const std::string& strFileName, const std::unordered_map<std::string, Recipe>& mapRecipes )
{
	OpenCSV( strFileName );

	WriteHeader( { "id", "recipeitem", "material", "piece", "basequantity", "basequantityunit", "type" } );

	for( const auto& pair : mapRecipes )
	{
		for( const RecipeItem& sRecipeItem : pair.second.vRecipeItems )
		{
			for( const MaterialDemand& sMaterialDemand : sRecipeItem.vMaterialDemands )
			{
				WriteLine( sMaterialDemand );
			}
		}
	}

	Close();
}
