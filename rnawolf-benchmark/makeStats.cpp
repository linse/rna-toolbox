#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <values.h>
#include <math.h>
using namespace std;



// g++ -Wall -O5 makeStats.cpp -o makeStats

// mc-fold:
// X-RAY <= 3A : 21680.138u 826.007s 6:28:24.26 96.5%    0+0k 0+0io 0pf+0w
// NMR :          5028.390u 158.513s 1:37:02.62 89.0%    0+0k 0+0io 0pf+0w


map< string, int > mapPDBcodes;
set< string > setNMR;


class CDisplay {
	public:
		static float fround( float x, int d ) {
			// rounding rules:
			// <= X.4 -> X
			// >= X.6 -> X+1
			// X.5 -> if X is even then X, else X+1

			x = x + pow( (float)10, (float)-d-2 );
			float d10 = pow( (float)10, (float)d );
			int a = ( (int)( x * pow( (float)10, (float)d   ) ) ) % 10;
			int b = ( (int)( x * pow( (float)10, (float)d+1 ) ) ) % 10;

			//cout << "x = " << x << ", a = " << a << " , b = " << b << "\n";

			if( b <= 4 )
				return floor( x * d10 ) / d10;

			if( b >= 6 )
				return ceil( x * d10 ) / d10;

			// b == 5
			if( ( a % 2 ) == 0 )
				return floor( x * d10 ) / d10;

			return floor( 1 + x * d10 ) / d10;
		}

		static char szNumber[ 5000 ];
		static char * MakeNumber( const char * szFormat, float fValue, int d = -1 )
		{
			if( d >= 0 )
				fValue = CDisplay::fround( fValue, d );

			sprintf( CDisplay::szNumber, szFormat, fValue );

			return szNumber;
		}
};
char CDisplay::szNumber[ 5000 ];



void MakeStructure( const char * szStr, set< pair< int, int > > & setStr ) {
	stack< int > stkStr;

	int iMax = strlen( szStr );
	for( int i = 0; i < iMax; ++i ) {
		if( szStr[ i ] == ' ' )
			break;

		if( szStr[ i ] == '(' )
			stkStr.push( i );

		if( szStr[ i ] == ')' ) {
			setStr.insert( pair< int, int >( stkStr.top(), i ) );
			stkStr.pop();
		}
	}
}



set< pair< int, int > > SetIntersection( set< pair< int, int > > & set1,
		set< pair< int, int > > & set2 ) {
	set< pair< int, int > > setInter;
	insert_iterator< set< pair< int, int > > > iter( setInter, setInter.begin() );

	set_intersection( set1.begin(), set1.end(),
			set2.begin(), set2.end(),
			iter );

	return setInter;
}



set< pair< int, int > > SetDiff( set< pair< int, int > > & set1,
		set< pair< int, int > > & set2 ) {
	set< pair< int, int > > setDiff;
	insert_iterator< set< pair< int, int > > > iter( setDiff, setDiff.begin() );

	set_difference( set1.begin(), set1.end(),
			set2.begin(), set2.end(),
			iter );

	return setDiff;
}



void LoadNMR( const char * szFile ) {
	ifstream hFile( szFile );
	string strBuffer;
	while( getline( hFile, strBuffer ) )
		setNMR.insert( strBuffer.c_str() );
}



char szShowNC[ 5000 ];
char * ShowNC( const char * szSequence,
		const char * szDotBracket,
		set< pair< int, int > > & setPairs ) {
	strcpy( szShowNC, szDotBracket );

	set< pair< int, int > >::iterator pos;
	for( pos = setPairs.begin(); pos != setPairs.end(); ++pos ) {
		char cType1 = szSequence[ pos->first  ];
		char cType2 = szSequence[ pos->second ];
		if( ! ( ( ( 'G' == cType1 ) && ( 'C' == cType2 ) ) ||
					( ( 'C' == cType1 ) && ( 'G' == cType2 ) ) ||
					( ( 'A' == cType1 ) && ( 'U' == cType2 ) ) ||
					( ( 'U' == cType1 ) && ( 'A' == cType2 ) ) ||
					( ( 'G' == cType1 ) && ( 'U' == cType2 ) ) ||
					( ( 'U' == cType1 ) && ( 'G' == cType2 ) ) ) ) {
			szShowNC[ pos->first  ] = '[';
			szShowNC[ pos->second ] = ']';
		}
	}

	return szShowNC;
}



