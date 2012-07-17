// $Id: WrongPVAssocBkg.cpp,v 1.2 2009/11/13 15:31:51 gcowan Exp $
/** @class WrongPVAssocBkg WrongPVAssocBkg.cpp
 *
 *  PDF for background due to wrong PV association 
 *
 *  @author Greig Cowan
 *  @date 2012-05-29
 */

#include "WrongPVAssocBkg.h"
#include "Mathematics.h"
#include <iostream>
#include <fstream>
#include "math.h"
#include "TMath.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TH3D.h"
#include "TAxis.h"
#include "TH1.h"

PDF_CREATOR( WrongPVAssocBkg );

//Constructor
WrongPVAssocBkg::WrongPVAssocBkg( PDFConfigurator* config ) :

	// Observables
	  massName		( config->getName("mass") )
	, timeName		( config->getName("time") )
	, eventResolutionName   ( config->getName("eventResolution") )
	, mass(), time()
	, xaxis(), yaxis()
	, nxbins(), nybins() 
	, xmin(), xmax()
	, ymin(), ymax()
	, deltax(), deltay()
	, total_num_entries()
{

	cout << "Constructing PDF: WrongPVAssocBkg  " << endl ;

	//Make prototypes
	MakePrototypes();
	
	//Find name of histogram needed to define 3-D angular distribution
	string fileName = config->getConfigurationValue( "TimeMassHistogram" ) ;

	//Initialise depending upon whether configuration parameter was found
	if( fileName == "" )
	{
		cout << "   No background histogram found: using flat background " << endl ;
		exit(-1);
	}
	else
	{
		cout << "   Background histogram requested: " << fileName << endl ;

		//File location
		ifstream input_file;
		input_file.open( fileName.c_str(), ifstream::in );
		input_file.close();

		bool local_fail = input_file.fail();

		if( !getenv("RAPIDFITROOT") && local_fail )
		{
			cerr << "\n" << endl;
			//cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			cerr << "$RAPIDFITROOT NOT DEFINED, PLEASE DEFINE IT SO I CAN USE ACCEPTANCE DATA" << endl;
			//cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			cerr << "\n" << endl;
			exit(-987);
		}

		string fullFileName;

		if( getenv("RAPIDFITROOT") )
		{
			string path( getenv("RAPIDFITROOT") ) ;

			cout << "RAPIDFITROOT defined as: " << path << endl;

			fullFileName = path+"/pdfs/configdata/"+fileName ;

			input_file.open( fullFileName.c_str(), ifstream::in );
			input_file.close();
		}
		bool elsewhere_fail = input_file.fail();

		if( elsewhere_fail && local_fail )
		{
			cerr << "\n\tFileName:\t" << fullFileName << "\t NOT FOUND PLEASE CHECK YOUR RAPIDFITROOT" << endl;
			cerr << "\t\tAlternativley make sure your XML points to the correct file, or that the file is in the current working directory\n" << endl;
			exit(-89);
		}

		if( fullFileName.empty() || !local_fail )
		{
			fullFileName = fileName;
		}

		//Read in histo
		TFile* f =  TFile::Open(fullFileName.c_str());
		histo = (TH2D*) f->Get("time_and_mass"); //(fileName.c_str())));

		// time
		xaxis = histo->GetXaxis();
		cout << "X axis Name: " << xaxis->GetName() << "\tTitle: " << xaxis->GetTitle() << endl;
		xmin = xaxis->GetXmin();
		xmax = xaxis->GetXmax();
		nxbins = xaxis->GetNbins();
		cout << "X axis Min: " << xmin << "\tMax: " << xmax << "\tBins: " << nxbins << endl;
		deltax = (xmax-xmin)/nxbins;

		// mass
		yaxis = histo->GetYaxis();
		cout << "Y axis Name: " << yaxis->GetName() << "\tTitle: " << yaxis->GetTitle() << endl;
		ymin = yaxis->GetXmin();
		ymax = yaxis->GetXmax();
		nybins = yaxis->GetNbins();
		cout << "Y axis Min: " << ymin << "\tMax: " << ymax << "\tBins: " << nybins << endl;
		deltay = (ymax-ymin)/nybins;

		//method for Checking whether histogram is sensible

		total_num_entries = histo->GetEntries();
		int total_num_bins = nxbins * nybins;
		int sum = 0;

		vector<int> zero_bins;

		//loop over each bin in histogram and print out how many zero bins there are
		for (int i=1; i < nxbins+1; ++i)
		{
			for (int j=1; j < nybins+1; ++j)
			{
						double bin_content = histo->GetBinContent(i, j);
						//cout << "Bin content: " << bin_content << endl;
						if(bin_content<=0)
						{
							zero_bins.push_back(1);
						}
						//cout << " Zero bins " << zero_bins.size() << endl;}
						else if (bin_content>0)
						{
							sum += (int) bin_content;
						}
			}
		}

		int average_bin_content = sum / total_num_bins;

		cout << "\n\n\t\t" << "****" << "For total number of bins " << total_num_bins << " there are " << zero_bins.size() << " bins containing zero events " << "****" << endl;
		cout <<  "\t\t\t" << "***" << "Average number of entries of non-zero bins: " << average_bin_content << "***" << endl;
		cout << endl;
		cout << endl;

		// Check.  This order works for both bases since phi is always the third one.
		if ((xmax-xmin) < 25. || (ymax-ymin) < 200. ) 
		{
			cout << "In WrongPVAssocBkg::WrongPVAssocBkg: The full angular range is not used in this histogram - the PDF does not support this case" << endl;
			exit(1);
		}

		cout << "Finishing processing histo" << endl;
	}
}

