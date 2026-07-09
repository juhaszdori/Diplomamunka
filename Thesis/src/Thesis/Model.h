#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

enum EOperationTimeUnit {
	TU_SECOND = 0
};

enum EEventType
{
	T_PRODUCT = 0,
	T_INPUT = 1,
	T_SCRAP = 2
};

enum EBOMItemType
{
	BIT_INPUT,                               // Felhasznált anyag
	BIT_SPINOFF,                             // Mellektermek
	BIT_WASTE,                               // Hulladek
	BIT_LEFTOVER                             // Maradék
};

struct ProductionEvent
{
	std::string strTaskId;
	std::string strProductId;
	std::string strOperationId;
	double      dQuantity;
	std::string strQuantityUnitId;
	std::string strMaterialId;
	EEventType  eEventType;
	std::string strMachineId;
	time_t      tTimeStamp;
	//std::tm tmTimeStamp = {};
	//double dOperationTime;
};

struct ProductionTimeData
{
	std::string strProductId;
	std::string strTaskId;
	std::string strOperationId;
	std::string strMachineId;
	time_t      tTimeBeg;
	time_t      tTimeEnd;

	double      dOperationTime;
};

struct Job
{
	std::string strOperationId;
	std::string strProductId;
	std::string strTaskId;

	time_t tBegin = {};
	time_t tEnd = {};

	int iInterval = 0;

	double dProducedQuantity = 0.0;
	double dScrapQuantity = 0.0;

	std::map<std::string, double> mMaterials;

	std::set<std::string> vMachines;
};

struct Operation
{
	std::string strOperationId;
	std::vector<ProductionEvent> vEvents;
};

struct Task
{
	std::string strTaskId;
	std::map<std::string, Operation> mapOperations;
};

struct Product
{
	std::string strProductId;
	std::map<std::string, Task> mapTasks;
};

struct MaterialDemand
{
	std::string  strId;
	std::string  strRecipeItemId;
	std::string  strMaterialId;
	EBOMItemType eType;
	double       dPiece;
	double       dBaseQuantity;
	std::string strBaseQuantityUnitId;

	/*bool bMainRawMaterial;
	bool bAlternativeAllowed;
	bool bUsedforSpecificMachinesOnly;*/
};

struct MachineDemand
{
	std::string strId;
	std::string strRecipeItemId;
	std::string strMachineId;
	double      dOperationTime;
	double      dBaseQuantity;
	std::string strBaseQuantityUnitId;
	//int iMachineOperationTime;
};

struct RecipeItem
{
	std::string strId;
	std::string strRecipeId;
	std::string strOperationId;

	int                iOrder;
	double             dBaseQuantity;
	std::string        strBaseQuantityUnitId;
	double             dOperationTime;
	EOperationTimeUnit eOperationTimeUnit;

	//E               type = false;

	std::vector<MachineDemand>  vMachineDemands;
	std::vector<MaterialDemand> vMaterialDemands;
};

struct Recipe
{
	std::string strId;
	std::string strProductId;
	bool        bDefault = false;

	std::vector<RecipeItem> vRecipeItems;
};

struct ProductRecipes
{
	std::vector<Recipe> vRecipes;
};

//vagy std::map<std::string, std::vector<Recipe>> mapRecipesByProduct;

EEventType GetEventType( int iEventType );
EBOMItemType GetBOMItemType( int iBOMItemType );
