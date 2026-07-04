#pragma once

#include <string>
#include <vector>
#include <map>

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

	std::vector<std::string> vMachines;
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

std::map<std::string, Product> mapProducts;

EBOMItemType GetBOMItemType( int iBOMItemType )  //fix
{
	switch( iBOMItemType )
	{
		case 0:
			return BIT_INPUT;
		case 1:
			return BIT_SPINOFF;
		case 4:
			return BIT_WASTE;
		default:
			return BIT_LEFTOVER; //fix
	}
}

struct MaterialDemand
{		
	double       dRatio;
	std::string  strMaterialId;
	EBOMItemType eType;
	double       dPiece;
	double       dBaseQuantity;

	/*bool bMainRawMaterial;
	bool bAlternativeAllowed;
	bool bUsedforSpecificMachinesOnly;*/
};

struct MachineDemand
{
	std::string strMachineId;
	double      dOperationTime;
	double      dBaseQuantity;
	std::string strBaseQuantityUnitId;
	//int iMachineOperationTime;
};

struct RecipeItem
{
	std::string        strOperationId;
	int                iOrder;
	double             dBaseQuantity;
	std::string        strBaseQuantityUnitId;
	double             dOperationTime;
	EOperationTimeUnit eOperationTimeUnit;

	std::vector<MachineDemand>  vMachineDemands;
	std::vector<MaterialDemand> vMaterialDemands;
};

struct Recipe
{
	std::string strProductId;

	std::vector<RecipeItem> vRecipeItem;
};

EEventType GetEventType( int iEventType )
{
	switch( iEventType )
	{
		case 0:
			return T_PRODUCT;
		case 1:
			return T_INPUT;
		case 4:
			return T_SCRAP;
		default:
			return T_PRODUCT; //fix
	}
}