/*!
 * @class FitAssembler
 *
 * @brief The intention is for this class to formalise the process of assembling the components of a fit
 * 
 * Ideally it will be a set of nested static methods, starting from more and more rudimentary components
 *
 * @author Benjamin M Wynne bwynne@cern.ch
 * @author Robert Currie rcurrie@cern.ch
 */

#pragma once
#ifndef FIT_ASSEMBLER_H
#define FIT_ASSEMBLER_H

///	RapidFit Headers
#include "FitResult.h"
#include "IMinimiser.h"
#include "IFitFunction.h"
#include "FitFunctionConfiguration.h"
#include "MinimiserConfiguration.h"
#include "PDFWithData.h"
#include "ScanParam.h"
#include "FitResultVector.h"
#include "XMLConfigReader.h"
///	System Headers
#include <vector>
#include <string>
#include <exception>

using namespace::std;

class FitAssembler
{
	public:
		/*!
		 * @brief This is where most developers should Address RapidFit to Minimise a Given Set of Inputs
		 *
		 *
		 * New Interface to Fits, for a well behaved PDF it will not crash, it will always return a FitResult which makes it more stable for scanning and such
		 * Also provides way to control the output level during the fit (default = Everything)
		 *
		 * Because this accepts PDFWithData Objects the Data will only be Generated when it's requested
		 *
		 * @param MinimiserConfig   This should point to a MinimiserConfiguration capable of Constructing an IMinimiser to be called within this class
		 *
		 * @param Config            This should contain a FitFunctionConfiguration capable of Constructing a FitFunction that the IMinimiser should Minimise
		 *
		 * @param Parameters        This should contain at least all of the Physics Parameters required to be minimised
		 *
		 * @param PDFWithDataSet    This must contain all of the PDFs and their corresponding DataSets wrapped in the PDFWithData objects
		 *
		 * @param Constraints       This should contain all of the External and other Constraints wished to be used in the Minimisation
		 *
		 * @param verbosity         This controls how much runtime output is generated by this Minimisation
		 *
		 * @return Returns a FitResult containing all of the information about what has been minimised and it's minimum Value
		 */
		static FitResult * DoSafeFit( MinimiserConfiguration* MinimiserConfig, FitFunctionConfiguration* Config, ParameterSet* Parameters, vector< PDFWithData* > PDFWithDataSet,
				vector< ConstraintFunction* > Constraints, bool forceContinue=false, int verbosity =1, DebugClass* =NULL );

		/*!
		 * @brief This is the version of DoFit which works with IPDFs and DataSets separated out from PDFWithData
		 *
		 * The difference between this an DoSafeFit is that it constructs the PhysicsBottle itself and jumps straight to:  DoFit( MinimiserConfiguration*, FitFunctionConfiguration*, PhysicsBottle* )
		 *
		 * This is a nice interface if you already have a dataset and PDF pointer available. They are already in a format that can be placed in a PhysicsBottle
		 *
		 * @param MinimiserConfig   This should point to a MinimiserConfiguration capable of Constructing an IMinimiser to be called within this class
		 *
		 * @param Config            This should contain a FitFunctionConfiguration capable of Constructing a FitFunction that the IMinimiser should Minimise
		 *
		 * @param Parameters        This should contain at least all of the Physics Parameters required to be minimised
		 *
		 * @param PDFS              This should contain all of the PDFs Used to Evaluate The DataSets
		 *
		 * @param DataSets          This should contain all of the DataSets being minimised over
		 *
		 * @param Constraints       This should contain all of the External and other Constraints wished to be used in the Minimisation
		 *
		 * @return Returns a FitResult containing all of the information about what has been minimised and it's minimum Value
		 */
		static FitResult * DoFit( MinimiserConfiguration* MinimiserConfig, FitFunctionConfiguration* Config, ParameterSet* Parameters, vector< IPDF* > PDFs, vector< IDataSet* > DataSets,
				vector< ConstraintFunction* > Constraints, DebugClass* =NULL );

		/*!
		 * @brief This Will perform Pete's Strategy of DoSafeFit which checks for Alternate Minimia in the Strong Phases of JpsiPhi
		 */
		static FitResult * Petes_DoSafeFit( MinimiserConfiguration*, FitFunctionConfiguration*, ParameterSet*, vector< PDFWithData* >,
				vector< ConstraintFunction* >, bool forceContinue=false, int OutputLevel=1, DebugClass* =NULL);

		/*!
		 * @brief Undocumented
		 */
		static FitResult * PetesGamma_DoSafeFit( MinimiserConfiguration*, FitFunctionConfiguration*, ParameterSet*, vector< PDFWithData* >,
				vector< ConstraintFunction* >, bool forceContinue=false, int OutputLevel=1, DebugClass* =NULL);

