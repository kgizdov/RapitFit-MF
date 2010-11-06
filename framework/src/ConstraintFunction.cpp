/**
  @class ConstraintFunction

  Where external, experimental constraints on PhysicsParameters are calculated

  @author Benjamin M Wynne bwynne@cern.ch
  @date 21-01-10
  */

#include "ConstraintFunction.h"
#include "StringProcessing.h"
#include <iostream>

using namespace std;

//Default constructor
ConstraintFunction::ConstraintFunction()
{
}

//Constructor with correct arguments
ConstraintFunction::ConstraintFunction( vector< ExternalConstraint* > NewConstraints ) : allConstraints(NewConstraints)
{
}

//Destructor
ConstraintFunction::~ConstraintFunction()
{
}

//Perform the constraint calculation
double ConstraintFunction::Evaluate( ParameterSet * NewParameters )
{
	vector<string> parameterNames = NewParameters->GetAllNames();
	double constraintValue = 0.0;

	//Loop over all ExternalConstraints
	for ( int constraintIndex = 0; constraintIndex < allConstraints.size(); constraintIndex++ )
	{
		string name = allConstraints[constraintIndex]->GetName();
		if ( name == "GammaL" )
		{
			//GammaL = Gamma + ( deltaGamma / 2 )
			// Get gamma and delta gamma
			double gamma = NewParameters->GetPhysicsParameter("gamma")->GetValue();
			double dgam =  NewParameters->GetPhysicsParameter("deltaGamma")->GetValue();
			double gaml_fit = gamma + (dgam/2.0);
			double gaml_con = allConstraints[constraintIndex]->GetValue();
			double gaussSqrt = ( gaml_fit -  gaml_con ) / allConstraints[constraintIndex]->GetError();
			constraintValue += gaussSqrt * gaussSqrt;
			
		}
		else if ( name == "GammaObs" )
		{
			//GammaObs^-1 = Gamma^-1 * ( 1 + (deltaGamma/2*Gamma)^2 ) / ( 1 - (deltaGamma/2*Gamma)^2 )
			// Get gamma and delta gamma
			double gamma = NewParameters->GetPhysicsParameter("gamma")->GetValue();
			double dgam =  NewParameters->GetPhysicsParameter("deltaGamma")->GetValue();
			double gamobs_fit = gamma * (1-(dgam/2/gamma)*(dgam/2/gamma)) / (1+(dgam/2/gamma)*(dgam/2/gamma));
			double gamobs_con = allConstraints[constraintIndex]->GetValue();
			double gaussSqrt = ( gamobs_fit -  gamobs_con ) / allConstraints[constraintIndex]->GetError();
			constraintValue += gaussSqrt * gaussSqrt;
			
		}
		else if ( StringProcessing::VectorContains( &parameterNames, &name ) >= 0 )
		{
			//Do standard gaussian constraint calculation
			double parameterValue = NewParameters->GetPhysicsParameter(name)->GetValue();
			double gaussSqrt = ( parameterValue - allConstraints[constraintIndex]->GetValue() ) / allConstraints[constraintIndex]->GetError();
			constraintValue += gaussSqrt * gaussSqrt;
		}
	}

	return 0.5 * constraintValue;
}