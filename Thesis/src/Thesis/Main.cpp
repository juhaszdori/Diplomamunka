#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "CSVReader.h"
#include "Parser.h"
#include "Domain.h"

using namespace std;

/*vector<ProductionEvent> ReadInputCSV(const string& strFileName)
{
	vector<ProductionEvent> vProductionEvent;

	ifstream file( strFileName );

	if( !file.is_open() )
	{
		std::cerr << "Failed to open file: " << strFileName << std::endl;
		return vProductionEvent;
	}

	string strHeader;
	getline( file, strHeader );

	string strLine;
	while( getline( file, strLine ) )
	{
		stringstream ss( strLine );
		ProductionEvent event = {};
		string quantity_str;
		string event_type_str;
		string strDateTime;

		if (getline(ss, event.strTaskId, ',') && //az oszlop neve lesz
			getline(ss, event.strProductId, ',') &&
			getline(ss, event.strOperationId, ',') &&
			getline(ss, quantity_str, ',') &&
			getline(ss, event.strQuantityUnitId, ',') &&
			getline(ss, event.strMaterialId, ',') &&
			getline(ss, event_type_str, ',') &&
			getline(ss, event.strMachineId, ',') &&
			getline(ss, strDateTime, ','))
		{
			clean(quantity_str);
			clean(event_type_str);
			clean(strDateTime);
			clean( event.strTaskId 			);
			clean( event.strProductId 		);
			clean( event.strOperationId    	);
			clean( event.strQuantityUnitId	);
			clean( event.strMaterialId		);
			clean( event.strMachineId       );

			event.dQuantity = stod(quantity_str);
			event.eEventType = GetEventType(stoi(event_type_str));


			struct tm datetime;
			time_t timestamp;

			datetime.tm_year = 2023 - 1900; // Number of years since 1900
			datetime.tm_mon = 12 - 1; // Number of months since January
			datetime.tm_mday = 17;
			datetime.tm_hour = 12;
			datetime.tm_min = 30;
			datetime.tm_sec = 1;
			// Daylight Savings must be specified
			// -1 uses the computer's timezone setting
			datetime.tm_isdst = -1;

			timestamp = mktime(&datetime);

			cout << ctime(&timestamp);

			std::wstring date_time_format = L"%m/%d/%Y";
			std::wistringstream ss{ L"4/28/2022" };
			std::tm dt;

			ss >> std::get_time(&dt, date_time_format.c_str());

			std::time_t final_time;

			final_time = std::mktime(&dt);
			std::tm* ltm = localtime(&final_time);

			std::cout << "Year:" << 1900 + ltm->tm_year << std::endl;
			std::cout << "Month: " << 1 + ltm->tm_mon << std::endl;
			std::cout << "Day: " << ltm->tm_mday << std::endl;

			//std::tm t = {};
			std::istringstream ss( strDateTime );
			ss.imbue(std::locale("de_DE.utf-8"));
			ss >> std::get_time(&event.tmTimeStamp, "%Y%m%d%H%M%S");


			if (ss.fail())
				std::cout << "Parse failed\n";
			else
				std::put_time(&event.tmTimeStamp, "%c");
				//std::cout << std::put_time(&t, "%c") << '\n';

			vProductionEvent.push_back( event );
		}
	}

	file.close();

	return vProductionEvent;
}*/

struct MaterialConsumption // termelési jelentés felhasznált típusú tétel
{
	string strTaskId;
	string strMaterialId;
	double dUsedQuantity;
	double dProducedQuantity;
	string strProductId;
};

struct JobInfoItem
{
	string strTaskId;
	int iSequence;
	string strOperationId;
	tm dtStartTime; // min prodrep time
	tm dtEndTime;    // max
	vector<string> vMachines;
	vector<MaterialConsumption> vMaterials;
};

struct OperationForProduct {

	string strProductId;
	string strOperationId;
	int iSequence;
	double dOperationTime;
	vector<string> vMachines;
	//int occurrence_count;
};

std::vector<ProductionEvent> LoadProductionEvents( const std::string& strFileName )
{
	std::vector<ProductionEvent> vProductionEvents;

	CSVReader csvReader;
	csvReader.ReadCSV( strFileName );

	Parser parser;

	for( int iRow = 0; iRow < csvReader.GetRowCount(); iRow++ )
	{
		ProductionEvent sProductionEvent = parser.ParseProductionEvent( csvReader, iRow );
		vProductionEvents.push_back( sProductionEvent );
	}

	return vProductionEvents;
}

