#include "Parser.h"
#include "CSVReader.h"
#include "Model.h"
#include <sstream>
#include <iomanip>

time_t Parser::ParseTimestamp( const std::string& strTimestamp )
{
	std::istringstream ss( strTimestamp );
	std::tm tmTimeStamp = {};
	ss >> std::get_time( &tmTimeStamp, "%Y%m%d%H%M%S" );

	if( ss.fail() )
		throw std::runtime_error( "Parse failed" );

	return std::mktime( &tmTimeStamp );
}

ProductionEvent Parser::ParseProductionEvent( const CSVReader& csvReader, int iRow )
{
	ProductionEvent sProductionEvent = {};

	sProductionEvent.strTaskId         = csvReader.GetField( "task"        , iRow );
	sProductionEvent.strProductId      = csvReader.GetField( "product"     , iRow );
	sProductionEvent.strOperationId    = csvReader.GetField( "operation"   , iRow );
	sProductionEvent.strMaterialId     = csvReader.GetField( "material"    , iRow );
	sProductionEvent.strMachineId      = csvReader.GetField( "machine"     , iRow );
	sProductionEvent.strQuantityUnitId = csvReader.GetField( "quantityunit", iRow );
	
	sProductionEvent.dQuantity = std::stod( csvReader.GetField( "quantity", iRow ) );
	sProductionEvent.eEventType = static_cast<EEventType>( std::stoi( csvReader.GetField( "eventtype", iRow ) ) );

	sProductionEvent.tTimeStamp = ParseTimestamp( csvReader.GetField( "timestamp", iRow ) );

	return sProductionEvent;
}

ProductionTimeData Parser::ParseProductionTimeData( const CSVReader& csvReader, int iRow )
{
	ProductionTimeData sProductionTimeData = {};
	sProductionTimeData.strProductId   = csvReader.GetField( "product"  , iRow );
	sProductionTimeData.strTaskId      = csvReader.GetField( "task"     , iRow );
	sProductionTimeData.strOperationId = csvReader.GetField( "operation", iRow );
	sProductionTimeData.strMachineId   = csvReader.GetField( "machine"  , iRow );

	sProductionTimeData.tTimeBeg = ParseTimestamp( csvReader.GetField( "timebeg", iRow ) );
	sProductionTimeData.tTimeEnd = ParseTimestamp( csvReader.GetField( "timeend", iRow ) );

	return sProductionTimeData;
}

Recipe Parser::ParseRecipe( const CSVReader& csvReader, int iRow )
{
	Recipe sRecipe = {};

	sRecipe.strId        = csvReader.GetField( "id"     , iRow );
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
	sRecipeItem.iOrder         = std::stoi( csvReader.GetField( "order"           , iRow ) );
	sRecipeItem.dOperationTime = std::stod( csvReader.GetField( "operationtime"   , iRow ) );
	sRecipeItem.dBaseQuantity  = std::stod( csvReader.GetField( "basequantity"    , iRow ) );
	sRecipeItem.dRunningScrap  = std::stod( csvReader.GetField( "runningscrap"    , iRow ) );

	sRecipeItem.eOperationTimeUnit = static_cast<ETimeUnit>( std::stoi( csvReader.GetField( "operationtimeunit", iRow ) ) );
	sRecipeItem.eProductionMode    = static_cast<EProductionMode>( std::stoi( csvReader.GetField( "productionmode", iRow ) ) );

	return sRecipeItem;
}

MaterialDemand Parser::ParseMaterialDemand( const CSVReader& csvReader, int iRow )
{
	MaterialDemand sMaterialDemand = {};

	sMaterialDemand.strId                 =            csvReader.GetField( "id"              , iRow );
	sMaterialDemand.strRecipeItemId       =            csvReader.GetField( "recipeitem"      , iRow );
	sMaterialDemand.strMaterialId         =            csvReader.GetField( "material"        , iRow );
	sMaterialDemand.strBaseQuantityUnitId =            csvReader.GetField( "basequantityunit", iRow );
	sMaterialDemand.dPiece                = std::stod( csvReader.GetField( "piece"           , iRow ) );
	sMaterialDemand.dBaseQuantity         = std::stod( csvReader.GetField( "basequantity"    , iRow ) );

	sMaterialDemand.eType = static_cast<EBOMItemType>( std::stoi( csvReader.GetField( "type" , iRow ) ) );

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