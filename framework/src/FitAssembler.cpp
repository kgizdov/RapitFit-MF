/**
  @class FitAssembler

  The intention is for this class to formalise the process of assembling the components of a fit
  Ideally it will be a set of nested static methods, starting from more and more rudimentary components

  @author Benjamin M Wynne bwynne@cern.ch
  @date 2009-10-02
 */

#include "FitAssembler.h"
#include "FitResult.h"
#include "ClassLookUp.h"
#include "ScanParam.h"
#include "ToyStudyResult.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

//The final stage - do the minimisation
FitResult * FitAssembler::DoFit( IMinimiser * Minimiser, FitFunction * TheFunction )
{
	Minimiser->Minimise(TheFunction);
	return Minimiser->GetFitResult();
}

//Create the minimiser and fit function
FitResult * FitAssembler::DoFit( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, PhysicsBottle * Bottle )
{
	IMinimiser * minimiser = MinimiserConfig->GetMinimiser( Bottle->GetParameterSet()->GetAllNames().size() );
	FitFunction * theFunction = FunctionConfig->GetFitFunction();
	theFunction->SetPhysicsBottle(Bottle);

	FitResult * result = DoFit( minimiser, theFunction );

	delete theFunction;
	delete minimiser;
	return result;
}

//Create the physics bottle
FitResult * FitAssembler::DoFit( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters,
		vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints )
{
	PhysicsBottle * bottle = new PhysicsBottle( BottleParameters );

	//Fill the bottle - data generation occurs in this step
	for ( int resultIndex = 0; resultIndex < BottleData.size(); resultIndex++ )
	{
		BottleData[resultIndex]->SetPhysicsParameters(BottleParameters);
		bottle->AddResult( BottleData[resultIndex]->GetPDF(), BottleData[resultIndex]->GetDataSet() );
	}

	//Add the constraints
	for ( int constraintIndex = 0; constraintIndex < BottleConstraints.size(); constraintIndex++ )
	{
		bottle->AddConstraint( BottleConstraints[constraintIndex] );
	}

	bottle->Finalise();
	FitResult * result = DoFit( MinimiserConfig, FunctionConfig, bottle );

	delete bottle;
	return result;
}

//Create the physics bottle with pre-made data
FitResult * FitAssembler::DoFit( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters,
		vector< IPDF* > AllPDFs, vector< IDataSet* > AllData, vector< ConstraintFunction* > BottleConstraints )
{
	if ( AllPDFs.size() == AllData.size() )
	{
		PhysicsBottle * bottle = new PhysicsBottle(BottleParameters);

		//Fill the bottle - data already generated
		for ( int resultIndex = 0; resultIndex < AllData.size(); resultIndex++ )
		{
			AllPDFs[resultIndex]->SetPhysicsParameters(BottleParameters);
			bottle->AddResult( AllPDFs[resultIndex], AllData[resultIndex] );
		}

		//Add the constraints
		for ( int constraintIndex = 0; constraintIndex < BottleConstraints.size(); constraintIndex++ )
		{
			bottle->AddConstraint( BottleConstraints[constraintIndex] );
		}  

		bottle->Finalise();
		FitResult * result = DoFit( MinimiserConfig, FunctionConfig, bottle );

		delete bottle;
		return result;
	}
	else
	{
		cerr << "Mismatched number of PDFs and DataSets" << endl;
		exit(1);
	}
}


