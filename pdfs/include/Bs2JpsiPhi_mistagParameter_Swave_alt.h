// $Id: Bs2JpsiPhi_mistagParameter_Swave_alt.h,v 1.1 2009/12/06  Pete Clarke Exp $
/** @class Bs2JpsiPhi_mistagParameter_Swave_alt RaPDF_.h
 *
 *  RapidFit PDF for Bs2JpsiPhi
 *
 *  @author Pete Clarke peter.clarke@ed.ac.uk
 *  @date 2011-01-28
 */

#ifndef Bs2JpsiPhi_mistagParameter_Swave_alt_H
#define Bs2JpsiPhi_mistagParameter_Swave_alt_H

#include "BasePDF.h"
#include "RooComplex.h"

class Bs2JpsiPhi_mistagParameter_Swave_alt : public BasePDF
{
	public:
		Bs2JpsiPhi_mistagParameter_Swave_alt();
		~Bs2JpsiPhi_mistagParameter_Swave_alt();

		//Mandatory method to evaluate the PDF value:
		virtual double Evaluate(DataPoint*);

		//Other opeating methods
		virtual bool SetPhysicsParameters(ParameterSet*);
		virtual vector<string> GetDoNotIntegrateList();

	protected:
		//Calculate the PDF normalisation
		virtual double Normalisation(DataPoint*, PhaseSpaceBoundary*);

	private:
	
		void MakePrototypes();

	    //void getPhysicsParameters( double&, double&, double&, double&, double&, double&, double&, 
		//						  double&, double&, double&, double&, double &, double &, double &, double & );

		// These contain the strings that correspond to the physics parameter names.
		string gammaName;		// gamma
		string deltaGammaName;	// delta gamma
		string deltaMName;		// delta mass
		string Phi_sName;		// what we want to measure!
		string Azero_sqName;	// amplitude
		string Apara_sqName;	// amplitude
		string Aperp_sqName;	// amplitude
		string As_sqName;		// amplitude
		string delta_zeroName;	// strong phase, set to 0
		string delta_paraName;	// strong phase
		string delta_perpName;	// strong phase
		string delta_sName;		// strong phase for S-wave

	    // These are detector parameters
	    string mistagName;		// mistag fraction
		string res1Name;		  // time resolution core
		string res2Name;		  // time resolution tail
		string res1FractionName;  // fraction of core
		string timeOffsetName;    // time offset
	
	    // These are the angular accceptance factors. The first 6 are P-wave, the second 4 are S-wave
	    string angAccI1Name ;  
		string angAccI2Name ;
		string angAccI3Name ;
		string angAccI4Name ;
		string angAccI5Name ;
		string angAccI6Name ;
		string angAccI7Name ;
		string angAccI8Name ;
		string angAccI9Name ;
		string angAccI10Name ;
	
		// These contain the strings that correspond to the observable names
		string timeName;		// proper time
		string cosThetaName;	// cos of angle of mu+ wrt z-axis in Jpsi frame
		string phiName;			// azimuthal angle of the mu+ in Jpsi frame
		string cosPsiName;		// helicity angle between K+ and -ve Jpsi direction
		string tagName;			// B tag

		// Measured Event Observables
		double t ;
		double ctheta_tr ;
		double phi_tr ;
		double ctheta_1 ;
		int tag ;
	
		// Physics Fit Parameters 
		double gamma_in ;
		double dgam ;

		double Aperp_sq ;
		double Apara_sq ;
		double Azero_sq ;
		double As_sq ;

		double delta_para ;
		double delta_perp ;
		double delta_zero ;
		double delta_s ;
		double delta1 ;
		double delta2 ;
	
		double delta_ms ;
		double phi_s ;
	
		// Other experimental parameters
		double tagFraction ;
		double resolution ;
		double resolution1 ;
		double resolution2 ;
		double resolution1Fraction ;
		double timeOffset ;
	
