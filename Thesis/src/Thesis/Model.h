#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

enum EEventType
{
	T_PRODUCT,                                                 // Elkészített termék
	T_INPUT,                                                   // Felhasznált anyag
	T_SPINOFF,                                                 // Mellektermek
	T_WASTE,                                                   // Hulladek
	T_SCRAP,                                                   // Selejt
	T_LEFTOVER                                                 // Maradék
};

enum ETimeUnit
{
	UN_MONTH,                                                  // time
	UN_WEEK,                                                   // time
	UN_DAY,                                                    // time
	UN_HOUR,                                                   // time
	UN_MINUTE,                                                 // time
	UN_SECOND,                                                 // time
	UN_MILLISECOND                                             // time
};

enum EProductionMode
{
	PM_OWN_PRODCUTION,                                         // Saját gyártás
	PM_OUTSOURCING,                                            // Kihelyezett
	PM_BOTH                                                    // Mindketto
};

enum EBOMItemType
{
	BIT_INPUT,                               // Felhasznált anyag
	BIT_SPINOFF,                             // Mellektermek
	BIT_WASTE,                               // Hulladek
	BIT_LEFTOVER                             // Maradék
};

/*enum EWorkingType
{
	WT_NORMAL,                                                 // Normál
	WT_REPAIR,                                                 // Javítás
	WT_PREPARE,                                                // Elõkészítés
};*/

struct ProductionEvent
{
	std::string strProductId;
	std::string strTaskId;
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

struct ProductionInterval
{
	std::string strProductId;
	std::string strTaskId;
	std::string strOperationId;

	std::string strMachineId;

	time_t tTimeBeg;
	time_t tTimeEnd;

	double dOperationTime = 0;

	double dProducedQuantity = 0;
	double dScrapQuantity = 0;
};


struct Job
{
	std::string strOperationId;
	std::string strProductId;
	std::string strTaskId;

	int iEventCount = 0;

	int iOrder = -1;

	//time_t tBegin = {};
	time_t tEnd = {};

	double dPieceGood = 0.0;
	double dPieceScrap = 0.0;

	std::map<std::string, double> mapMaterialConsumptions;
	std::set<std::string> vUsedMachines;

	std::vector<ProductionInterval> vProductionIntervals;

	std::map<std::string, double> mapOperationTimesByMachine;
};

struct Operation
{
	std::string strOperationId;
	std::vector<ProductionEvent> vEvents;
	std::vector<ProductionTimeData> vProductionTimes;
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
	double       dPiece = 0.0;
	double       dBaseQuantity = 1.0;
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
	double      dOperationTime = 0.0;
	double      dBaseQuantity = 1.0;
	std::string strBaseQuantityUnitId;
	//int iMachineOperationTime;
};

struct RecipeItem
{
	std::string strId;
	std::string strRecipeId;
	std::string strOperationId;

	int             iOrder = -1;
	double          dBaseQuantity = 1.0;
	std::string     strBaseQuantityUnitId;
	double          dOperationTime =0.0;
	ETimeUnit       eOperationTimeUnit = UN_SECOND;
	EProductionMode eProductionMode = PM_OWN_PRODCUTION;;
	double          dRunningScrap = 0.0;

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

struct MachineInfo
{
	double dTotalOperationTime = 0.0;
	int iIntervalCount = 0;
	double dProducedQuantity = 0.0;
	double dScrapQuantity = 0.0;
};

struct AggregatedOperationData
{
	int iJobCount = 0;
	std::vector<int> vOperationOrders;
	std::string strOperationId;
    double dProducedQuantity = 0.0;
    double dScrapQuantity = 0.0;
    std::map<std::string, double> mapMaterials; // T_INPUT, T_SPINOFF, T_WASTE, T_LEFTOVER
	std::map<std::string, int> mapMaterialCounts;
    std::map<std::string, MachineInfo> mapMachineInfos;
    std::set<std::string> vMachines;
};