//  Interface for internal calls
void FitAssembler::DoScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, ScanParam* Wanted_Param, ToyStudyResult* output_interface )
{

	double uplim = Wanted_Param->GetMax();
	double lolim = Wanted_Param->GetMin();
	double npoints = Wanted_Param->GetPoints();
	string scanName = Wanted_Param->GetName();

	cout << "Performing Scan for the parameter " << scanName << endl ;
	
	// Get a pointer to the physics parameter to be scanned and fix it	
	// CAREFUL:  this must be reset as it was at the end.
	PhysicsParameter * scanParameter = BottleParameters->GetPhysicsParameter(scanName);
	double originalValue = scanParameter->GetBlindedValue( ) ;
	string originalType = scanParameter->GetType( ) ;
	scanParameter->SetType( "Fixed" ) ;

	// Need to set up a loop , fixing the scan parameter at each point
	double deltaScan = (uplim - lolim) / (npoints-1.) ;

	for( int si=0; si<npoints; si++) {

		// Set scan parameter value
		double scanVal = lolim + si*deltaScan ;
		scanParameter->SetBlindedValue( scanVal ) ;

		output_interface->StartStopwatch();

		// Do a scan point fit
		FitResult * scanStepResult;

		try{
			scanStepResult= FitAssembler::DoFit( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints );
		}
		catch( int e){
			if ( e == 10 ){
			cerr << "Caught exception : fit failed for these parameters - continuing to next scan value" << endl;  }
			else if ( e == 13 ){
			cerr << "Integration Error: Fit Failed" << endl;  }
			int status = -1;
			vector<string> NewNamesList = BottleParameters->GetAllNames();
			ResultParameterSet* DummyFitResults = new ResultParameterSet( NewNamesList );
			scanStepResult = new FitResult( LLSCAN_FIT_FAILURE_VALUE, DummyFitResults, status, BottleParameters );

		}
		catch (...) {
			cerr << "\n\n\n\t\t\tCaught Unknown Exception, THIS IS SERIOUS!!!\n\n\n" << endl;
			int status = -1;
			vector<string> NewNamesList = BottleParameters->GetAllNames();
			ResultParameterSet* DummyFitResults = new ResultParameterSet( NewNamesList );
			scanStepResult = new FitResult( LLSCAN_FIT_FAILURE_VALUE, DummyFitResults, status, BottleParameters );
		}

		//  THIS IS ALWAYS TRUE BY DEFINITION OF THE SCAN
		string name = Wanted_Param->GetName();
		string type = BottleParameters->GetPhysicsParameter( name )->GetType();
		string unit = BottleParameters->GetPhysicsParameter( name )->GetUnit();
		scanStepResult->GetResultParameterSet()->SetResultParameter( name, scanVal, scanVal, 0., scanVal, scanVal, type, unit );

		output_interface->AddFitResult( scanStepResult );
	}
	
	//Reset the parameter as it was
	scanParameter->SetType( originalType ) ;
	scanParameter->SetBlindedValue( originalValue ) ;

}

//  Interface for internal calls
void FitAssembler::DoScan2D( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, pair<ScanParam*, ScanParam*> Param_Set, vector<ToyStudyResult*>* output_interface )
{
	vector<string> namez = BottleParameters->GetAllNames();
	vector<string> result_names;
	for( short int i=0; i < namez.size(); i++ )
	{
		cout << namez[i] << endl;
		cout << BottleParameters->GetPhysicsParameter( namez[i] )->GetType() << endl;
		if( ( BottleParameters->GetPhysicsParameter( namez[i] )->GetType() != "Fixed" ) || ( ( namez[i] == Param_Set.first->GetName() ) || ( namez[i] == Param_Set.second->GetName() ) ) )
		{
			result_names.push_back( namez[i] );
		}
	}

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

	double deltaScan = (uplim-lolim) / (npoints-1.) ;
	
	for( int si=0; si < npoints; si++) {
		ToyStudyResult* Returnable_Result = new ToyStudyResult( result_names );
		
		// Set scan parameter value
		double scanVal = lolim + si*deltaScan ;
		scanParameter->SetBlindedValue( scanVal ) ;

		// Do a scan point fit
		FitAssembler::DoScan( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, Param_Set.second, Returnable_Result );

		//  THIS IS ALWAYS TRUE BY DEFINITION OF THE SCAN
                string name = Param_Set.first->GetName();
                string type = BottleParameters->GetPhysicsParameter( name )->GetType();
                string unit = BottleParameters->GetPhysicsParameter( name )->GetUnit();

		for( short int i=0; i < Returnable_Result->NumberResults(); i++ )
		{
			Returnable_Result->GetFitResult( i )->GetResultParameterSet()->SetResultParameter( name, scanVal, scanVal, 0.0, scanVal, scanVal, type, unit );
		}

		output_interface->push_back( Returnable_Result );
	}

	//Reset the parameter as it was
	scanParameter->SetType( originalType ) ;
	scanParameter->SetBlindedValue( originalValue ) ;

}

// Interface for external calls
vector<ToyStudyResult*> FitAssembler::ContourScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, OutputConfiguration* OutputConfig, string scanName, string scanName2 )
{
	vector<ToyStudyResult*>* Returnable_Result = new vector<ToyStudyResult*>;

	pair< ScanParam*, ScanParam* > Param_Set = OutputConfig->Get2DScanParams( scanName, scanName2 );

	DoScan2D( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, Param_Set, Returnable_Result );

	return *Returnable_Result;
}

//  Interface for external calls
ToyStudyResult* FitAssembler::SingleScan( MinimiserConfiguration * MinimiserConfig, FitFunctionConfiguration * FunctionConfig, ParameterSet * BottleParameters, vector< PDFWithData* > BottleData, vector< ConstraintFunction* > BottleConstraints, OutputConfiguration* OutputConfig, string scanName )
{
	ToyStudyResult* Returnable_Result = new ToyStudyResult( BottleParameters->GetAllNames() );

	ScanParam* local_param = OutputConfig->GetScanParam( scanName );

	DoScan( MinimiserConfig, FunctionConfig, BottleParameters, BottleData, BottleConstraints, local_param, Returnable_Result );

	return Returnable_Result;
}