		/*!
		 * @brief This Will perform Rob's Strategy of DoSafeFit which checks for Alternate Minima in Strong Phases and dG and Phi_s in JpsiPhi
		 */
		static FitResult * Robs_DoSafeFit( MinimiserConfiguration*, FitFunctionConfiguration*, const ParameterSet*, const vector< PDFWithData* >,
				const vector< ConstraintFunction* >, bool forceContinue=false, const int OutputLevel=1, DebugClass* =NULL);

		/*!
		 * @brief This checks the given PDFs and DataSets to make sure that they are consistent and can be used for fitting
		 *
		 * @return void
		 */
		static void CheckInputObs( const vector<IPDF*> AllPDFs, const vector<IDataSet*> allDataNum, const string WeightName=string(), const string AlphaName=string(), const DebugClass* =NULL );

		/*!
		 * @brief This checks the given ParameterSet for the required Parameters
		 *
		 * Making use of this function allows us to pass a sensible configuration to the Minimiser that does not contain erronious or uncontrolled parameters
		 *
		 * If a Parameter is missing this will exit 'cleanly'
		 *
		 * @param givenParams       This is the ParameterSet wished to be checked
		 *
		 * @param allPDFs           These are the PDFs which claim the Physics Parameters in the Fit
		 *
		 * @param allDataNum        These are the size of the requested datasets corresponding to each PDF
		 *
		 * @return Returns a ParamaterSet containing ONLY the parameters wanted by the PDFs during the fit
		 */
		static ParameterSet* CheckInputParams( const ParameterSet* givenParams, const vector<IPDF*> allPDFs, const vector<int> allDataNum, DebugClass* =NULL );


		/*!
		 *
		 */
		static ParameterSet* GenerationParameters( const ParameterSet* checkedBottleParameters, const ParameterSet* BottleParameters );

	private:

		/*!
		 * @brief This moves the 'safety' part of the DoFit into a dedicated function call which is much neater to work with
		 *
		 * This is the part of DoSafeFit which actually performs catching of Errors thrown within a bad fit, it just makes the output cleaner
		 *
		 * @return Void
		 */
		static void SafeMinimise( IMinimiser* );

		/*!
		 * @brief Private internal function called from DoSafeFit
		 *
		 *  This step checks the verbosity level and to perform checks on the output FitResult before it is passed out of this class
		 *
		 * @return Returns the final checked FitResult
		 */
		static FitResult * DoSingleSafeFit( MinimiserConfiguration*, FitFunctionConfiguration*, ParameterSet*, vector< PDFWithData* >,
				vector< ConstraintFunction* >, bool forceContinue=false, int OutputLevel=-1, DebugClass* =NULL );

		/*!
		 * @brief Used for checking a ResultDataSet against the input Parameter to insert 'missing' parameters to avoid bugs downstream in further analysis in RapidFit
		 *
		 * This performs a check on the FitResult and adds in any Physics Parameters which were not passed to the Minimiser
		 *
		 * These parameters are expected further up in RapidFit and this should be considered a User error at runtime more than a RapidFit error, performing the check keeps everything in balance
		 */
		static void CheckParameterSet( FitResult*, ParameterSet*, DebugClass* =NULL );


		/*!
		 * @brief Last DoFit which initialises the minimiser and the FitFunction and does the first fit
		 *
		 * This step is where the actual Minimisation is called and once it has completed the FitResult is extraced
		 *
		 * @callergraph
		 *
		 * @return Passes the FitResult up the chain
		 */
		static FitResult * DoFit( IMinimiser*, IFitFunction*, DebugClass* =NULL );

		/*!
		 * @brief Second DoFit - Used for Constructing a FitFunction for passing to the Minimiser
		 *
		 * This Step constructs the Minimiser and the FitFunction to be used in the minimisation
		 *
		 * The FitFunction takes control of the PhysicsBottle here
		 *
		 * @return Passes the FitResult up the chain
		 */
		static FitResult * DoFit( MinimiserConfiguration*, FitFunctionConfiguration*, PhysicsBottle*, DebugClass* );

		/*!
		 * @brief First DoFit - Used For Constructing a PhysicsBottle to contain all of the 'Physics' input
		 *
		 * This is the first internal function called from DoSingleSafeFit
		 *
		 * This step Initializes the PhysicsBottle and adds the PDFs and DataSets to the bottle as well as the ParameterSet
		 *
		 * @return Passes the FitResult up the chain
		 */
		static FitResult * DoFit( MinimiserConfiguration*, FitFunctionConfiguration*, ParameterSet*, vector< PDFWithData* >,
				vector< ConstraintFunction* >, DebugClass* );

};

#endif

