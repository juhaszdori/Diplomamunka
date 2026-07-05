#include "Model.h"

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