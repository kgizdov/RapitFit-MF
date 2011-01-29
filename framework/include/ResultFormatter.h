/**
        @class ResultFormatter

        A collection of static methods for outputting RapidFit data objects

        @author Benjamin M Wynne bwynne@cern.ch
	@date 2009-10-02
*/

#ifndef RESULT_FORMATTER_H
#define RESULT_FORMATTER_H

#include "IDataSet.h"
#include <string>
#include <vector>
#include "FitResult.h"
#include "ToyStudyResult.h"
#include "LLscanResult.h"
#include "LLscanResult2D.h"

using namespace std;

class ResultFormatter
{
	public:
		static void MakeRootDataFile( string, IDataSet* );
		static void DebugOutputFitResult( FitResult* );
		static void LatexOutputFitResult( FitResult* );
		static void LatexOutputCovarianceMatrix( FitResult* );
		static void PlotFitContours( FitResult*, string );
		static double GetElementFromCovarianceMatrix( vector<double>, int, int);		
		static bool IsParameterFree( FitResult*, string );

		//MakePullPlots chooses the appropriate method based on the first string argument
		static void MakePullPlots( string, string, ToyStudyResult* );
		static void FlatNTuplePullPlots( string, ToyStudyResult* );
		static void SeparateParameterPullPlots( string, ToyStudyResult* );
		static void MakeLLscanPlots( vector<LLscanResult*> scanResults, string filename ); 
		static void MakeLLcontourPlots( vector<LLscanResult2D*> scanResults, string filename ); 
};

#endif
