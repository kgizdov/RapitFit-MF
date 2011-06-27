/**
	@class FitFunctionConfiguration

	Container that stores all information related to FitFunction configuration, and returns an appropriate instance of a FitFunction

	@author Benjamin M Wynne bwynne@cern.ch
	@date 2009-11-27
*/

//	RapidFit Headers
#include "FitFunctionConfiguration.h"
#include "ClassLookUp.h"

//Default constructor
FitFunctionConfiguration::FitFunctionConfiguration() : functionName(), weightName(), hasWeight(false), wantTrace(false), TraceFileName()
{
}

//Constructor with only name of FitFunction
FitFunctionConfiguration::FitFunctionConfiguration( string InputName ) : functionName(InputName), weightName(), hasWeight(false), wantTrace(false), TraceFileName()
{
}

//Constructor for FitFunction with event weights
FitFunctionConfiguration::FitFunctionConfiguration( string InputName, string InputWeight ) : functionName(InputName), weightName(InputWeight), hasWeight(true), wantTrace(false), TraceFileName()
{
}

//Destructor
FitFunctionConfiguration::~FitFunctionConfiguration()
{
}

//Return appropriate instance of FitFunction
FitFunction * FitFunctionConfiguration::GetFitFunction( PhysicsBottle* PhysBottle )
{
	FitFunction * theFunction = ClassLookUp::LookUpFitFunctionName(functionName);

	//Use event weights if specified
	if (hasWeight)
	{
		theFunction->UseEventWeights(weightName);
	}

	theFunction->SetPhysicsBottle( PhysBottle );

	if( wantTrace )
	{
		theFunction->SetupTrace( TraceFileName );
	}

	return theFunction;
}

//Return whether weights are being used
bool FitFunctionConfiguration::GetWeightsWereUsed()
{
	return hasWeight ;
}

//Return weight name
string FitFunctionConfiguration::GetWeightName()
{
	return weightName ;
}

void FitFunctionConfiguration::SetupTrace( TString FileName )
{
	wantTrace = true;
	TraceFileName = FileName;
}