// Copy
WrongPVAssocBkg::WrongPVAssocBkg( const WrongPVAssocBkg& input ) : BasePDF( (BasePDF) input ),
histo(input.histo), xaxis(input.xaxis), yaxis(input.yaxis),
nxbins(input.nxbins), nybins(input.nybins), 
xmin(input.xmin), xmax(input.xmax), 
ymin(input.ymin), ymax(input.ymax), 
deltax(input.deltax), deltay(input.deltay), 
total_num_entries(input.total_num_entries), 
massName(input.massName), timeName(input.timeName), eventResolutionName( input.eventResolutionName ), 
mass(input.mass), time(input.time)
{
}

//Destructor
WrongPVAssocBkg::~WrongPVAssocBkg()
{
}

//Make the data point and parameter set
void WrongPVAssocBkg::MakePrototypes()
{
	//Make the DataPoint prototype
	allObservables.push_back( massName );
	allObservables.push_back( timeName );

	//Make the parameter set
	vector<string> parameterNames;
	allParameters = ParameterSet(parameterNames);
}

//Return a list of observables not to be integrated
vector<string> WrongPVAssocBkg::GetDoNotIntegrateList()
{
        vector<string> list;
        list.push_back(eventResolutionName);
        return list;
}


bool WrongPVAssocBkg::SetPhysicsParameters( ParameterSet * NewParameterSet )
{
	bool isOK = allParameters.SetPhysicsParameters(NewParameterSet);
	return isOK;
}

//Main method to build the PDF return value
double WrongPVAssocBkg::Evaluate(DataPoint * measurement)
{
	// Observable
	mass = measurement->GetObservable( massName )->GetValue();
	time = measurement->GetObservable( timeName )->GetValue();
	
	return this->timeMassFactor();
}


// Normlisation
double WrongPVAssocBkg::Normalisation(PhaseSpaceBoundary * boundary)
{
	(void)boundary;
	return buildPDFdenominator();
}

double WrongPVAssocBkg::buildPDFdenominator()
{
	return 1.;
}


//Angular distribution function
double WrongPVAssocBkg::timeMassFactor( )
{
	double returnValue = 0.;

	int globalbin = -1;
	int xbin = -1, ybin = -1;
	double num_entries_bin = -1.;

		//Find global bin number for values of angles, find number of entries per bin, divide by volume per bin and normalise with total number of entries in the histogram
		xbin = xaxis->FindFixBin( time ); if( xbin > nxbins ) xbin = nxbins;
		ybin = yaxis->FindFixBin( mass ); if( ybin > nybins ) ybin = nybins;
		
		globalbin = histo->GetBin( xbin, ybin );
		num_entries_bin = histo->GetBinContent(globalbin);

		//Angular factor normalized with phase space of histogram and total number of entries in the histogram
		returnValue = num_entries_bin / (deltax * deltay) / total_num_entries ;
	
	return returnValue;
}
