
//	RapidFit Headers
#include "ScanStudies.h"
#include "FitAssembler.h"
#include "PhysicsParameter.h"
#include "ParameterSet.h"
#include "ScanParam.h"
#include "MinimiserConfiguration.h"
#include "FitFunctionConfiguration.h"
#include "PhysicsBottle.h"
#include "OutputConfiguration.h"
#include "PDFWithData.h"
//	System Headers
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace::std;


//  Interface for internal calls
void ScanStudies::DoScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet* BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, ScanParam* Wanted_Param, FitResultVector* output_interface, int OutputLevel )
{
	FunctionConfig->SetIntegratorTest( false );

	double uplim = Wanted_Param->GetMax();
	double lolim = Wanted_Param->GetMin();
	double npoints = Wanted_Param->GetPoints();
	string scanName = Wanted_Param->GetName();

	//	cout << "Performing Scan for the parameter " << scanName << endl ;

	// Get a pointer to the physics parameter to be scanned and fix it	
	// CAREFUL:  this must be reset as it was at the end.
	PhysicsParameter * scanParameter = BottleParameters->GetPhysicsParameter(scanName);
	double originalValue = scanParameter->GetBlindedValue( ) ;
	string originalType = scanParameter->GetType( ) ;
	scanParameter->SetType( "Fixed" ) ;

	// Need to set up a loop , fixing the scan parameter at each point
	double deltaScan;

	for( int si=0; si<int(npoints); ++si)
	{
		cout << "\n\nSINGLE SCAN NUMBER\t\t" << si+1 << "\t\tOF\t\t" <<int(npoints)<< endl<<endl;
		// Set scan parameter value
		if( int(npoints)!=1 ) deltaScan = (uplim-lolim) / (npoints-1.) ;
		else deltaScan=0;
		double scanVal = lolim+deltaScan*si;
		scanParameter->SetBlindedValue( scanVal ) ;

		cout << "Fitting at:\t" << scanName << "=" << scanVal << endl;

		output_interface->StartStopwatch();

		FitResult* scanStepResult=NULL;

		try{
			//	Use the SafeFit as this always returns something when a PDF has been written to throw not exit
			//	Do a scan point fit
			scanStepResult = FitAssembler::DoSafeFit( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, OutputLevel );
		}
		catch( int e )
		{
			cerr << "Caught Scan Error: " << e << endl;
			exit(-987);
		}
		catch( ... )
		{
			cerr << "Caught Unknown Scan Error" << endl;
			exit(-986);
		}

		int retries = 0;
		int left_right = 1;
		int wiggle_step_num = 0;
		double scanVal_orig = scanVal;
		double wiggle_step_size = deltaScan/20.;


		while( scanStepResult->GetFitStatus() != 3 )
		{

			if( retries != 1 )
			{
				cout << "\n\t\t\tRETRYING FIT" << endl;
				scanVal = scanVal_orig;
				++retries;
			}
			else
			{
				if( int(wiggle_step_num/2)*2 == wiggle_step_num )	// Even
				{
					left_right = 1;
				}
				else							// Odd
				{
					left_right = -1;
				}
										//  0/2 and 1/2 are both 0 as an integer

				scanVal = scanVal_orig + (double)left_right * wiggle_step_size * (double)int((wiggle_step_num)/2 + 1);

				cout << "\tStepping to: " << scanVal << " Retrying!" << endl;

				++wiggle_step_num;
			}

			if( wiggle_step_num >= 20 ) break;

			scanParameter->SetBlindedValue( scanVal ) ;
			output_interface->StartStopwatch();
			scanStepResult = FitAssembler::DoSafeFit( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, OutputLevel );
		}


		cout << "Fit Finished!\n" <<endl;
		//  THIS IS ALWAYS TRUE BY DEFINITION OF THE SCAN

		string name = Wanted_Param->GetName();
		string type = BottleParameters->GetPhysicsParameter( name )->GetType();
		string unit = BottleParameters->GetPhysicsParameter( name )->GetUnit();
		scanStepResult->GetResultParameterSet()->SetResultParameter( name, scanVal, scanVal, 0., scanVal, scanVal, type, unit );

		vector<string> Fixed_List = BottleParameters->GetAllFixedNames();
		vector<string> Fit_List = scanStepResult->GetResultParameterSet()->GetAllNames();
		for( unsigned short int i=0; i < Fixed_List.size() ; ++i )
		{
			bool found=false;
			for( unsigned short int j=0; j < Fit_List.size(); ++j )
			{
				if( Fit_List[j] == Fixed_List[i] )
				{
					found = true;
				}
			}
			if( !found )
			{
				string fixed_type = BottleParameters->GetPhysicsParameter( Fixed_List[i] )->GetType();
				string fixed_unit = BottleParameters->GetPhysicsParameter( Fixed_List[i] )->GetUnit();
				double fixed_value = BottleParameters->GetPhysicsParameter( Fixed_List[i] )->GetValue();
				scanStepResult->GetResultParameterSet()->ForceNewResultParameter( Fixed_List[i],
												fixed_value, fixed_value, 0., fixed_value, fixed_value, fixed_type, fixed_unit );
			}
		}

		scanStepResult->GetResultParameterSet()->GetResultParameter( scanName )->SetScanStatus( true );

		ResultFormatter::ReviewOutput( scanStepResult );

		output_interface->AddFitResult( scanStepResult );
	}

	//Reset the parameter as it was
	scanParameter->SetType( originalType ) ;
	scanParameter->SetBlindedValue( originalValue ) ;
}

