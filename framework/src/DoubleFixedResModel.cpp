/**
  @class DoubleFixedResModel

  A class for holding a sliced propertime acceptance

  @author Pete Clarke
  @data 2011-06-07
  */


#include "DoubleFixedResModel.h"
#include "StringProcessing.h"
#include "Mathematics.h"


#include <stdio.h>
#include <vector>
#include <string>

using namespace::std;


//............................................
// Constructor 
DoubleFixedResModel::DoubleFixedResModel( PDFConfigurator* configurator, bool quiet ) :
	resScaleName		( configurator->getName( "timeResolutionScale" ) ),
	resScale2Name            ( configurator->getName( "timeResolutionScale2" ) ),
	timeResFracName            ( configurator->getName( "timeResFraction" ) ),
	eventResolutionName	( configurator->getName( "eventResolution" ) ),
	numberComponents( 2 ), wantedComponent( 1 )
{
	if( !quiet) cout << "DoubleFixedResModel:: Instance created " << endl ;
}


//..........................
//This method allows the instance to add the parameters it needs to the list
void DoubleFixedResModel::addParameters( vector<string> & parameterNames )
{
	parameterNames.push_back( resScaleName );
	parameterNames.push_back( resScale2Name );
	parameterNames.push_back( timeResFracName );
	parameterNames.push_back( eventResolutionName );
	return;
}

//..........................
//To take the current value of a parameter into the instance
void DoubleFixedResModel::setParameters( ParameterSet & parameters )
{
	eventResolution = parameters.GetPhysicsParameter( eventResolutionName )->GetValue();
	resScale = parameters.GetPhysicsParameter( resScaleName )->GetValue();
	resScale2 = parameters.GetPhysicsParameter( resScale2Name )->GetValue();
	resFrac = parameters.GetPhysicsParameter( timeResFracName )->GetValue();
	return;
}

//..........................
//This method allows the instance to add the specific observables it needs to the list
void DoubleFixedResModel::addObservables( vector<string> & observableNames )
{
	(void) observableNames;
	//observableNames.push_back( eventResolutionName );
	return;
}

//..........................
//To take the current value of an obserable into the instance
void DoubleFixedResModel::setObservables( DataPoint * measurement )
{
	(void) measurement;
	//eventResolution = measurement->GetObservable( eventResolutionName )->GetValue();
	return;
}

//..........................
//To take the current value of an obserable into the instance
bool DoubleFixedResModel::isPerEvent( ) {  return false; }

//..............................
// Primitive Functions
double DoubleFixedResModel::Exp( double time, double gamma ) {
	return Mathematics::Exp( time, gamma, this->GetThisScale() );
}

double DoubleFixedResModel::ExpInt( double tlow, double thigh, double gamma ) {
	return Mathematics::ExpInt( tlow, thigh, gamma, this->GetThisScale() );
}

double DoubleFixedResModel::ExpSin( double time, double gamma, double dms ) {
	return Mathematics::ExpSin( time, gamma, dms, this->GetThisScale() );
}
double DoubleFixedResModel::ExpSinInt( double tlow, double thigh, double gamma, double dms ) {
	return Mathematics::ExpSinInt( tlow, thigh, gamma, dms, this->GetThisScale() );
}

double DoubleFixedResModel::ExpCos( double time, double gamma, double dms ) {
	return Mathematics::ExpCos( time, gamma, dms, this->GetThisScale() );
}
double DoubleFixedResModel::ExpCosInt( double tlow, double thigh, double gamma, double dms ) {
	//cout << " tlow" << tlow << "   thigh  "  << thigh << "   gamma  "  << gamma << "  dms  "  << dms  << "    res  " << eventResolution*resScale << endl;
	return Mathematics::ExpCosInt( tlow, thigh, gamma, dms, this->GetThisScale() );
}

double DoubleFixedResModel::GetThisScale()
{
	double thisRes = eventResolution;
	if( wantedComponent == 1 ) { eventResolution *= resScale; }
	else if( wantedComponent == 2 ) { eventResolution *= resScale2; }
	else { eventResolution *= resScale; }
	return thisRes;
}

unsigned int DoubleFixedResModel::numComponents()
{
	return numberComponents;
}

void DoubleFixedResModel::requestComponent( unsigned int wanted )
{
	wantedComponent = wanted;
	return;
}

double DoubleFixedResModel::GetFraction( unsigned int input )
{
	if( input == 1 )
	{
		return resFrac;
	}
	else if( input == 2 )
	{
		return (1.-resFrac);
	}
	else return 0.;
}
