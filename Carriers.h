#include "Field.h"
#include "TMath.h"
#include <stdlib.h>

#define EPSILON		8.854e-12		// vacuum permittivity
#define EPSILONR		11.9			// relative permittivity of silicon
#define ECHARGE		1.602177e-19	// elementary charge in C

#define kB			1.38065e-23	// Boltzmann constant (J/K)
#define m0			9.109383e-31	// electron mass (kg)
#define MU_E_300			0.1414		// electron mobility in m2/Vs at 300K (Sentaurus)
#define MU_H_300			0.0470		// hole mobility in m2/Vs at 300 K (Sentaurus)
#define VESAT_300      		        1.07e5		//1.07e5 sentaurus electron saturation velocity in m/s
#define VHSAT_300			8.37e4		// sentaurus hole saturation velocity in m/s
#define MU_E_D_300			0.18		// Diamond electron mobility in m2/Vs at 300K
#define MU_H_D_300			0.150		// Diamond hole mobility in m2/Vs at 300 K
#define VESAT_D_300      		270e3		// Diamond electron saturation velocity in m/s
#define VHSAT_D_300			270e3		// Diamond hole saturation velocity in m/s
#define SQMUEDIVVESAT	(MU_E_300/(VESAT_300))*(MU_E_300/(VESAT_300))
#define MUHDIVVHSAT		(MU_H_300/(VHSAT_300))
#define RANGE_ALPHA		30	//Nicolo: range of alpha particles in silicon (um)
#define ALPHAPRECISION 50 //Precision of alpha simulation
#define ALPHAENERGY 5000000 //Fix Energy for alpha
#define IONENERGY 3.6 //Ionization Energy in Silicon
#define GAINDEPTH		3	//depth at which gain layer starts
#define GAINLENGHT		4	//Extension of  gain layer
#define ALPHA_AVALANCHE 0.746 //second townsend coefficient
#define ELECTRODE_DEPTH  1   // Depth of the electrodes, charged in this space do no generate current
//#define CHARGESCALE 2 // Scaling value to speed up computation. Set to = 1 for full computation
#define OSC_IN_RESISTANCE 	50 //Ohm osc input impedance
#define beta_0_e 1.109
#define beta_exp_e 0.66
#define beta_0_h 1.213
#define beta_exp_h 0.17
#define LONGDIF         1 // longitudinal diffusion
#define PRINTFREQUENCY         1 // 
#define TIMEMAX 60*1.e-9/TIMEUNIT
#ifndef Carriers_H
#define Carriers_H
#define LaserPairsPerMicron 75
#define Laser1024Range 1000 // in micron 1mm absortion lenght?
#define XRayRangeDef 16 // 10 cm absortion lenght?

class Carriers {

	private:
		double x;		// x position of carrier
		double y;		// y position of carrier
		int inside;		// if carrier inside detector, inside=1 ... if outside inside=0, if not yet created inside=-1
		double etheta;	// lorentz angle of electrons
		double htheta;	// lorentz angle of holes
		//		int mipcharge; // number of carriers
		int charge;		//-1= electron +1=hole
		double vx;		//velocity along x axis
		double vy;		//velocity along y axis
		bool egain;		//if electron is a gain electron or not true=gain false=no gain
		// double ygain;	//position of gain layer in ultrafast
		// bool alpha_above;	//if alpha particle comes from above or not
		double leftovergain;
		//		double alphaRange;	//range of alpha particles
// 		double tau;		//time constant (Oscill BW)^-1



	public:
		Carriers();			// carrier constructor
		~Carriers();			// carrier destructor
		double Getx();			// returns x position of carrier
		double Gety();			// returns y position of carrier
		void Setx(double);		// set x position of carrier
		void Sety(double);		// set y position of carrier
		void Setinside(int);	// set inside 
		int Getinside();		// returns inside 
		void Setetheta(double);	// set lorentz angle of electrons
		void Seththeta(double);	// set lorentz angle of holes
		double Getetheta();		// returns lorentz angle of electrons
		double Geththeta();		// returns lorentz angle of holes
		void SetGainPart(bool);	//sets particle as gain particle
		bool GetGainPart();		//returns is gain particle
		void SetCharge(int);
		int GetCharge();
		void SetVx(double);
		void SetVy(double);
		double GetVx();
		double GetVy();
		void SetLeftovergain(double);
		double GetLeftovergain();

};

#endif

void CreateCharges(Potentials &,Carriers *,int, void*);							// create charges along path of particle
void CreateChargesAlphaTop(Potentials &pot, Carriers *carriers, int hit, void*);				//creates alpha particles
void CreateChargesAlphaBottom(Potentials &pot, Carriers *carriers, int hit, void*);
void CreateChargesLaserSide(Potentials &pot, Carriers *carriers, int hit, void*);
void CreateChargesXRay(Potentials &pot, Carriers *carriers, int hitx,int hity, void*);
void CalculateCurrents(Potentials &,Field **, Field**, Carriers *, void*, int);		// calculate currents
void SetLorentz(Potentials&, Carriers *,double, double, int);									// set lorentz angle