int main( int argc, char * argv [] ) {
	if( argc < 2 ) {
		cout << "Usage:\n\t" << argv[0] << " <stem_loops.algo.fsa>  [N best] [NMR list file]\n";
		return -1;
	}


	ifstream hFile( argv[1] );
	if( !hFile ) {
		cout << "Failed to open file: `" << argv[1] << "`\n";
		return -2;
	}


	int nBest = 1;
	if( argc >= 3 )
		nBest = atoi( argv[2] );

	if( argc >= 4 )
		LoadNMR( argv[3] );


	int nbHairpins  = 0;
	int nbCanonicals[2] = { 0, 0 };
	int nbPairings[2]   = { 0, 0 };
	ofstream hStemSize( "stem_size.data" );

	int nbTP     = 0;
	int nbFP     = 0;
	int nbFN     = 0;

	int nbTPCano = 0;
	int nbTPNonC = 0;
	int nbFPCano = 0;
	int nbFPNonC = 0;
	int nbFNCano = 0;
	int nbFNNonC = 0;


	string strBuffer;
	while( getline( hFile, strBuffer ) ) {
		if( strBuffer.find( ">" ) == 0 ) {
			++nbHairpins;

			cout << strBuffer << "\n";

			char * szBuffer = strdup( strBuffer.c_str() + 1 );

			char * szPDB = strtok( szBuffer, " \t" );
			mapPDBcodes[ szPDB ] += 1;

			int bNMR = 0;
			set< string >::iterator nmr;
			nmr = setNMR.find( szPDB );
			if( nmr != setNMR.end() )
				bNMR = 1;

			char * szStemSize  = strtok( NULL, " \t" );
			/*char * szStemRange =*/ strtok( NULL, " \t" );
			char * szStemStruc = strtok( NULL, " \t" );
			char * szStemPairs = strtok( NULL, " \t" );


			set< pair< int, int > > setNative;
			MakeStructure( szStemStruc, setNative );


			if( NULL != szStemSize ) {
				hStemSize << szStemSize << "\n";
			}


			if( NULL != szStemPairs ) {
				char * szNum  = strtok( szStemPairs, "/" );
				char * szDNum = strtok( NULL, "$" );

				if( ( NULL != szNum ) && ( NULL != szDNum ) ) {
					nbCanonicals[ bNMR ] += atoi( szNum  );
					nbPairings[ bNMR ]   += atoi( szDNum  );
				}
			}

			free( szBuffer );


			// get the sequence
			string strSequence;
			getline( hFile, strSequence );
			cout << strSequence << "\n";


			// get the N best predicted structures
			int nUsed = nBest;
			string strStructure;
			unsigned int iMaxGood = 0;
			unsigned int iMinFP   = MAXINT;
			unsigned int iMinFN   = MAXINT;
			set< pair< int, int > > setPredicted;
			do {
				streampos inpos = hFile.tellg();
				getline( hFile, strBuffer );

				// check if dot-bracket
				if( ( strBuffer.find( "(" ) != 0 ) &&
						( strBuffer.find( "." ) != 0 ) ) {
					// if not then backup
					hFile.seekg( inpos );
					break;
				}

				set< pair< int, int > > setTmpPredicted;
				MakeStructure( strBuffer.c_str(), setTmpPredicted );

				// get first set
				set< pair< int, int > > setTmpInter;
				copy( setNative.begin(), setNative.end(),
						inserter( setTmpInter, setTmpInter.begin() ) );

				// intersection with the other
				setTmpInter = SetIntersection( setTmpPredicted, setTmpInter );
				unsigned int nbTmpGood = setTmpInter.size();


				set< pair< int, int > > setTmpFP;
				setTmpFP = SetDiff( setTmpPredicted, setNative );
				unsigned int nbTmpFP = setTmpFP.size();

				set< pair< int, int > > setTmpFN;
				setTmpFN = SetDiff( setNative, setTmpPredicted );
				unsigned int nbTmpFN = setTmpFN.size();


				// if better goods then take it
				if( nUsed > 0 ) {
					if( ( ( nbTmpGood  > iMaxGood ) ) ||
							( ( nbTmpGood == iMaxGood ) && ( nbTmpFN  < iMinFN ) ) ||
							( ( nbTmpGood == iMaxGood ) && ( nbTmpFN == iMinFN ) && ( nbTmpFP < iMinFP ) ) )
						// else, if equally good then take lower false negative
						// else, take lower false positive
					{
						--nUsed;
						strStructure = strBuffer;
						iMaxGood = nbTmpGood;
						iMinFP   = nbTmpFP;
						iMinFN   = nbTmpFN;
						setPredicted.clear();
						copy( setTmpPredicted.begin(), setTmpPredicted.end(),
								inserter( setPredicted, setPredicted.begin() ) );
					}
				}
			}
			while( true );


			cout << ShowNC( strSequence.c_str(),
					szStemStruc,
					setNative ) << "   native\n";
			cout << ShowNC( strSequence.c_str(),
					strStructure.c_str(),
					setPredicted ) << "\n";

			set< pair< int, int > > setInter;
			copy( setNative.begin(), setNative.end(),
					inserter( setInter, setInter.begin() ) );

			// intersection with the other
			setInter = SetIntersection( setPredicted, setInter );

			nbTP += setInter.size();

			int iFPCano = 0;
			int iFPNonC = 0;
			int iFNCano = 0;
			int iFNNonC = 0;


			// within the good ones, count the canonicals
			set< pair< int, int > >::iterator pos;
			for( pos = setInter.begin(); pos != setInter.end(); ++pos ) {
				char cType1 = strSequence[ pos->first  ];
				char cType2 = strSequence[ pos->second ];
				if( ( ( 'G' == cType1 ) && ( 'C' == cType2 ) ) ||
						( ( 'C' == cType1 ) && ( 'G' == cType2 ) ) ||
						( ( 'A' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'A' == cType2 ) ) ||
						( ( 'G' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'G' == cType2 ) ) )
					++nbTPCano;
				else
					++nbTPNonC;
			}



			set< pair< int, int > > setFP;
			setFP = SetDiff( setPredicted, setNative );
			nbFP += setFP.size();

			for( pos = setFP.begin(); pos != setFP.end(); ++pos ) {
				char cType1 = strSequence[ pos->first  ];
				char cType2 = strSequence[ pos->second ];
				if( ( ( 'G' == cType1 ) && ( 'C' == cType2 ) ) ||
						( ( 'C' == cType1 ) && ( 'G' == cType2 ) ) ||
						( ( 'A' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'A' == cType2 ) ) ||
						( ( 'G' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'G' == cType2 ) ) ) {
					++iFPCano;
					++nbFPCano;
				}
				else {
					++iFPNonC;
					++nbFPNonC;
				}
			}



			set< pair< int, int > > setFN;
			setFN = SetDiff( setNative, setPredicted );
			nbFN += setFN.size();

			for( pos = setFN.begin(); pos != setFN.end(); ++pos ) {
				char cType1 = strSequence[ pos->first  ];
				char cType2 = strSequence[ pos->second ];
				if( ( ( 'G' == cType1 ) && ( 'C' == cType2 ) ) ||
						( ( 'C' == cType1 ) && ( 'G' == cType2 ) ) ||
						( ( 'A' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'A' == cType2 ) ) ||
						( ( 'G' == cType1 ) && ( 'U' == cType2 ) ) ||
						( ( 'U' == cType1 ) && ( 'G' == cType2 ) ) ) {
					++iFNCano;
					++nbFNCano;
				}
				else {
					++iFNNonC;
					++nbFNNonC;
				}
			}


			cout << "\tCano: FP: " << iFPCano << " FN: " << iFNCano << "\n"
				<< "\tNonC: FP: " << iFPNonC << " FN: " << iFNNonC << "\n";
		}
	}


	cout << "Total number of hairpins: " << nbHairpins << "\n";

	int nbPDB = 0;
	map< string, int >::iterator pos;
	for( pos = mapPDBcodes.begin(); pos != mapPDBcodes.end(); ++pos ) {
		++nbPDB;
	}
	cout << "Total number of PDB codes: " << nbPDB << "\n";

	int nbPredic = nbTP + nbFP;

	int nbNative = nbTPCano + nbTPNonC + nbFNCano + nbFNNonC;
	if( nbNative != ( nbTP + nbFN ) ) {
		cout << "------ ATTENTION! ------\n";
	}

	int nbCano   = nbTPCano + nbFNCano;

	int nbNonC   = nbNative - nbCano;



	cout << "Total number of base-pairings: " << nbNative << "\n"
		<< "Total number of canonicals: " << nbCano
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbCano / (float)nbNative, 1 ) << "%)\n";
	cout << "Total number of non-cano  : " << nbNonC
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbNonC / (float)nbNative, 1 ) << "%)\n";


	cout << "\nTP+FP: " << nbPredic
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbPredic/(float)nbNative, 1 ) << "%)\n";



	cout << "TP: " << nbTP
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbTP/(float)nbNative, 1 ) << "%)\n";

	cout << "\tCano: " << nbTPCano << " ("
		<< CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbTPCano/(float)nbCano, 1 ) << "%)\n";
	cout << "\tnonC: " << nbTPNonC << " ("
		<< CDisplay::MakeNumber
		( "%6.1f", 100.0 * (float)nbTPNonC/(float)nbNonC, 1 ) << "%)\n";



	cout << "FP: " << nbFP
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFP/(float)nbNative, 1 ) << "%)" << "\n";

	cout << "\tCano: " << nbFPCano << " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFPCano/(float)nbFP, 1 ) << "%)" << "\n";
	cout << "\tnonC: " << nbFPNonC << " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFPNonC/(float)nbFP, 1 ) << "%)" << "\n";



	cout << "FN: " << nbFN
		<< " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFN/(float)nbNative, 1 ) << "%)"
		<< "\n";

	cout << "\tCano: " << nbFNCano << " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFNCano/(float)nbFN, 1 ) << "%)" << "\n";
	cout << "\tnonC: " << nbFNNonC << " (" << CDisplay::MakeNumber
		( "%6.1f", 100.0*(float)nbFNNonC/(float)nbFN, 1 ) << "%)" << "\n";


	float fMatthews = 100.0 * sqrt( ( (float)nbTP / (float)( nbTP + nbFN ) ) *
			( (float)nbTP / (float)( nbTP + nbFP ) ) );
	cout << "Matthews: " << CDisplay::MakeNumber( "%6.1f", fMatthews, 1 ) << "%\n";


	float fPPV = 100.0 * sqrt( ( (float)nbTP / (float)( nbTP + nbFP ) ) );
	cout << "PPV     : " << CDisplay::MakeNumber( "%6.1f", fPPV, 1 ) << "%\n";

	float fSen = 100.0 * sqrt( ( (float)nbTP / (float)( nbTP + nbFN ) ) );
	cout << "Sensitiv: " << CDisplay::MakeNumber( "%6.1f", fSen, 1 ) << "%\n";

	return 0;
}
