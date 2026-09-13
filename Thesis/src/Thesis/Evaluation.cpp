#include "Evaluation.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace
{
	// egy muvelet osszehasonlithato modellje: gepek/anyagok az ertekeikkel
	struct OperationModel
	{
		std::map<std::string, double> mapMachineTimes;    // gepId  -> operationtime
		std::map<std::string, double> mapMaterialPieces;  // anyagId -> piece
	};

	// A receptelem idoegyseget masodpercre valtja at. A generalt oldal mindig
	// masodpercben szamol (DataLoader: difftime), az ERP-referencia viszont
	// tulnyomoreszt percben tarolja -- kozos egyseg nelkul 60x-os elteres jon ki.
	double SecondsPerUnit( ETimeUnit eUnit )
	{
		switch( eUnit )
		{
			case UN_MONTH:       return 30.0 * 24.0 * 3600.0;   // kozelites: 30 napos honap
			case UN_WEEK:        return 7.0 * 24.0 * 3600.0;
			case UN_DAY:         return 24.0 * 3600.0;
			case UN_HOUR:        return 3600.0;
			case UN_MINUTE:      return 60.0;
			case UN_SECOND:      return 1.0;
			case UN_MILLISECOND: return 0.001;
		}

		return 1.0;
	}

	// receptbol keszit egy { operationId -> OperationModel } lekepezest.
	// Az id-kat szandekosan eldobjuk (gen. es ref. oldalon maskepp neznek ki),
	// csak a tartalmat hasonlitjuk. Ha egy muvelet tobbszor szerepel, osszevonjuk.
	std::map<std::string, OperationModel> BuildModel( const Recipe& sRecipe )
	{
		std::map<std::string, OperationModel> mapModel;

		for( const RecipeItem& sItem : sRecipe.vRecipeItems )
		{
			// A kihelyezett (outsourcing) muvelethez nincs sajat gyartasi esemeny,
			// ezert azt nem is generaljuk -- kihagyjuk az osszehasonlitasbol.
			if( sItem.eProductionMode == PM_OUTSOURCING )
				continue;

			OperationModel& sOperation = mapModel[sItem.strOperationId];

			// A gepido a receptelem idoegysegeben ertendo, ami a generalt es a
			// referencia oldalon kulonbozik -- mindkettot masodpercre hozzuk.
			const double dSecondsPerUnit = SecondsPerUnit( sItem.eOperationTimeUnit );

			// A referencia a piece/operationtime erteket egy basequantity-hez adja meg
			// (pl. 1 anyag / 700 termek). A generalt oldal mindig basequantity=1, ezert
			// mindket oldalt a sajat basequantity-jevel normalizaljuk a fajlagos ertekre.
			// A degeneralt (basequantity <= 0) sorokat kihagyjuk.
			for( const MachineDemand& sMachine : sItem.vMachineDemands )
			{
				if( sMachine.dBaseQuantity > 0 )
					sOperation.mapMachineTimes[sMachine.strMachineId] = sMachine.dOperationTime * dSecondsPerUnit / sMachine.dBaseQuantity;
			}

			for( const MaterialDemand& sMaterial : sItem.vMaterialDemands )
			{
				if( sMaterial.dBaseQuantity > 0 )
					sOperation.mapMaterialPieces[sMaterial.strMaterialId] = sMaterial.dPiece / sMaterial.dBaseQuantity;
			}
		}

		return mapModel;
	}

	// a muveletek sorrendje iOrder szerint (outsourcing kihagyva, ismetlodo muvelet egyszer).
	// Csak a muveletek RELATIV sorrendjet nezzuk, nem a nyers iOrder ertekeket.
	std::vector<std::string> BuildOrderedOperations( const Recipe& sRecipe )
	{
		std::vector<std::pair<int, std::string>> vItems;
		for( const RecipeItem& sItem : sRecipe.vRecipeItems )
		{
			if( sItem.eProductionMode == PM_OUTSOURCING )
				continue;

			vItems.push_back( std::make_pair( sItem.iOrder, sItem.strOperationId ) );
		}

		std::sort( vItems.begin(), vItems.end(),
			[]( const std::pair<int, std::string>& a, const std::pair<int, std::string>& b )
			{
				if( a.first != b.first )
					return a.first < b.first;
				return a.second < b.second;
			} );

		std::vector<std::string> vOperations;
		std::set<std::string> setSeen;
		for( const auto& item : vItems )
		{
			if( setSeen.insert( item.second ).second )
				vOperations.push_back( item.second );
		}

		return vOperations;
	}

	// halmaz-atfedes a kulcsokra: |metszet| / |unio|
	template<typename TMap>
	double Jaccard( const TMap& mapA, const TMap& mapB )
	{
		if( mapA.empty() && mapB.empty() )
			return 1.0;

		std::size_t iIntersection = 0;
		for( const auto& pair : mapA )
			if( mapB.count( pair.first ) )
				++iIntersection;

		std::size_t iUnion = mapA.size() + mapB.size() - iIntersection;
		return iUnion ? static_cast<double>( iIntersection ) / iUnion : 1.0;
	}

	// halmaz-osszevetes zavarossagi ertekei a generalt (=becsult) es a referencia (=igaz) kozott.
	struct Confusion
	{
		long iTP = 0;   // igaz pozitiv: mindket halmazban benne van
		long iFP = 0;   // hamis pozitiv: csak a generaltban (folosleges)
		long iFN = 0;   // hamis negativ: csak a referenciaban (hianyzo)

		double Precision() const { return ( iTP + iFP ) ? static_cast<double>( iTP ) / ( iTP + iFP ) : 0.0; }
		double Recall()    const { return ( iTP + iFN ) ? static_cast<double>( iTP ) / ( iTP + iFN ) : 0.0; }
		double F1() const
		{
			double dP = Precision(), dR = Recall();
			return ( dP + dR > 0.0 ) ? 2.0 * dP * dR / ( dP + dR ) : 0.0;
		}
	};

	// a generalt es referencia kulcshalmazbol gyujti a TP/FP/FN-t (mikro-atlaghoz)
	template<typename TMapGen, typename TMapRef>
	void Accumulate( Confusion& sConfusion, const TMapGen& mapGen, const TMapRef& mapRef )
	{
		for( const auto& pair : mapGen )
		{
			if( mapRef.count( pair.first ) )
				++sConfusion.iTP;
			else
				++sConfusion.iFP;
		}

		for( const auto& pair : mapRef )
		{
			if( !mapGen.count( pair.first ) )
				++sConfusion.iFN;
		}
	}

	double RelativeError( double dGenerated, double dReference )
	{
		double dDenominator = std::max( std::abs( dReference ), 1e-9 );
		return std::abs( dGenerated - dReference ) / dDenominator;
	}

	double Average( const std::vector<double>& vValues )
	{
		if( vValues.empty() )
			return 0.0;

		double dSum = 0.0;
		for( double dValue : vValues )
			dSum += dValue;

		return dSum / vValues.size();
	}

	double Median( std::vector<double> vValues )
	{
		if( vValues.empty() )
			return 0.0;

		std::sort( vValues.begin(), vValues.end() );
		std::size_t n = vValues.size();
		return ( n % 2 ) ? vValues[n / 2] : ( vValues[n / 2 - 1] + vValues[n / 2] ) / 2.0;
	}

	// azon elemek aranya, amelyek relativ hibaja <= dThreshold
	double WithinRatio( const std::vector<double>& vErrors, double dThreshold )
	{
		if( vErrors.empty() )
			return 0.0;

		std::size_t iCount = 0;
		for( double dError : vErrors )
			if( dError <= dThreshold )
				++iCount;

		return static_cast<double>( iCount ) / vErrors.size();
	}
}