//  Interface for internal calls
void ScanStudies::DoScan2D( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet* BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, pair<ScanParam*, ScanParam*> Param_Set, vector<FitResultVector*>* output_interface, int OutputLevel )
{
	FunctionConfig->SetIntegratorTest( false );

	//	vector<string> namez = BottleParameters->GetAllNames();
	vector<string> result_names = BottleParameters->GetAllNames();
	double uplim = Param_Set.first->GetMax();
	double lolim = Param_Set.first->GetMin();
	double npoints = Param_Set.first->GetPoints();

	string scanName = Param_Set.first->GetName();
	string scanName2 = Param_Set.second->GetName();


	// Get a pointer to the physics parameter to be scanned and fix it
	// CAREFUL:  this must be reset as it was at the end.
	PhysicsParameter * scanParameter = BottleParameters->GetPhysicsParameter(scanName);
	double originalValue = scanParameter->GetBlindedValue( );
	string originalType = scanParameter->GetType( );
	scanParameter->SetType( "Fixed" );

	// Need to set up a loop , fixing the scan parameter at each point

	double deltaScan;
	if( int(npoints) !=1 ) deltaScan = (uplim-lolim) / (npoints-1.) ;
	else deltaScan=0.;

	for( int si=0; si < int(npoints); ++si)
	{

		cout << "\n\n2DSCAN OUTER NUMBER\t\t" << si+1 << "\t\tOF\t\t" << int(npoints) <<endl<<endl;
		FitResultVector* Returnable_Result = new FitResultVector( result_names );

		// Set scan parameter value
		double scanVal = lolim + si*deltaScan;
		scanParameter->SetBlindedValue( scanVal ) ;

		// Do a scan point fit
		ScanStudies::DoScan( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, Param_Set.second, Returnable_Result, OutputLevel );

		//  THIS IS ALWAYS TRUE BY DEFINITION OF THE SCAN
		string name = Param_Set.first->GetName();
		string type = BottleParameters->GetPhysicsParameter( name )->GetType();
		string unit = BottleParameters->GetPhysicsParameter( name )->GetUnit();

		for( short int i=0; i < Returnable_Result->NumberResults(); ++i )
		{
			Returnable_Result->GetFitResult( i )->GetResultParameterSet()->SetResultParameter( name, scanVal, scanVal, 0., scanVal, scanVal, type, unit );
			Returnable_Result->GetFitResult( i )->GetResultParameterSet()->GetResultParameter( scanName )->SetScanStatus( true );
		}

		output_interface->push_back( Returnable_Result );
	}

	//Reset the parameter as it was
	scanParameter->SetType( originalType ) ;
	scanParameter->SetBlindedValue( originalValue ) ;
}

// Interface for external calls
vector<FitResultVector*> ScanStudies::ContourScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet* BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, OutputConfiguration* OutputConfig, string scanName, string scanName2, int OutputLevel )
{
	vector<FitResultVector*>* Returnable_Result = new vector<FitResultVector*>;

	pair< ScanParam*, ScanParam* > Param_Set = OutputConfig->Get2DScanParams( scanName, scanName2 );

	DoScan2D( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, Param_Set, Returnable_Result, OutputLevel );

	return *Returnable_Result;
}

//  Interface for external calls
FitResultVector* ScanStudies::SingleScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet* BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, OutputConfiguration* OutputConfig, string scanName, int OutputLevel )
{
	FitResultVector* Returnable_Result = new FitResultVector( BottleParameters->GetAllNames() );

	ScanParam* local_param = OutputConfig->GetScanParam( scanName );

	DoScan( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, local_param, Returnable_Result, OutputLevel );

	return Returnable_Result;
}
