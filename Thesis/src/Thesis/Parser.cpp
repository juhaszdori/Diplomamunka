#include "Parser.h"
#include "CSVReader.h"
#include "Model.h"
#include <sstream>
#include <iomanip>

ProductionEvent Parser::ParseProductionEvent( const CSVReader& csvReader, int iRow )
{
	ProductionEvent sProductionEvent = {};

	sProductionEvent.strTaskId         = csvReader.GetField( "task"     , iRow );
	sProductionEvent.strProductId      = csvReader.GetField( "product"  , iRow );
	sProductionEvent.strOperationId    = csvReader.GetField( "operation", iRow );
	sProductionEvent.strMaterialId     = csvReader.GetField( "material" , iRow );
	sProductionEvent.strMachineId      = csvReader.GetField( "machine"  , iRow );
	sProductionEvent.strQuantityUnitId = csvReader.GetField( "unit"     , iRow );
	
	sProductionEvent.dQuantity = std::stod( csvReader.GetField( "quantity", iRow ) );
	sProductionEvent.eEventType = GetEventType( std::stoi( csvReader.GetField( "event_type", iRow ) ) );

	std::istringstream ss( csvReader.GetField( "dt_timestamp", iRow ) );
	std::tm tmTimeStamp = {};
	ss >> std::get_time( &tmTimeStamp, "%Y%m%d%H%M%S" );

	if( ss.fail() )
		throw std::runtime_error( "Parse failed" );

	sProductionEvent.tTimeStamp = std::mktime( &tmTimeStamp );

	return sProductionEvent;
}

Recipe Parser::ParseRecipe( const CSVReader& csvReader, int iRow )
{
	Recipe sRecipe = {};

	sRecipe.strId        = csvReader.GetField( "id"     ,  iRow );
	sRecipe.strProductId = csvReader.GetField( "product", iRow );
	sRecipe.bDefault     = csvReader.GetField( "default", iRow ) == "True";

	return sRecipe;
}

RecipeItem Parser::ParseRecipeItem( const CSVReader& csvReader, int iRow )
{
	RecipeItem sRecipeItem = {};

	sRecipeItem.strId                     = csvReader.GetField( "id"              , iRow );
	sRecipeItem.strRecipeId               = csvReader.GetField( "recipe"          , iRow );
	sRecipeItem.strOperationId            = csvReader.GetField( "operation"       , iRow );
	sRecipeItem.strBaseQuantityUnitId     = csvReader.GetField( "basequantityunit", iRow );
	sRecipeItem.bSubcontracting           = csvReader.GetField( "subcontracting"  , iRow ) == "True"; //fix
	sRecipeItem.iOrder         = std::stoi( csvReader.GetField( "order"           , iRow ) );
	sRecipeItem.dOperationTime = std::stod( csvReader.GetField( "operationtime"   , iRow ) );
	sRecipeItem.dBaseQuantity  = std::stod( csvReader.GetField( "basequantity"    , iRow ) );
	//sRecipeItem.eOperationTimeUnit = GetTimeUnit( std::stoi( csvReader.GetField( "operationtimeunit", iRow ) ) );

	return sRecipeItem;
}

MaterialDemand Parser::ParseMaterialDemand( const CSVReader& csvReader, int iRow )
{
	MaterialDemand sMaterialDemand = {};

	sMaterialDemand.strId                 =            csvReader.GetField( "id"          , iRow );
	sMaterialDemand.strRecipeItemId       =            csvReader.GetField( "recipeitem"  , iRow );
	sMaterialDemand.strMaterialId         =            csvReader.GetField( "material"    , iRow );
	sMaterialDemand.dRatio                = std::stod( csvReader.GetField( "ratio"       , iRow ) );
	sMaterialDemand.dPiece                = std::stod( csvReader.GetField( "piece"       , iRow ) );
	sMaterialDemand.dBaseQuantity         = std::stod( csvReader.GetField( "basequantity", iRow ) );
	sMaterialDemand.eType = GetBOMItemType( std::stoi( csvReader.GetField( "type"        , iRow ) ) );

	return sMaterialDemand;
}

MachineDemand Parser::ParseMachineDemand( const CSVReader& csvReader, int iRow )
{
	MachineDemand sMachineDemand = {};

	sMachineDemand.strId                     = csvReader.GetField( "id"              , iRow );
	sMachineDemand.strRecipeItemId           = csvReader.GetField( "recipeitem"      , iRow );
	sMachineDemand.strMachineId              = csvReader.GetField( "machine"         , iRow );
	sMachineDemand.strBaseQuantityUnitId     = csvReader.GetField( "basequantityunit", iRow );
	sMachineDemand.dBaseQuantity  = std::stod( csvReader.GetField( "basequantity"    , iRow ) );
	sMachineDemand.dOperationTime = std::stod( csvReader.GetField( "operationtime"   , iRow ) );

	return sMachineDemand;
}