void Evaluation::CompareRecipes( const std::unordered_map<std::string, Recipe>& mapGeneratedRecipes, const std::unordered_map<std::string, Recipe>& mapReferenceRecipes )
{
	// A referencia termekenkent tobb receptet is tartalmazhat (default = true/false).
	// Termekenkent a default receptet valasztjuk alapigazsagnak; ha nincs, az elsot.
	std::map<std::string, Recipe> mapReferenceByProduct;
	for( const auto& pair : mapReferenceRecipes )
	{
		const Recipe& sReferenceRecipe = pair.second;

		auto it = mapReferenceByProduct.find( sReferenceRecipe.strProductId );
		if( it == mapReferenceByProduct.end() )
			mapReferenceByProduct[sReferenceRecipe.strProductId] = sReferenceRecipe;
		else if( sReferenceRecipe.bDefault )
			it->second = sReferenceRecipe;
	}

	int iCommon = 0;
	int iMissing = 0;
	int iExactOperationMatch = 0;
	int iExactOrderMatch = 0;

	std::vector<double> vOperationJaccard;
	std::vector<double> vMaterialJaccard;
	std::vector<double> vMachineJaccard;
	std::vector<double> vMachineTimeError;
	std::vector<double> vMaterialPieceError;
	std::vector<double> vMachineTimeAbsError;   // abszolut hiba (MAE-hez)
	std::vector<double> vMaterialPieceAbsError; // abszolut hiba (MAE-hez)

	// mikro-atlagolt precision/recall/F1 (osszes TP/FP/FN globalisan)
	Confusion sOperationConfusion;
	Confusion sMaterialConfusion;
	Confusion sMachineConfusion;

	for( const auto& pair : mapGeneratedRecipes )
	{
		const Recipe& sGenerated = pair.second;
		const std::string& strProductId = sGenerated.strProductId;

		auto itReference = mapReferenceByProduct.find( strProductId );
		if( itReference == mapReferenceByProduct.end() )
		{
			std::cout << "No reference recipe for product: " << strProductId << std::endl;
			++iMissing;
			continue;
		}

		++iCommon;

		std::map<std::string, OperationModel> mapGen = BuildModel( sGenerated );
		std::map<std::string, OperationModel> mapRef = BuildModel( itReference->second );

		double dOperationJaccard = Jaccard( mapGen, mapRef );
		vOperationJaccard.push_back( dOperationJaccard );

		Accumulate( sOperationConfusion, mapGen, mapRef );

		// pontos muvelet-halmaz egyezes
		bool bExact = ( mapGen.size() == mapRef.size() );
		if( bExact )
		{
			for( const auto& operationPair : mapGen )
			{
				if( !mapRef.count( operationPair.first ) )
				{
					bExact = false;
					break;
				}
			}
		}
		if( bExact )
			++iExactOperationMatch;

		// pontos sorrend-egyezes: a muveletek iOrder szerinti szekvenciaja azonos-e.
		// Ez csak akkor lehet igaz, ha a muvelet-halmaz is egyezik.
		bool bExactOrder = ( BuildOrderedOperations( sGenerated ) == BuildOrderedOperations( itReference->second ) );
		if( bExactOrder )
			++iExactOrderMatch;

		// a kozos muveletekre hasonlitjuk az anyag- es gephalmazokat, majd a numerikus ertekeket
		for( const auto& operationPair : mapGen )
		{
			auto itReferenceOperation = mapRef.find( operationPair.first );
			if( itReferenceOperation == mapRef.end() )
				continue;

			const OperationModel& sGenOperation = operationPair.second;
			const OperationModel& sRefOperation = itReferenceOperation->second;

			vMaterialJaccard.push_back( Jaccard( sGenOperation.mapMaterialPieces, sRefOperation.mapMaterialPieces ) );
			vMachineJaccard.push_back( Jaccard( sGenOperation.mapMachineTimes, sRefOperation.mapMachineTimes ) );

			Accumulate( sMaterialConfusion, sGenOperation.mapMaterialPieces, sRefOperation.mapMaterialPieces );
			Accumulate( sMachineConfusion,  sGenOperation.mapMachineTimes,   sRefOperation.mapMachineTimes );

			for( const auto& machinePair : sGenOperation.mapMachineTimes )
			{
				auto itReferenceMachine = sRefOperation.mapMachineTimes.find( machinePair.first );
				if( itReferenceMachine != sRefOperation.mapMachineTimes.end() )
				{
					vMachineTimeError.push_back( RelativeError( machinePair.second, itReferenceMachine->second ) );
					vMachineTimeAbsError.push_back( std::abs( machinePair.second - itReferenceMachine->second ) );
				}
			}

			for( const auto& materialPair : sGenOperation.mapMaterialPieces )
			{
				auto itReferenceMaterial = sRefOperation.mapMaterialPieces.find( materialPair.first );
				if( itReferenceMaterial != sRefOperation.mapMaterialPieces.end() )
				{
					vMaterialPieceError.push_back( RelativeError( materialPair.second, itReferenceMaterial->second ) );
					vMaterialPieceAbsError.push_back( std::abs( materialPair.second - itReferenceMaterial->second ) );
				}
			}
		}

		std::cout << "Product " << strProductId
			<< " | operations gen/ref: " << mapGen.size() << "/" << mapRef.size()
			<< " | op-Jaccard: " << std::fixed << std::setprecision( 2 ) << dOperationJaccard
			<< ( bExact ? "  [exact-set]" : "" )
			<< ( bExactOrder ? "  [exact-order]" : "" )
			<< std::endl;
	}

	// -------- aggregalt osszegzes --------
	std::cout << "\n===== EVALUATION SUMMARY =====\n";
	std::cout << "Generated recipes : " << mapGeneratedRecipes.size() << "\n";
	std::cout << "Reference products: " << mapReferenceByProduct.size() << "\n";
	std::cout << "Common products   : " << iCommon << "\n";
	std::cout << "Missing reference : " << iMissing << "\n\n";

	if( iCommon > 0 )
	{
		std::cout << std::fixed << std::setprecision( 3 );
		std::cout << "-- Structure --\n";
		std::cout << "Exact operation-set match : " << iExactOperationMatch << "/" << iCommon
			<< " (" << std::setprecision( 1 ) << ( 100.0 * iExactOperationMatch / iCommon ) << "%)\n";
		std::cout << "Exact operation-order match: " << iExactOrderMatch << "/" << iCommon
			<< " (" << std::setprecision( 1 ) << ( 100.0 * iExactOrderMatch / iCommon ) << "%)\n";
		std::cout << std::setprecision( 3 );
		std::cout << "Avg operation-set Jaccard : " << Average( vOperationJaccard ) << "\n";
		std::cout << "Avg material-set Jaccard  : " << Average( vMaterialJaccard ) << "\n";
		std::cout << "Avg machine-set Jaccard   : " << Average( vMachineJaccard ) << "\n\n";

		std::cout << "-- Set match (micro precision / recall / F1) --\n";
		std::cout << "Operations: P=" << sOperationConfusion.Precision()
			<< " R=" << sOperationConfusion.Recall()
			<< " F1=" << sOperationConfusion.F1()
			<< "  (TP=" << sOperationConfusion.iTP << " FP=" << sOperationConfusion.iFP << " FN=" << sOperationConfusion.iFN << ")\n";
		std::cout << "Materials : P=" << sMaterialConfusion.Precision()
			<< " R=" << sMaterialConfusion.Recall()
			<< " F1=" << sMaterialConfusion.F1()
			<< "  (TP=" << sMaterialConfusion.iTP << " FP=" << sMaterialConfusion.iFP << " FN=" << sMaterialConfusion.iFN << ")\n";
		std::cout << "Machines  : P=" << sMachineConfusion.Precision()
			<< " R=" << sMachineConfusion.Recall()
			<< " F1=" << sMachineConfusion.F1()
			<< "  (TP=" << sMachineConfusion.iTP << " FP=" << sMachineConfusion.iFP << " FN=" << sMachineConfusion.iFN << ")\n\n";

		std::cout << "-- Numeric (matched entries) --\n";
		std::cout << "Machine operationtime pairs: " << vMachineTimeError.size()
			<< " | MAE: " << Average( vMachineTimeAbsError )
			<< " | median rel.err: " << Median( vMachineTimeError )
			<< " | within 10%: " << std::setprecision( 1 ) << ( 100.0 * WithinRatio( vMachineTimeError, 0.10 ) ) << "%\n";
		std::cout << std::setprecision( 3 );
		std::cout << "Material piece pairs       : " << vMaterialPieceError.size()
			<< " | MAE: " << Average( vMaterialPieceAbsError )
			<< " | median rel.err: " << Median( vMaterialPieceError )
			<< " | within 10%: " << std::setprecision( 1 ) << ( 100.0 * WithinRatio( vMaterialPieceError, 0.10 ) ) << "%\n";
	}

	std::cout << "==============================" << std::endl;
}