	    // Angular acceptance factors
		double angAccI1 ;
		double angAccI2 ;
		double angAccI3 ;
		double angAccI4 ;
		double angAccI5 ;
		double angAccI6 ;
		double angAccI7 ;
		double angAccI8 ;
		double angAccI9 ;
		double angAccI10 ;
	
		// Othere things calculated later on the fly
		double tlo, thi ;

		// Caching 
		bool normalisationCacheValid ;
		double normalisationCacheValueRes1[3] ;
		double normalisationCacheValueRes2[3] ;

		// stored time primitives
		double expL_stored ;
		double expH_stored ;
		double expSin_stored ;
		double expCos_stored ;
	

		//------ This is all stuff from Petes J/PsiPhi PDF ---------------------------
		    	
		//Amplitudes Used in three angle PDF
		double AT() const ;
		double AP() const ;
		double A0() const ;
		double AS() const ;
	
		double ctrsq() const ;
		double strsq() const ;
		double ct1sq() const ;
		double st1sq() const ;
		double cphsq() const ;
		double sphsq() const ;
	
		// Widths
		double gamma_l() const ;
		double gamma_h() const ;
		double gamma() const ;
	
		// Time primitives
		double expL() const ;
		double expH() const ;
		double expCos() const ;
		double expSin() const ;
		double intExpL() const ;
		double intExpH() const ;
		double intExpSin() const ;
		double intExpCos() const ;
	
	
		//--------------------
		// Tag category, i.e B, Bbar or untagged.
		double q() const ;
	
	
		//------------------------------------------------------------------------------
		// These are the common time factors 
	
		//..................................
		double timeFactorEven(  )  const ;
		double timeFactorEvenInt(  )  const ;
	
		//..................................
		double timeFactorOdd(  )   const ;
		double timeFactorOddInt(  )  const ;
	
		//----------------------------------------------------------
		// These are the time factors and their analytic integrals for the three angle PDF
	
		//.......... P Wave .........
		//...........................
		double timeFactorA0A0( ) const ;      
		double timeFactorA0A0Int( ) const ;
	
		//...........................
		double timeFactorAPAP( ) const ;
		double timeFactorAPAPInt( ) const ;
	
		//...........................
		double timeFactorATAT( ) const ;
		double timeFactorATATInt( ) const ;

		//...........................
		double timeFactorImAPAT( ) const ; 
		double timeFactorImAPATInt( ) const ;
	
		//...........................	
		double timeFactorReA0AP( )  const ;		
		double timeFactorReA0APInt( ) const ;
	
		//...........................
		double timeFactorImA0AT(  ) const ;
		double timeFactorImA0ATInt( ) const ;
    
		//.....  S Wave..............
		//...........................
		double timeFactorASAS( ) const ;
		double timeFactorASASInt( ) const ;
	
		//............................
		double timeFactorReASAP( ) const ;
		double timeFactorReASAPInt( ) const ;

		//............................
		double timeFactorImASAT( ) const ;
		double timeFactorImASATInt( ) const ;

		//............................
		double timeFactorReASA0( ) const ;
		double timeFactorReASA0Int( ) const ;
	
	
		//------------------------------------------------------
		// Angle factors for three angle distributions
	
		double angleFactorA0A0(  ) const ;
		double angleFactorAPAP(  ) const ;
		double angleFactorATAT(  ) const ;
		double angleFactorImAPAT(  ) const ;
		double angleFactorReA0AP( ) const ;
		double angleFactorImA0AT(  ) const ;

		double angleFactorASAS(  ) const ;
		double angleFactorReASAP( ) const ;
		double angleFactorImASAT(  ) const ;
		double angleFactorReASA0( ) const ;
	
		//-------------------------------------------------------
		// Putting it together for the differential cross section and its integrals.
	
		//...................................
		double diffXsec(  )  const ;   
		
		//...................................
		// Integral over all variables: t + angles
		double diffXsecNorm1(  ) const ;
	
		//...................................
		// Integral over angles only
		double diffXsecNorm2(  ) const ;
	
	

};

#endif