void BuildDataStructure( std::vector<ProductionEvent>& vProductionEvents )
{
	for( const ProductionEvent& sProductionEvent : vProductionEvents )
	{
		mapProducts[sProductionEvent.strProductId].mapTasks[sProductionEvent.strTaskId].mapOperations[sProductionEvent.strOperationId].vEvents.push_back(sProductionEvent);
	}
}

// std::map<std::string, Recipe> mapRecipes;
// for loop:
//	Recipe sRecipe = GenerateRecipe();
// mapRecipes[sRecipe.strId] = sRecipe

int main( int argc, char* argv[] )
{
	std::vector<ProductionEvent> vProductionEvents = LoadProductionEvents( "../../data/input/mf_march.csv" );
	BuildDataStructure( vProductionEvents );

	//routing
	for( const auto& pair : mapProducts )
	{
		string strProductId = pair.first;

		cout << strProductId << endl;
	}
		/*STaskInfoItem sSTaskInfoItemFindData;
		sSTaskInfoItemFindData.strTaskId = strTaskId;
		sSTaskInfoItemFindData.pOperation = pOperation;
		sSTaskInfoItemFindData.pMachine = pMachine;

		STaskInfoItem* pTaskInfoItem = (STaskInfoItem*)m_listTaskInfoItem.Find(&sSTaskInfoItemFindData);
		bool bIsNewItem = (pTaskInfoItem == NULL);
		if (bIsNewItem)
		{
			pTaskInfoItem = new STaskInfoItem;
			pTaskInfoItem->strTaskId = strTaskId;
			pTaskInfoItem->pOperation = pOperation;
			pTaskInfoItem->pMachine = pMachine;
			pTaskInfoItem->iIntervall = 0;
			pTaskInfoItem->nPieces = 0;
		}

		if (dtEnd > dtBeg)
			pTaskInfoItem->iIntervall += dtEnd.GetValue() - dtBeg.GetValue();

		if (mapProcessedJobs.Find(iJobId) == NULL)
		{
			pTaskInfoItem->nPieces += nPieceGood + nPieceScrap;
			mapProcessedJobs.Insert(iJobId, true);
		}

		if (bIsNewItem)
			m_listTaskInfoItem.InsertOrdered(pTaskInfoItem);
	}

	// fill operations time (FIXED: weighted average base)
	STaskInfoItem* pTaskInfoItem;

	TMap<Operation*, int64> mapTotalTime;
	TMap<Operation*, int64> mapTotalPieces;

	for (int iPos = 0; pTaskInfoItem = m_listTaskInfoItem.GetPosition(iPos); iPos++)
	{
		if (pTaskInfoItem->nPieces.GetInt64() <= 0)
			continue;

		int64 iTotalTime = pTaskInfoItem->iIntervall;
		int64 iTotalPieces = pTaskInfoItem->nPieces.GetInt64();

		mapTotalTime[pTaskInfoItem->pOperation] += iTotalTime;
		mapTotalPieces[pTaskInfoItem->pOperation] += iTotalPieces;
	}

	m_listTaskInfoItem.Resort();
		
	}

	for( const auto& pair : mTasks )
	{
		string strTaskId = pair.first;
		const vector<ProductionEvent>& records = pair.second;

		cout << "Task: " << strTaskId << endl;

		for (const auto& r : records)
		{
			cout << r.strTaskId << " " << r.dQuantity << endl;
		}
	}*/

	//routing

	
	/*for (const auto& pair : mTasks)
	{
		string strTaskId = pair.first;
		const vector<ProductionEvent>& records = pair.second;

		cout << "Task: " << strTaskId << endl;

		double dProducedQuantity = 0.0;


		for( const auto& r : records )
		{
			map<int, vector<ProductionEvent>> mJobs; //az összes productionevent ugyanahhoz a taskhoz ugyanahhoz a munkához  megnézni a dSuiteban hogy hogy össezsíti a munkára az adatokat

			for (int iPos = 0; iPos < vProductionEvent.size(); iPos++)
			{
				ProductionEvent ProductionEvent = vProductionEvent[iPos];

				mTasks[ProductionEvent.strTaskId].push_back(ProductionEvent);
			}
		}
	}*/


	
	/*for (const auto& r : vProductionEvent)
	{
		std::tm tm;

	#ifdef _WIN32
			localtime_s(&tm, &r.tTimeStamp);
	#else
			localtime_r(&r.tTimeStamp, &tm);
	#endif
		cout << r.strTaskId      << " "
			 << r.strProductId  << " "
			 << r.strOperationId  << " "
			 << r.dQuantity  << " "
			 << r.strQuantityUnitId  << " "
			 << r.strMaterialId  << " "
			 << r.eEventType  << " "
			 << r.strMachineId  << " "
			 << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
			<< endl;
	}*/

	return 0;
}
