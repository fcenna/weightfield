#include "WFGUI.h"
/////////////////////////////// ///////////////////////////////////////////////////////
Carriers::Carriers() {
	x=0;
	y=0;
	inside=-1;
	vx=0;
	vy=0;
	charge=0;
	leftovergain=0;
}
////////////////////////////////// ////////////////////////////////////////////////////
void Carriers::Setx(double valx) {
	x=valx;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::Sety(double valy) {
	y=valy;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::Setinside(int val) {
	inside=val;
}
//////////////////////////////////////////////////////////////////////////////////////
int Carriers::Getinside() {
	return inside;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::Getx() {
	return x;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::Gety() {
	return y;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::Setetheta(double angle) {
	etheta=angle;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::Seththeta(double angle) {
	htheta=angle;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::Getetheta() {
	return etheta;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::Geththeta() {
	return htheta;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::SetGainPart(bool gain) {
	egain=gain;
}
//////////////////////////////////////////////////////////////////////////////////////
bool Carriers::GetGainPart() {
	return egain;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::SetVx(double velx){
	vx=velx;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::SetVy(double vely){
	vy=vely;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::GetVx(){
	return vx;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::GetVy(){
	return vy;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::SetCharge(int valcharge){
	charge=valcharge;
}
//////////////////////////////////////////////////////////////////////////////////////
int Carriers::GetCharge(){
	return charge;
}
//////////////////////////////////////////////////////////////////////////////////////
void Carriers::SetLeftovergain(double vallog=0){
	leftovergain=vallog;
}
//////////////////////////////////////////////////////////////////////////////////////
double Carriers::GetLeftovergain(){
  return leftovergain;
}
//////////////////////////////////////////////////////////////////////////////////////
double Vsat_e(double Temp, int DType){

  float vs = 0;
  if( DType == 0 ||  DType == 2)
    {

      double vsoe = 1.07e7; //velocity saturation model according to Sentaurus Device
      double vse = 0.87;
//       double C=0.8;
//       double I=600;
      vs=vsoe*(pow((300/Temp),vse));
    }
  else
    {
      
      vs=VESAT_D_300;
    }
return vs;
}
//////////////////////////////////////////////////////////////////////////////////////
double Vsat_h(double Temp, int DType){

  float vs = 0 ;
  if( DType == 0 ||  DType == 2)
    {
      double vsoh = 8.37e6;
      double vsh = 0.52;
//       double C=0.8;
//       double I=600;
//       vs= vso/(1+C*exp(Temp/I))*VHSAT_300/VESAT_300;     
      vs=vsoh*(pow((300/Temp),vsh)); 
    }
  else
    {
      vs=VHSAT_D_300;
    }


return vs ;
}
//////////////////////////////////////////////////////////////////////////////////////
double Mu_e(double Temp, int DType){

  Temp = 0;
  double tune = 1;
  double mu = 0;
  if( DType == 0 ||  DType == 2)
    {
      //mu = tune*MU_E_300/pow(300.,-2.4)*pow(Temp, -2.4);
      //mu = MU_E_300*pow(Temp, -2.5)/pow(300, -2.5); //Sentaurus
      mu = MU_E_300;
    }
  else
    {
      mu = tune*MU_E_D_300;
    }


  return mu ;
}
//////////////////////////////////////////////////////////////////////////////////////
double Mu_h(double Temp, int DType){

  Temp = 0;
  double tune = 1;
  double mu = 0;
  if( DType == 0 ||  DType == 2)
    {
      //mu = MU_H_300*pow(Temp, -2.2)/pow(300, -2.2); //Sentaurus
      //mu = tune*MU_H_300/pow(300,-2.2)*pow(Temp, -2.2);
      mu = MU_H_300;
    }
  else
    {
      mu = tune*MU_H_D_300;
    }

  return mu ;
}

//////////////////////////////////////////////////////////////////////////////////////
void CreateCharges(Potentials &pot, Carriers *carriers, int hit, void* wfgui)   // crea cariche con MIP
{     // create electrons and holes, evenly distributed	
  

  //  cout << "Choosing Incidente particle " << endl;
  
  WFGUI* gui= (WFGUI*) wfgui; // pointer to gui
  //  cout << "Detector Type = " << gui->CallGetDetType()<< endl;
  //  TFile Myf("root_files/dEdz_200_5mu.root");
  TFile Myf("root_files/Geant_Vin.root");
  int TotPairs = 0;
  int NTotPairs = 0;

  int PairsPer5Micron[100];
  int NPairsPer5Micron[100];

  TBox *bb1 = new TBox(0.,1.,1.,2);
  TBox *bb2 = new TBox(0.,1.,1.,2);
  TBox *bb3 = new TBox(0.,1.,1.,2);
  int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
  int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
  
  int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
  int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
  
  int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
  int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;
  
  bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
  bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
  bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);

  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);
  

  for (int nn = 0;nn<100;nn++)
    {
      PairsPer5Micron[nn]=0;
      NPairsPer5Micron[nn]=0;
    }
  
  int ll = 0;
  int SumPairs=0;
  int SumSteps=0;
  int mm = 0;
  float LastBinRescale = 1;

  //  bool ConstQFlag = 0; // flag to fix the total charge at (75 pairs per micron)*d
  // bool UniformQFlag = 0; //flag to have a uniform charge distribution of 75 pairs per micron



  bool ConstQFlag = gui->GetConstQFlag(); // flag to fix the total charge at (75 pairs per micron)*d
  bool UniformQFlag = gui->GetUniformQFlag(); //flag to have a uniform charge distribution of 75 pairs per micron
  bool UserUniformQFlag = gui->GetUserUniformQFlag(); //flag to have a uniform charge distribution user defined
  bool CoutFlag = (gui->GetBatchOn() == false) ? 1:0; //flag to have a uniform charge distribution of 75 pairs per micron
  int chargescale = gui->GetPrecision();
  cout << "Tracking one e/h pair for every " << chargescale << " pairs" << endl;

  float NormConstFlag = 1.;

  
  gui->GetEllipse()->SetFillStyle(0);

  if (!gui->GetLess2DPlot())
    {
      pot.DriftPal();
      gui->Getcanvasp()->cd();
      gui->Getdhist()->Draw("COLZ");
      bb2->Draw();
      
      if (gui->GetXMax()>=2) 
	{
	  bb1->Draw();
	  bb3->Draw();
	}		    
      
      
    }
      double angle=0;
      //if (gui->GetAngle()==0) angle =0;
      if ( gui->GetAngle()<45)
	{
	  angle =gui->GetAngle()*(TMath::Pi()/180);
	  if (gui->GetBatchRandomOn())  angle =gui->GetAngle()*(TMath::Pi()/180*gRandom->Gaus());
	}
      else if (gui->GetAngle()>=45) 
	{
	  cout<<"Angle set at = 45 deg"<<endl;
	  angle=45*(TMath::Pi()/180);
	}
      cout<<"Incidente Angle = "<< gui->GetAngle() << " Degrees " << endl;
      int DistHisto = 5; //The histogram has the energy deposition every 5 microns  

      if (angle == TMath::Pi()/2) angle = 1.5;
      float Path = pot.GetYMAX()/cos(angle);
      
      if (Myf.IsOpen())  
    {
      //      Myf.ls();
      float EPair = 0;
      float AvPair = 0;
      TH1F * EDist;
	//	TH1F * EDist =  (TH1F*)Myf.Get("AllNL");

      if( gui->CallGetDetType() == 0 ||  gui->CallGetDetType() == 2)
	{
	  EDist =  (TH1F*)Myf.Get("Silicon_Vin");
	  EPair = 3.6;
	  AvPair = LaserPairsPerMicron;
	}
      else
	{
	  EDist =  (TH1F*)Myf.Get("Diamond_Vin");
      	  EPair = 13.6;
	  AvPair = 36;
	}

      
      if (!UserUniformQFlag && !ConstQFlag && !UniformQFlag && CoutFlag)  cout << "MIP non-uniform charge deposition and Landau amplitude " << endl;
      if (!UserUniformQFlag && ConstQFlag  && !UniformQFlag && CoutFlag)  cout << "MIP non-uniform charge deposition and Fixed amplitude " << endl;
      if (!UserUniformQFlag && ConstQFlag  &&  UniformQFlag && CoutFlag)   cout << "MIP uniform charge deposition " << endl;
      if ( UserUniformQFlag && CoutFlag)   cout << "MIP uniform charge deposition with User Charge = " << gui->GetChargeEntry() << " pairs/micron" << endl;

      if (gui->GetBatchOn()==false) gui->GetEnhist()->Reset();
      TRandom3 Lan;
      TDatime *time;		// current time
      time = new TDatime();	  
      Lan.SetSeed(time->TDatime::GetTime());

      for (int PartNum = 0;PartNum< gui->GetNumberP();PartNum++) // loop on the number of particles
	{
	  ll=0;
	  NTotPairs = 0;
	  
	  for (int il =0; il<=Path; il+=DistHisto)
	    {
	      
	      if (!UniformQFlag)  
		{
		  //		  if (!ConstQFlag && CoutFlag)  cout << "Pairs in each 5-um layer: " << endl;	  
		  float ran = EDist->GetRandom();
		  //float ran = 75*gRandom->Gaus(0, 1.);
		  NPairsPer5Micron[ll] = ran*1e6/EPair;
		}	      
	      else 
		{
		  if (ConstQFlag && ! UserUniformQFlag)  
		    {
		      NPairsPer5Micron[ll] = AvPair*5.;		  
		    }
		  else if (UserUniformQFlag)  
		    {
		      NPairsPer5Micron[ll] = gui->GetChargeEntry()*5.*exp(-1.*ll*DistHisto/Laser1024Range);
		    }
		}
	      if (il+DistHisto>Path) 
		{
		  LastBinRescale = (1.-(double((il+DistHisto*cos(angle)-Path))/double(DistHisto*cos(angle))));
		  NPairsPer5Micron[ll] *= LastBinRescale;
		  // cout << "Last bin rescaled by = " <<  LastBinRescale << endl;
		}
	      //	      if (!ConstQFlag ) cout <<  " ll = " << ll << " " << PairsPer5Micron[ll]<< ", " ;

	      NTotPairs += NPairsPer5Micron[ll] ;
	      ll++;
	    }

	  if (ConstQFlag && !UniformQFlag) //
	    {
	      //Renormalize using 75 pair per micron
	      //	  if ( CoutFlag) cout << "MIP Non uniform Q with Qtot = " << AvPair << " pairs*um = " 
	      //	      << AvPair*pot.GetYMAX()/cos(TMath::Pi()/180*gui->GetAngle()) << endl;	  


	      NormConstFlag = float (AvPair*pot.GetYMAX()/cos(TMath::Pi()/180*gui->GetAngle()) )/float(NTotPairs); //renormalization constant
	      //	      for (mm = 0; mm<ll; mm++)	 NPairsPer5Micron[mm] *=NormConstFlag;
	      
	    }
	  
	  	  
	  if (!ConstQFlag && !UniformQFlag) //
	    {
	      NormConstFlag = 1;
	      //Renormalize following Meroli et al...
	      double d = pot.GetYMAX()/cos(TMath::Pi()/180*gui->GetAngle());
	      double   mpv = (0.027*log(d)+0.126); //keV/micron
	      double  DA = 0.7079*pow(d,-0.266)*mpv;
	      if (PartNum == 0) cout << "# of particles from Landau MPV per micron MPV  = " << 1000*mpv/(3.6) << ",  Width = "  << 1000*DA/(3.6) << endl;

	      double LanConst = 0;
	      //  for (int PartNum = 0;PartNum< gui->GetNumberP();PartNum++)
	      LanConst = Lan.Landau(mpv, DA); //keV/micron: Random Number from the landau distribution	      
	      if (LanConst > 5.*mpv) LanConst = Lan.Landau(mpv, DA);
	      NormConstFlag =  (1000*LanConst*d/3.6)/float(NTotPairs); //renormalization constant	      	      	      	      
	      //    for (mm = 0; mm<ll; mm++)	  NPairsPer5Micron[mm] *=NormConstFlag;
	      cout << "Initial Total Pairs = " << NTotPairs << " Landau normalization constant = " <<  NormConstFlag << " Value of Landau: = "<< (1000*LanConst*d/3.6) << endl;
	    }
	  
		      
	  for (mm = 0; mm<ll; mm++)
	    {
	      PairsPer5Micron[mm] += NPairsPer5Micron[mm]*NormConstFlag;

	      //	      cout << " i = " << mm << " particles =  " << PairsPer5Micron[mm] <<  " iter = " << NPairsPer5Micron[mm]  <<endl;
	      NPairsPer5Micron[mm] = 0;  // reset to 0 the N particle per micron
	    }
	}
      
      for (mm = 0; mm<ll; mm++)
	{
	  //		cout << " i = " << mm << " particles =  " << PairsPer5Micron[mm] << endl;
	  TotPairs +=PairsPer5Micron[mm]; // this is the sum of all pairs.
	}
      
      SumPairs = 0;
      
      gui->SetNumPairs(TotPairs);
      
      	     
      TotPairs  *=1./chargescale;
      int HMax = 0;
      int HMin = 50000;
      
      
      for (mm = 0; mm<ll; mm++)
	{

	  float ypos = 0;
	  float Elypos = 10;
	  float xangle = 0;
	  if (mm != ll-1 && gui->GetBatchOn()==false && gui->GetParticleType() != 4 && gui->GetParticleType() != 5) gui->GetEnhist()->Fill(PairsPer5Micron[mm]);
	  if ( PairsPer5Micron[mm] > HMax)
	    {
	      HMax = PairsPer5Micron[mm];
	    }

	  if ( PairsPer5Micron[mm] < HMin)
	    {
	      HMin = PairsPer5Micron[mm];
	    }	  
	  //	      gui->GetEnhist()->GetXaxis()->SetRangeUser(0.8*HMin,  HMax*1.2);
	  if (gui->GetBatchOn() == false)  gui->GetEnhist()->GetXaxis()->SetRangeUser(0.,  HMax*1.2);
	  
	  // Scale the number by chargescale
	  PairsPer5Micron[mm] *=1./chargescale;

	  
	  for (int i=SumSteps; i<SumSteps+PairsPer5Micron[mm]; i++)
	    {

	      ypos = mm*DistHisto*cos(angle)+((i-SumSteps)/double(PairsPer5Micron[mm]))*double(DistHisto*cos(angle));		  
	      xangle= hit+gRandom->Gaus(0,1) - ypos*tan(angle);
	      //rescale the last bin, as it's not always 5 micron long
	      if (mm == ll-1) ypos = 0.001+ mm*DistHisto*cos(angle)+((i-SumSteps)/double(PairsPer5Micron[mm]))*double(DistHisto*cos(angle)*LastBinRescale);
	      if (ypos>=pot.GetYMAX()-ELECTRODE_DEPTH || ypos < ELECTRODE_DEPTH) continue; //remove particles at the very edges

	      Elypos =  ypos;

	      // super-electrons 
	      //   cout << "electron " << i << " at y = " << ypos << endl;	      
	      carriers[i].Setx(xangle);
	      carriers[i].Sety(ypos);
	      //  
	      carriers[i].SetCharge(-1);
	      carriers[i].Setinside(1);    // yes
	      carriers[i].SetGainPart(false);  //is not a gain electron!
	      // cout << "super-holes" << endl;
	      
	      carriers[TotPairs+i].Setx(xangle);
	      carriers[TotPairs+i].Sety(ypos);
	      carriers[TotPairs+i].SetCharge(1);
	      carriers[TotPairs+i].Setinside(1);    // yes
	      carriers[TotPairs+i].SetGainPart(false); 	//is not a gain hole
	      // cout << "hole " << TotPairs +i << " at y = " << carriers[TotPairs+i].Gety() << endl;
	      //cout<<"pot.getymax= "<<pot.GetYMAX()<<endl;
	      SumPairs++;
	      
	      
	      
	    }

	  SumSteps +=PairsPer5Micron[mm];
	  //draw ellipses
	  
	  if (!gui->GetLess2DPlot())  gui->Getcanvasp()->cd();
	  //	  cout << "Calculating Ellipses " << mm << endl;
	  
	  double eradius= 2*PairsPer5Micron[mm]*0.003*chargescale/(0.5*gui->GetNumberP());
	  //	  cout << Elypos << endl;
	  
	  if (!gui->GetLess2DPlot()) 
	    gui->GetEllipse()->DrawEllipse(xangle,Elypos,eradius,eradius,0.,360.,0.);

	}

      //      cout << "Updating Canvas " << endl;
      if( !gui->GetLess2DPlot()) gui->Getcanvasp()->Update();

      Myf.Close();
    }

  if ( CoutFlag)      cout << "Total number of pairs = " << SumPairs*chargescale << endl;
  pot.Setmipcharge(SumPairs);

  delete bb1;
  delete bb2;
  delete bb3;

  
}

//////////////////////////////////////////////////////////////////////////////////////
void CreateChargesAlphaTop(Potentials &pot, Carriers *carriers, int hit, void* wfgui)   // crea coppie eh da particelle alfa
{     // create electrons and holes, evenly distributed	

  WFGUI* gui= (WFGUI*) wfgui; // pointer to gui
  bool CoutFlag = (gui->GetBatchOn() == false) ? 1:0; 

  TEllipse *ellipse = new TEllipse();
  ellipse->SetFillStyle(0);


  TBox *bb1 = new TBox(0.,1.,1.,2);
  TBox *bb2 = new TBox(0.,1.,1.,2);
  TBox *bb3 = new TBox(0.,1.,1.,2);
  int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
  int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
  
  int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
  int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
  
  int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
  int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;
  
  bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
  bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
  bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);

  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);

  
 
  double angle=0;
  if ( gui->GetAngle()<85) angle =gui->GetAngle()*(TMath::Pi()/180);
  else if (gui->GetAngle()>=85) 
  {
      cout<<"Maximum angle = 45 deg"<<endl;
      angle=45*(TMath::Pi()/180);
  }
    
  double xangle=0;
  if (!gui->GetLess2DPlot()) 
    {
      pot.DriftPal();
      gui->Getcanvasp()->cd();
      gui->Getdhist()->Draw("COLZ");

      bb2->Draw();
      
      if (gui->GetXMax()>=2) 
	{
	  bb1->Draw();
	  bb3->Draw();
	}		    
      

    }
  xangle=hit+gRandom->Gaus(0,1);
  gui->SetPrecision(gui->GetPrecision()*ALPHAPRECISION);
  int chargescale = gui->GetPrecision();
  float TotPairs  =pot.Getmipcharge()/chargescale;
  pot.Setmipcharge(TotPairs);
  float ypos;

  for (int i=0; i<TotPairs; i++)
    {	

      ypos = (double)pot.GetYMAX()- i/((double) pot.Getmipcharge())*gui->GetAlphaRange();
      if (ypos>=pot.GetYMAX()-ELECTRODE_DEPTH || ypos < ELECTRODE_DEPTH) continue; //remove particles at the very edges

      carriers[i].Sety(ypos);
      carriers[i].Setx(xangle+(pot.GetYMAX()-carriers[i].Gety())*sin(angle)+gRandom->Gaus(0,1)); 
      carriers[i].SetCharge(-1);
      carriers[i].Setinside(1);    // yes 
      carriers[i].SetGainPart(false);  //is not a gain electron!
      // super-holes		
      carriers[pot.Getmipcharge()+i].Sety(carriers[i].Gety());
      carriers[pot.Getmipcharge()+i].Setx(xangle+(pot.GetYMAX()-carriers[i].Gety())*sin(angle) +gRandom->Gaus(0,1));	//posiziona la lacuna
      carriers[pot.Getmipcharge()+i].SetCharge(1);
      carriers[pot.Getmipcharge()+i].Setinside(1);    // yes
      carriers[pot.Getmipcharge()+i].SetGainPart(false); 	//is not a gain hole
      //      cout << pot.Getmipcharge()+i << " From Tot" << endl;
      // cout << carriers[i].Gety() << " " << carriers[pot.Getmipcharge()+i].Gety()<< endl;
    }
 
  if ( CoutFlag)	cout << "Alpha range from " << carriers[1].Gety() << " to "<<carriers[pot.Getmipcharge()-1].Gety()<<endl;
  if ( CoutFlag)	cout << "Alpha Energy " <<  ALPHAENERGY*1e-6 << " MeV"<<endl;




  double eradius= 2*LaserPairsPerMicron*5*0.003;  

  for (int il = 0; il< gui->GetAlphaRange(); il++)
    {
      if ((int) il % 5 == 0 && 	 !gui->GetLess2DPlot() )
	{
	  gui->Getcanvasp()->cd();
	  ellipse->DrawEllipse(xangle+(il-2.5)*sin(angle), pot.GetYMAX()-il-2.5,eradius,eradius,0.,360.,0.);
	}
    }
  if (!gui->GetLess2DPlot())   cout << "Updating Canvas, Drawing Ellipses " << endl;
  if (!gui->GetLess2DPlot())   gui->Getcanvasp()->Update(); 

  delete bb1;
  delete bb2;
  delete bb3;
  
  


}
//////////////////////////////////////////////////////////////////////////////////////
void CreateChargesAlphaBottom(Potentials &pot, Carriers *carriers, int hit, void* wfgui)   // crea coppie eh da particelle alfa
{     
  WFGUI* gui= (WFGUI*) wfgui; // pointer to gui
  bool CoutFlag = (gui->GetBatchOn() == false) ? 1:0;
  
  TEllipse *ellipse = new TEllipse();
  ellipse->SetFillStyle(0);

  
  TBox *bb1 = new TBox(0.,1.,1.,2);
  TBox *bb2 = new TBox(0.,1.,1.,2);
  TBox *bb3 = new TBox(0.,1.,1.,2);
  int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
  int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
  
  int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
  int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
  
  int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
  int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;
  
  bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
  bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
  bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);

  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);

  
  double angle=0;
    if ( gui->GetAngle()<45) angle =gui->GetAngle()*(TMath::Pi()/180);
  else if (gui->GetAngle()>=45) 
  {
      cout<<"Maximum angle = 45 deg"<<endl;
      angle=45*(TMath::Pi()/180);
  }
  cout<<"Angle = "<<angle<<endl;
  
  double xangle=0;
  if (!gui->GetLess2DPlot()) 
    {
      gui->Getcanvasp()->cd();
      pot.DriftPal();
      gui->Getdhist()->Draw("COLZ");

      bb2->Draw();
      
      if (gui->GetXMax()>=2) 
	{
	  bb1->Draw();
	  bb3->Draw();
	}		    
      

    }
  xangle=hit+gRandom->Gaus(0,1);

  gui->SetPrecision(gui->GetPrecision()*ALPHAPRECISION);
  int chargescale = gui->GetPrecision();
  float TotPairs  =pot.Getmipcharge()/chargescale;
  pot.Setmipcharge(TotPairs);

  float ypos;

  for (int i=0; i<TotPairs; i++)
    {	

      ypos = i/((double) pot.Getmipcharge() )*gui->GetAlphaRange(); 
      if (ypos>=pot.GetYMAX()-ELECTRODE_DEPTH || ypos < ELECTRODE_DEPTH) continue; //remove particles at the very edges
      
      carriers[i].Sety(ypos);
      carriers[i].Setx(xangle + carriers[i].Gety()*sin(angle)+gRandom->Gaus(0,1)); 	//posiziona l'elettrone nella x in cui la mip colpisce il rivelatore
      carriers[i].Setinside(1);    // yes
      carriers[i].SetCharge(-1);
      carriers[i].SetGainPart(false);  //is not a gain electron!
      

      carriers[pot.Getmipcharge()+i].Sety(ypos);
      carriers[pot.Getmipcharge()+i].Setx(xangle+carriers[i].Gety()*sin(angle)+gRandom->Gaus(0,1));	//posiziona la lacuna
      carriers[pot.Getmipcharge()+i].Setinside(1);    // yes
      carriers[pot.Getmipcharge()+i].SetGainPart(false); 	//is not a gain hole
      carriers[pot.Getmipcharge()+i].SetCharge(1);
 
      //      cout << carriers[pot.Getmipcharge()+i].Gety() << " " << carriers[pot.Getmipcharge()+i].Getx() << endl;
      //    cout << carriers[i].Gety() << " " << carriers[pot.Getmipcharge()+i].Gety()<< endl;
           
    }
  
  if ( CoutFlag)	cout << "Alpha range from " << carriers[1].Gety() << " to "<<carriers[pot.Getmipcharge()-1].Gety()<<endl;
  if ( CoutFlag)	cout << "Alpha Energy " <<  ALPHAENERGY*1e-6 << " MeV"<<endl;

  
  double eradius= 2*LaserPairsPerMicron*5*0.003;  
  
  for (int il = 0; il< gui->GetAlphaRange(); il++)
    {
      if ((int) il % 5 == 0 && !gui->GetLess2DPlot())
	{
	  gui->Getcanvasp()->cd();	  
	  ellipse->DrawEllipse(xangle+(il+2.5)*sin(angle), il+2.5,eradius,eradius,0.,360.,0.);
	}
    }
  if (!gui->GetLess2DPlot())   cout << "Updating canvas, Drawing Ellipses " << endl;
  if (!gui->GetLess2DPlot()) gui->Getcanvasp()->Update();


  delete bb1;
  delete bb2;
  delete bb3;
 
 
}
//////////////////////////////////////////////////////////////////////////////////////
void CreateChargesLaserSide(Potentials &pot, Carriers *carriers, int hit, void* wfgui)   // crea cariche con Laser pulse incidente da destra (edgeTCT)
{
    WFGUI* gui= (WFGUI*) wfgui; // pointer to gui
    //    bool CoutFlag = (gui->GetBatchOn() == false) ? 1:0;
    
    TEllipse *ellipse = new TEllipse();
    ellipse->SetFillStyle(0);
    
    TBox *bb1 = new TBox(0.,1.,1.,2);
    TBox *bb2 = new TBox(0.,1.,1.,2);
    TBox *bb3 = new TBox(0.,1.,1.,2);
    
    int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
    int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
    
    int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
    int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
    
    int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
    int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;
    
    bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
    bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
    bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);
    
    bb1->SetFillColor(1);
    bb2->SetFillColor(1);
    bb3->SetFillColor(1);

    int PairsPer5Micron[1000];
    int NPairsPer5Micron[1000];
    for (int nn = 0;nn<1000;nn++)
    {
        PairsPer5Micron[nn]=0;
        NPairsPer5Micron[nn]=0;
    }
    int TotPairs = 0;   
    int ll = 0;
    int SumPairs=0;
    int SumSteps=0;
    int mm = 0;
    int HMax = 0;
    //int HMin = 50000;
    float xpos = 0;
    float ypos = 0;
    int DistHisto = 5; //The histogram has the energy deposition every 5 microns
    float Path = pot.GetXMAX();
    if (!gui->GetLess2DPlot())
    {
        pot.DriftPal();
        gui->Getcanvasp()->cd();
        gui->Getdhist()->Draw("COLZ");
        
        bb2->Draw();
        
        if (gui->GetXMax()>=2)
        {
            bb1->Draw();
            bb3->Draw();
        }
    }
    
      //    float ypos = hit; //initial y position is specified by user and doesn't change
    int chargescale = gui->GetPrecision();
    
    for (int il =0; il<=Path; il+=DistHisto)
      {
	PairsPer5Micron[ll] = gui->GetChargeEntry()*5.*exp(-1.*il/Laser1024Range);
	TotPairs +=PairsPer5Micron[ll];
	ll++;

      }
    
    TotPairs  *=1./chargescale;
    if( gui->GetBatchOn()==false)    gui->GetEnhist()->Reset();
    

    for (mm = 0; mm<ll; mm++)
      {
	if( gui->GetBatchOn()==false)
	  {
	    if (mm != ll-1) gui->GetEnhist()->Fill(PairsPer5Micron[mm]);
	    if ( PairsPer5Micron[mm] > HMax)
	      {
		HMax = PairsPer5Micron[mm];
		gui->GetEnhist()->GetXaxis()->SetRangeUser(0.,  HMax*1.2);
	      }
	  }
	
	PairsPer5Micron[mm] *=1./chargescale;	    

	
	
	
	
	
	for (int i=SumSteps; i<SumSteps+PairsPer5Micron[mm]; i++)
	  //   for (int i=0; i<TotPairs; i++)
	  {
	    ypos= hit + gRandom->Gaus(0,1);	    
	    xpos = mm*DistHisto+((i-SumSteps)/double(PairsPer5Micron[mm]))*double(DistHisto); //+1 and *.98 to be sure we are withing the active volume
	   
	    carriers[i].Setx(xpos);
	    carriers[i].Sety(ypos);
	    carriers[i].SetCharge(-1);
	    carriers[i].Setinside(1);    // yes
	    carriers[i].SetGainPart(false);  //is not a gain electron!
	    
	    //	    cout << "i =" << i << " x-edge " << xpos<< " y " << ypos <<endl;
	    
	    
	    carriers[TotPairs+i].Sety(carriers[i].Gety());
	    carriers[TotPairs+i].Setx(carriers[i].Getx());	//posiziona la lacuna
	    carriers[TotPairs+i].SetCharge(1);
	    carriers[TotPairs+i].Setinside(1);    // yes
	    carriers[TotPairs+i].SetGainPart(false); 	//is not a gain hole
	    //      cout << pot.Getmipcharge()+i << " From Tot" << endl;
	    // cout << carriers[i].Gety() << " " << carriers[pot.Getmipcharge()+i].Gety()<< endl;
	    SumPairs++;
	  }
	SumSteps +=PairsPer5Micron[mm];
	double eradius= 2*PairsPer5Micron[mm]*0.003*chargescale/(0.5*gui->GetNumberP());
	
	if (!gui->GetLess2DPlot() )
	  {
	    gui->Getcanvasp()->cd();
	    ellipse->DrawEllipse(xpos-2.5, ypos-2.5,eradius,eradius,0.,360.,0.);
	  }
      }
    if (!gui->GetLess2DPlot())   cout << "Updating Canvas, Drawing Ellipses " << endl;
    if (!gui->GetLess2DPlot())   gui->Getcanvasp()->Update();

    pot.Setmipcharge(TotPairs);
}
//////////////////////////////////////////////////////////
void CreateChargesXRay(Potentials &pot, Carriers *carriers, int hitx, int hity, void* wfgui)   // crea coppie eh da particelle alfa
{     // create electrons and holes, evenly distributed	

  WFGUI* gui= (WFGUI*) wfgui; // pointer to gui
  bool CoutFlag = (gui->GetBatchOn() == false) ? 1:0; 

  TEllipse *ellipse = new TEllipse();
  ellipse->SetFillStyle(0);

  TBox *bb1 = new TBox(0.,1.,1.,2);
  TBox *bb2 = new TBox(0.,1.,1.,2);
  TBox *bb3 = new TBox(0.,1.,1.,2);
  int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
  int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
  
  int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
  int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
  
  int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
  int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;
  
  bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
  bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
  bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);

  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);

  
 
  double angle=0;
  if ( gui->GetAngle()<85) angle =gui->GetAngle()*(TMath::Pi()/180);
  else if (gui->GetAngle()>=85) 
  {
      cout<<"Maximum angle = 45 deg"<<endl;
      angle=45*(TMath::Pi()/180);
  }
    
  double xangle=0;
  if (!gui->GetLess2DPlot()) 
    {
      pot.DriftPal();
      gui->Getcanvasp()->cd();
      gui->Getdhist()->Draw("COLZ");

      bb2->Draw();
      
      if (gui->GetXMax()>=2) 
	{
	  bb1->Draw();
	  bb3->Draw();
	}		    
      

    }
  xangle=hitx+gRandom->Gaus(0,3);

  int chargescale = gui->GetPrecision();
  float TotPairs  =pot.Getmipcharge()/chargescale;
  pot.Setmipcharge(TotPairs);
  float ypos;

  for (int i=0; i<TotPairs; i++)
    {	

      ypos = hity- i/((double) pot.Getmipcharge())*10;
      if (ypos>=pot.GetYMAX()-ELECTRODE_DEPTH || ypos < ELECTRODE_DEPTH) continue; //remove particles at the very edges

      carriers[i].Sety(ypos);
      carriers[i].Setx(xangle+(pot.GetYMAX()-carriers[i].Gety())*sin(angle)+gRandom->Gaus(0,1)); 
      carriers[i].SetCharge(-1);
      carriers[i].Setinside(1);    // yes 
      carriers[i].SetGainPart(false);  //is not a gain electron!
      // super-holes		
      carriers[pot.Getmipcharge()+i].Sety(carriers[i].Gety());
      carriers[pot.Getmipcharge()+i].Setx(xangle+(pot.GetYMAX()-carriers[i].Gety())*sin(angle) +gRandom->Gaus(0,1));	//posiziona la lacuna
      carriers[pot.Getmipcharge()+i].SetCharge(1);
      carriers[pot.Getmipcharge()+i].Setinside(1);    // yes
      carriers[pot.Getmipcharge()+i].SetGainPart(false); 	//is not a gain hole
      //      cout << pot.Getmipcharge()+i << " From Tot" << endl;
      // cout << carriers[i].Gety() << " " << carriers[pot.Getmipcharge()+i].Gety()<< endl;
    }
 
  if ( CoutFlag)	cout << "X-Ray range from " << carriers[1].Gety() << " to "<<carriers[pot.Getmipcharge()-1].Gety()<<endl;




  double eradius= 2*TotPairs/gui->GetXRayRange()*5*0.03;  

  for (int il = 0; il< gui->GetXRayRange(); il++)
    {
      if ((int) il % 5 == 0 && 	 !gui->GetLess2DPlot() )
	{
	  gui->Getcanvasp()->cd();
	  ellipse->DrawEllipse(xangle+(il-2.5)*sin(angle)+gRandom->Gaus(0,3), hity-il-2.5,eradius,eradius,0.,360.,0.);
	}
    }
  if (!gui->GetLess2DPlot())   cout << "Updating Canvas, Drawing Ellipses " << endl;
  if (!gui->GetLess2DPlot())   gui->Getcanvasp()->Update(); 
}


//////////////////////////////////////////////////////////////////////////////////////

void SetLorentz(Potentials &pot, Carriers *carriers, double b, double Temp, int DType) {
  double mu_e = Mu_e(Temp, DType);
  double mu_h = Mu_h(Temp, DType);
	for(int i=0; i<pot.Getmipcharge(); i++) {
		carriers[i].Setetheta(atan(mu_e*b)); //1.15
		carriers[i].Seththeta(0.0);
		carriers[i].SetCharge(-1);
	}
	for(int j=pot.Getmipcharge(); j<2*pot.Getmipcharge(); j++) {
		carriers[j].Seththeta(atan(mu_h*b)); //0.72
		carriers[j].Setetheta(0.0);		
		carriers[j].SetCharge(1);
	}	
}
//////////////////////////////////////////////////////////////////////////////////////
void CalculateCurrents(Potentials &pot, Field **dfield, Field **wfield, Carriers *carriers, void* wfgui, int Counter) //pot = un puntatore a dwpot!
{
	WFGUI* gui= (WFGUI*) wfgui; // pointer to gui

	//	const char* fileName = (gui->GetFileName())->c_str();
	const char* fileName = (gui->CallSetFileName())->c_str();
	//	return;
	stringstream ss;
	//	ss << fileName << "_TW";
	// string  fileNameT = ss.str();
	// const char* fileNameTW = fileNameT.c_str();

	stringstream ss1;
	ss1 << fileName << "_" <<  Counter <<".txt" ;
	string  fileNameT1 = ss1.str();
	fileName = fileNameT1.c_str();

	TBox *bb1 = new TBox(0.,1.,1.,2);
	TBox *bb2 = new TBox(0.,1.,1.,2);
	TBox *bb3 = new TBox(0.,1.,1.,2);
	

	FILE* pfile =  NULL;
	//	FILE* pfileTW =  NULL;
	if (gui->GetFileNameOn() ) 
	  {
	    if (gui->GetBatchOn()==false)
	      {
		pfile =  fopen(fileName, "w");	
		//	pfileTW =  fopen(fileNameTW, "w");	
	      }
	    else
	      {
		//		pfile=  fopen(fileName, "a+");	
		// pfileTW=  fopen(fileNameTW, "a+");	
		pfile=  fopen(fileName, "w");	
		//		pfileTW=  fopen(fileNameTW, "w");	
	      }
	    if (pfile==NULL) cout<<"Unable to open "<<fileName<< " file"<<endl;
	  }
	//	int tpath = (int)(0.000368984*pot.GetYMAX()*pot.GetYMAX()+7);	// estimate time in ns the particles need to leave detector
	
//number of time steps, simulation parameter: Total time = timeunit*tmax= 1.0e-12*3.0e4 = 30 ns


	double TIMEUNIT= 1.e-9/gui->GetSampling();	//time step in s
	// cout << "TimeUNit" << TIMEUNIT << endl;

	//	int TMAX= (int) 30*1.e-9/TIMEUNIT; //Nicolo //=10000;

	int TMAX= (int) TIMEMAX; //Nicolo /;  

	bool bStop=false;		//bool to join currentsthread
	double edpar=0;			// absolute value of E field
	double mu=0;				// carrier mobility
	double ynew=0;				// new y position of carrier after calculation
	double xnew=0;				// new x position of carrier after calculation
	double ex=0,ey=0;			// x and y component of drift field
	double qe=0,qh=0,qeh=0,qeg=0,qhg=0,qehg=0,qetot=0,qhtot=0,qtot=0;		// collected charge of electrons, holes and total
  
	double *itot=new double[TMAX];// total induced current
	double *ih=new double[TMAX];	// induced current of non gain holes
	double *ie=new double[TMAX];	// induced current of non gain electrons
	double *ieg=new double[TMAX];	// induced current of gain electrons
	double *ihg=new double[TMAX];	// induced current of gain holes
	double *ietot=new double[TMAX];//induced current of electrons
	double *ihtot=new double[TMAX];//induced current of holes
	double *VoscGraph=new double[TMAX];// total induced current for oscilloscope graph
	double *BBGraph=new double[TMAX];// total induced current for oscilloscope graph
	double *qtot_sh=new double[TMAX];// total induced current for oscilloscope graph

	


	// double *ShaperGraph=new double[TMAX];// Shaper graph
	double d=0;
	double gain=0;

	int firstfreeslot=2*pot.Getmipcharge();

	//	cout << "FirstfreeSlot = " << firstfreeslot << endl;

	int carrierslimit=0;

	TRandom3 *xi;		// create random number
	TDatime *time;		// current time
	time = new TDatime();
	xi = new TRandom3(time->TDatime::GetTime());  //seed = current time
	// double k = xi->Uniform(); // uniformly distributed random number
	//	double rand = xi->Uniform();
	//	double vabs=0,rx=0,ry=0,vdiffy=0,vdiffx=0; // absolute value of diffusion velocity
	double me = 0.26*m0; // mass of electron see: DOI: 10.1109/TNS.2009.2021426
	double mh = 0.36*m0; // mass of holes
	double Temp = gui->GetT();
	double m=0;  //temporary mass value

	//Nicolo'
	int IMax = 0; //Max index of currents
	int IMaxSh = 0; //Max index of currents
	int NIsysMax = 0;
	int NDer0_sh=0;
	int NDer0_BB=0;
	double TimeMax = 0; //Max time of window
	double tau_RC=0;
	double tau_BW=0;
	double tau_BB_BW=0;
	double tau_BB_RC=0;
	double tau_C50=0;
	double alpha_BB_LCR = 0;
	double omega_BB_LCR = 0;
	double alpha_CSA_LCR = 0;
	double omega_CSA_LCR = 0;
	double alpha_Osc_LCR = 0;
	double omega_Osc_LCR = 0; 

	double s_1_BB_LCR = 0;
	double s_2_BB_LCR = 0;

	double s_1_CSA_LCR = 0;
	double s_2_CSA_LCR = 0;

	double s_1_Osc_LCR = 0;
	double s_2_Osc_LCR = 0;
	
	double tau=0;
	double Jitter = 0;


	int chargescale = gui->GetPrecision();
	
	double beta_e = beta_0_e*pow((Temp/300),beta_exp_e);
	double beta_h = beta_0_h*pow((Temp/300),beta_exp_h);
	double inv_beta_e = 1/beta_e;
	double inv_beta_h = 1/beta_h;
	
	//	int dop=1;
	// if(pot.GetDoping()==true) dop=1;	// if strips are PTYPE, set dop = 1
	// if(pot.GetDoping()==false) dop=-1;	// if strips are NTYPE

	double theta1=carriers[1].Getetheta(); // lorentz angle of electrons
	double theta2=carriers[pot.Getmipcharge()+1].Geththeta(); // lorentz angle of holes
	double DiffDistance=0;

	// cout << " Theta1 "<< theta1 << " theta2 " << theta2 << endl;
	for(int i=0; i<TMAX; i++) { // initialize currents to zero
	      itot[i]=0.0;	  
	      ie[i]=0.0;
	      ih[i]=0.0;
	      ieg[i]=0.0;
	      ihg[i]=0.0;
	      ihtot[i]=0.0;
	      ietot[i]=0.0;
	      VoscGraph[i]=0.0;	  
	      BBGraph[i]=0.0;	  
	      qtot_sh[i]=0.0;	  
	}
	
	Field** tempdf1;	 // temporary field for electrons
	tempdf1 = new Field*[(pot.GetYMAX())]; // allocate memory
	for (int i = 0; i < (pot.GetYMAX()); i++) tempdf1[i] = new Field[(pot.GetXMAX())];
	//tempdf1=RotateField(pot,dfield,theta1); // rotate field vectors about theta1 for calculation of drift velocity
	RotateField(pot,tempdf1,dfield,theta1); // rotate field vectors about theta1 for calculation of drift velocity

	Field** tempdf2; // temporary field for holes
	tempdf2 = new Field*[(pot.GetYMAX())]; // allocate memory vettore di puntatori a field
	for (int i = 0; i < (pot.GetYMAX()); i++) tempdf2[i] = new Field[(pot.GetXMAX())];
	//tempdf2=RotateField(pot,dfield,theta2); // rotate field vectors about theta2 for calculation of drift velocity
	RotateField(pot,tempdf2,dfield,theta2); // rotate field vectors about theta2 for calculation of drift velocity

	
	int totaltime = TMAX;	//total simulation time, IT CAN BE CHANGED
	float ygainlow= gui->Getygainlow();
	float ygainhigh= gui->Getygainhigh();
	float GConst = 0;
	GConst = gui->GetGainRatio();

	if(gui->GetBatchOn()==false)
	{
	  
	    	if (gui->GetGainon()==true )
		{
		    cout<<"Detector with internal gain. Gain layer extends from "<<ygainlow<<" to "<< ygainhigh <<endl;
		    cout << "Ratio h/e gain = " <<  gui->GetGainRatio() << endl;

		}
		else
		{
		    cout<<"Detector without internal gain."<<endl;
		}
	    
	}
    
	double fluence = gui->GetIrradiation();//get fluence from gui in units of 10 ^13 neq/cm^2 (equivalent 1Mev neutron following NIEL)
	
	double mu_e = Mu_e(Temp, gui->CallGetDetType());
    
    /*
     //effects of irradiation on electron mobility
     
    if (fluence >0){
        
        double temperature = gui->GetT();
        
        //define constants whose values are experimentally determined for various fluences
        double a = 4.2*pow(10,6);
        double alfa = -1.4;
        double b = 0;
        double c = 0;
        double d = 0;
        
        //all data from WFM and just for electronrmend by any other work, but it's a recent work, not really confo
        
        if (fluence>=0.1 && fluence<1){
            b = 1.5;
            c = pow(10,7);
            d = 1.6*pow(10,3);
        }
        if (fluence>=1 && fluence<10){
            b = 1.8;
            c = 2*pow(10,6);
            d = pow(10,3);
        }
        if (fluence>=10 && fluence<100){
            b = 3;
            c = 6.5*pow(10,5);
            d = pow(10,3);
        }
        if (fluence>=100 && fluence<1000){
            b = 3;
            c = 5*pow(10,5);
            d = pow(10,3);
        }
        
        mu_e = pow(10,-4)/(1/(a*pow(temperature,alfa))+1/(b*pow(temperature,1.5))+1/(c*pow(temperature,-1))+1/(d*pow(temperature,0.5)));//converted it from cm^2 in the article to m^2 here
        
    }
    */

	double mu_h = Mu_h(Temp, gui->CallGetDetType());
	double mu_e_over_v_e = (double) MU_E_300/VESAT_300;//(mu_e/Vsat_e(Temp, gui->CallGetDetType()));
	double mu_h_over_v_h = (double) MU_H_300/VHSAT_300;// (mu_h/Vsat_h(Temp, gui->CallGetDetType()));

	if(gui->GetBatchOn()==false)
	{
	    cout << "Mobility e = " << mu_e << " [m2/(V*s)]" << " Mobility h = " << mu_h << " [m2/(V*s)]"<< endl;
	    cout << "V Saturation:  e = " << Vsat_e(Temp, gui->CallGetDetType()) << " [m/s]"<<  "  h = " << Vsat_h(Temp,gui->CallGetDetType() ) << " [m/s]"<<endl;
	    cout << "Maximum time limit simulated " << TMAX*TIMEUNIT*1e9 << " nanoseconds "<<endl;
	}
	
	double dist = 0;
	//double InitX = 0;
	//double InitY = 0;
	double itot_max= 0;
	double Iout_max_Scope= 0;
	double BBout_max= 0;
	double BBout_min= 0;
	double BBGraph_der_max= 0;
	double BBGraph_der_min= 0;
	double BBout04_max= 0;
	double BBout04_min= 0;
	double csa_max= 0;
	double sh_max= 0;
	double sh_min= 0;
	double sh_der_max= 0;
	double sh_der_min= 0;
	int Nsh_der_max= 0;
	int Nsh_der_min= 0;
	float Der_min = 0;
	float Der_max = 0;
	
	
	float DielConst;
	if( gui->CallGetDetType() == 0 ||  gui->CallGetDetType() == 2)
	  {
	    DielConst = 11.9;
	  }
	else
	  {
	    DielConst = 5.7;
	  }
	
	double DCap = (gui->GetSWidth()*gui->GetSWidth())/pot.GetYMAX()*DielConst*8.854e-3; //fF Backplane
	DCap += 0.017*DielConst*4*gui->GetSWidth(); //fF side
	DCap += 50; //fF fixed


	double TRise = gui->GetTRise()*1e-9;
	double TFall = gui->GetTFall()*1e-9;

	double TauRise = TRise/2.2;
	double TauFall = TFall/2.2;

	if(TauRise == TauFall) TauRise *= 0.9;

	double Noise =  2.*DCap/sqrt(TRise*1e9); // ~ 300 electron of Noise at 5.5 ns shaping time (NA62)

	//CSA parameters


	double CSATransImp = gui->GetShTrans(); //Transimpedance amplifier 		  
	float CSA_Noise = gui->GetShNoise();

	//BB Amplifier parameters (50 input impedance fixed)


	double BBBW = gui->GetBBBW(); // BB BW
	float BB_Noise = gui->GetBBNoise(); //BB Noise
	int BBGain = gui->GetBBGain(); //BB gain		  
	double BBImp = gui->GetBBImp(); // BB input impedance

	//	cout << BBImp<< endl;

	tau_RC = 1.0e-12*gui->GetCSAImp()*gui->GetCDet(); // 
	tau_C50 = 1.0e-12*50.*gui->GetCDet(); //Oscil. RC
	tau_BW = 0.35/(1.0e9*gui->GetOscBW())/2.2; // Oscil. RC
	tau_BB_RC = 1.0e-12*BBImp*gui->GetCDet(); // BB RC
	tau_BB_BW = 0.35/(1.0e9*BBBW)/2.2; // BB Tau
	float Inductance = gui->GetLDet()*1.0e-9;

	alpha_BB_LCR = BBImp/(2.*Inductance);
	omega_BB_LCR = 1./pow(Inductance*gui->GetCDet()*1.0e-12,0.5); 
	s_1_BB_LCR =  - alpha_BB_LCR + pow( pow(alpha_BB_LCR,2.) - pow(omega_BB_LCR,2.),0.5);
	s_2_BB_LCR =  - alpha_BB_LCR - pow( pow(alpha_BB_LCR,2.) - pow(omega_BB_LCR,2.),0.5);

	alpha_CSA_LCR = gui->GetCSAImp()/(2.*Inductance);
	omega_CSA_LCR = 1./pow(Inductance*gui->GetCDet()*1.0e-12,0.5); 
	s_1_CSA_LCR =  - alpha_CSA_LCR + pow( pow(alpha_CSA_LCR,2.) - pow(omega_CSA_LCR,2.),0.5);
	s_2_CSA_LCR =  - alpha_CSA_LCR - pow( pow(alpha_CSA_LCR,2.) - pow(omega_CSA_LCR,2.),0.5);

	alpha_Osc_LCR = 50./(2.*Inductance);
	omega_Osc_LCR = 1./pow(Inductance*gui->GetCDet()*1.0e-12,0.5); 
	s_1_Osc_LCR =  - alpha_Osc_LCR + pow( pow(alpha_Osc_LCR,2.) - pow(omega_Osc_LCR,2.),0.5);
	s_2_Osc_LCR =  - alpha_Osc_LCR - pow( pow(alpha_Osc_LCR,2.) - pow(omega_Osc_LCR,2.),0.5);
	


	//	cout << "Ratio alpha/omega = " << alpha_LCR/omega_LCR << " " << alpha_LCR << " " << omega_LCR <<  endl;
	//	cout << "s1,s2 = "  << s_1_LCR << " " << s_2_LCR <<  endl;

	tau = sqrt( pow( (double) ((0.35/(1.0e9*gui->GetOscBW()))/2.2),2.0) + pow( (double) ((1.0e-12*gui->GetCSAImp()*gui->GetCDet())),2.0));




	bool FVTh = true;
	bool BBFVTh = true;

	if (gui->GetOscOn()==true  && gui->GetBatchOn()==false)
	{
	    //	    cout << "Predicted System Tau (C_det * R=50 Ohm)= " << tau_RC*1e9tau_RC*1e9 << " ns" << endl;
	    cout << "Oscilloscope  BW = " << gui->GetOscBW() << " GHz" << endl;
	    cout << "Shaper Int. Time (10-90 %)= " <<  TRise*1e9 << " ns" << endl;
	    cout << "Shaper Fall Time = " << TFall*1e9 << " ns" << endl;
	    cout << "Noise (Assuming square pixel) = " << Noise << " ENC" << endl;
	    //	    cout << "Shaper Threshold set at VTh = " << VTh << " fQ" << endl;
	    //	    cout << "CSA Threshold set at VTh = " << CSAVth << " mV" << endl;

	}
	double STime = 0;	
	int IintTime= 0;
	float GainScale =  0;
	float ChargeScale =  0;
	float HistoMax = 0;
	float ExChargeCloud = 0;
	float Hoffset = 0;
	float QAverage = 0 ;
	int xLat = 0;
	float Pair = 0;
	int LostCarriers = 0;
	int ActiveCarriers = 0;
	int ActiveCarriersm1 = 0;
	
	
	//	int IintTimeSh= 0;
	// int IintTimeSCA= 0;

	if (gui->GetOscOn()==true)
	{
	  IintTime =  2*(TRise+TFall)/double(TIMEUNIT);  //Extra bins for integration time
	}

	if (gui->GetCalibFlag()) goto CALIB;

	gui->Getchist()->SetMarkerColor(2);
	gui->Getchist()->SetMarkerStyle(2);
	gui->Getchist()->SetMarkerSize(0.1);

	gui->Getchhist()->SetMarkerColor(4);
	gui->Getchhist()->SetMarkerStyle(4);
	gui->Getchhist()->SetMarkerSize(0.1);


	gui->Getctothist()->SetMarkerColor(3);
	gui->Getctothist()->SetMarkerStyle(3);
	gui->Getctothist()->SetMarkerSize(0.1);


	for (int i=0; i<totaltime; i++)	
	  {
	    if (gui->Getstopped()==true) 
	      {
		bStop=true;
		break;
	      }
	    //	    cout << i << " totaltime" << endl;
	    //if(i==2) return;
	    
	    // sweep over all supercharges
	    
	    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    gui->Getchist()->Reset();
	    gui->Getchhist()->Reset();
	    gui->Getctothist()->Reset();

	    
	    int xqmin = 1000;
	    int yqmin = 1000;
	    int xqmax = 0;
	    int yqmax = 0;
	    //	  cout << i << " -20 " << gui->Getchist()->GetMaximumStored() << endl;


	    carrierslimit=firstfreeslot-1;

	    if(i==0 && gui->GetBatchOn()== true)
	      {
		if (gui->GetParticleType() != 8)  gui->GetEnhist()->Fill(firstfreeslot/2*chargescale/pot.GetYMAX());
		else  gui->GetEnhist()->Fill(firstfreeslot/2*chargescale/pot.GetXMAX());
	      }
	    
	  
	    //	    cout<< "t= "<<i<<" firstfreeslot="<<firstfreeslot<<endl;
          int first_loop_over = 2;

          if (i==0){
            first_loop_over = 0;
          }
	  
	    for (int j=0; j<carrierslimit ; j++)
	      {
		
		// only if the charge is still inside the detector and if it is initialized
		
		if (carriers[j].Getinside()!=0 && carriers[j].Getinside()!=-1)	//if charges are inside the detector
		  {

		    ExChargeCloud = 0;
		    if (gui->GetChargeCloud() && (gui->GetParticleType() != 4 && gui->GetParticleType() != 5) )
		      {
			//The cloud field can be in any direction in the y-z plane		      
			ExChargeCloud = gui->GetExhist()->GetBinContent((int)carriers[j].Getx(), (int)carriers[j].Gety());
			ExChargeCloud *=cos(2*TMath::Pi()*gRandom->Uniform(-1.,1) ) ;
		      }
		    
		    // calculate drift velocity
		    if (carriers[j].GetCharge()==-1)	//electrons
		      {
			ex=tempdf1[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()+ExChargeCloud;

			//			if (j<10) cout << tempdf1[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy() << " " << ExChargeCloud <<endl;;
				    
			// x component of rotated E-field
			//	cout << (int)carriers[j].Getx() << " " <<  (int)carriers[j].Gety() << " " << gui->GetExhist()->GetBinContent((int)carriers[j].Getx(), (int)carriers[j].Gety()) << endl;
			ey=tempdf1[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy(); // y component of rotated E-field
			edpar=sqrt(ey*ey);	// mobility depends only on the y component of the field
			mu=mu_e/(pow(1+pow((mu_e_over_v_e*edpar),beta_e),inv_beta_e)); // electron mobility
			//cout<<"mu ="<<mu<<", beta_e="<<beta_e<<", inv_beta_e="<<inv_beta_e<<endl;
			m=me;
						
			//Electrons drift toward higher Potential, so against E = -dV/dy
			
			carriers[j].SetVy(-mu*ey);
			carriers[j].SetVx(-mu*ex); // carrier velocity				
			
			if(gui->Getdiffusionon()
			   &&  (carriers[j].Gety() <ygainlow-1 || carriers[j].Gety() > ygainhigh+1 )
			   && (carriers[j].Gety()< (pot.GetYMAX()-ELECTRODE_DEPTH-1))
			   && (carriers[j].Gety()> ELECTRODE_DEPTH+1)
			   )
		      
			  {
			    // Position diffusion
			    
			    //  rand = xi->Gaus(0,1);
			    DiffDistance = pow(2*mu*kB*Temp/(1.6e-19)*TIMEUNIT,0.5)/GRIDDIST; //it's a lenght!			    
			    // Diffusion along x
			    carriers[j].Setx(carriers[j].Getx()+DiffDistance*gRandom->Gaus(0,1)) ; // Set new position
			    // Diffusion along y
			    // away form the electrodes and not in the gain layer
			    // ynew = carriers[j].Gety()+ LONGDIF*DiffDistance;
			    //  cout << rand<< " " << LONGDIF << " " << LONGDIF*DiffDistance << endl;
			    ynew = carriers[j].Gety()+1.*LONGDIF*DiffDistance*gRandom->Gaus(0,1);
			    if (ynew > ELECTRODE_DEPTH+1 && ynew< pot.GetYMAX()-ELECTRODE_DEPTH-1 && ( ynew < ygainlow || ynew > ygainhigh)  ) 
			      carriers[j].Sety(ynew) ;
			  }
		      }
		    else					//holes
		      {

			ex=tempdf2[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()+ExChargeCloud;
			ey=tempdf2[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy(); // y component of rotated E-field
			//	edpar=sqrt(ex*ex+ey*ey);	// calculate abs. value of field
			edpar=sqrt(ey*ey);	// calculate abs. value of field 
			mu=mu_h/(pow((1+pow((mu_h_over_v_h*edpar),beta_h)),inv_beta_h)); // hole 
			m=mh;
			
			//Holes drift toward lower Potential, so with E = -dV/dy
			
			carriers[j].SetVy(mu*ey);
			carriers[j].SetVx(mu*ex); // carrier velocity			
			// }
			
			
			// Diffusion only outside the gain layer
			if(gui->Getdiffusionon() && (carriers[j].Gety() <ygainlow-1 || carriers[j].Gety() > ygainhigh+1 )) 
			  {
			    // Position diffusion			    
			    //			    rand = xi->Gaus();
			    //DiffDistance = rand*pow(2*mu*kB*Temp/(1.6e-19)*TIMEUNIT,0.5)/GRIDDIST; //it's a lenght!
			    			    
			    // Diffusion along x
			    //carriers[j].Setx(carriers[j].Getx()+DiffDistance) ; // Set new position
			    // Diffusion along y
			    // away form the electrodes and not in the gain layer
			    //ynew = carriers[j].Gety()+LONGDIF*DiffDistance;

			    //  rand = xi->Gaus(0,1);
			    DiffDistance = pow(2*mu*kB*Temp/(1.6e-19)*TIMEUNIT,0.5)/GRIDDIST; //it's a lenght!			    
			    // Diffusion along x
			    carriers[j].Setx(carriers[j].Getx()+DiffDistance*gRandom->Gaus(0,1)) ; // Set new position
			    // Diffusion along y
			    // away form the electrodes and not in the gain layer
			    // ynew = carriers[j].Gety()+ LONGDIF*DiffDistance;
			    //  cout << rand<< " " << LONGDIF << " " << LONGDIF*DiffDistance << endl;
			    ynew = carriers[j].Gety()+1.*LONGDIF*DiffDistance*gRandom->Gaus(0,1);


			    if (ynew > ELECTRODE_DEPTH+1 && ynew< pot.GetYMAX()-ELECTRODE_DEPTH-1 && ( ynew < ygainlow || ynew > ygainhigh)  ) 
			      {
				carriers[j].Sety(ynew) ;
			      }
			  }
			
		      }

		    // calculate induced currents using the dot product of velocity and weighting field
		    if (carriers[j].GetGainPart()==false)
		      {	
			if (carriers[j].GetCharge()==-1)
			  ie[i]+=(-1)*(-1)*chargescale*ECHARGE*(wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()*carriers[j].GetVx() 
					       + wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy()*carriers[j].GetVy());   
			else
			  ih[i]+=(-1)*chargescale*ECHARGE*(wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()*carriers[j].GetVx() 
					  + wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy()*carriers[j].GetVy());

		    }
		    else
		    {
			if (carriers[j].GetCharge()==-1)
			  ieg[i]+=(-1)*(-1)*chargescale*ECHARGE*(wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()*carriers[j].GetVx() 
						+ wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy()*carriers[j].GetVy());   
			else
			  ihg[i]+=(-1)*chargescale*ECHARGE*(wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldx()*carriers[j].GetVx() 
					   + wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy()*carriers[j].GetVy());
		    }
		    
		    //	    if (i == 10)
		    //  cout << j << " ie " << ie[i] << " ih " << ih[i] << " x,y= " << (int)carriers[j].Gety() << ","<< (int) carriers[j].Getx() 
		    //	   <<  " Ey " << wfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy() << " Vy= " << carriers[j].GetVy() << endl;
		    // calculate the new position due to motion in the drift field = drift for electrons and holes

		    // Calculate new position
		    
		    ynew=carriers[j].Gety()+carriers[j].GetVy()*TIMEUNIT/GRIDDIST;
		    xnew=carriers[j].Getx()+carriers[j].GetVx()*TIMEUNIT/GRIDDIST;


		    if (carriers[j].GetCharge() > 0 )
		      {
		      gui->Getchhist()->Fill((int) xnew, (int) ynew ,  1.*chargescale);
		      gui->Getctothist()->Fill((int) xnew, (int) ynew , 1.*chargescale);
		      }
		    else
		      {
		      gui->Getchist()->Fill((int) xnew, (int) ynew , 1.*chargescale);
		      gui->Getctothist()->Fill((int) xnew, (int) ynew , -1.*chargescale);
		      }

		    if (xnew<xqmin) xqmin = xnew;
		    if (xnew>xqmax) xqmax = xnew;

		    if (ynew<yqmin) yqmin = ynew;
		    if (ynew>yqmax) yqmax = ynew;
		    
		    //		    cout << "x,y = " << xnew <<" ," << ynew << " charge = " <<  carriers[j].GetCharge() << endl;
		    if(xnew<=0) xnew=pot.GetXMAX()+xnew;     // periodic boundary conditions
		    if(xnew>=pot.GetXMAX()) xnew=0.0+(xnew-pot.GetXMAX());

		    if (j==2000)
		    {
			//if (i == 0)
			//{
			 //   InitX = xnew;
			  //  InitY = ynew;
			//}
			if (i>0) dist += sqrt( pow(carriers[j].GetVy()*TIMEUNIT/GRIDDIST,2) + pow(carriers[j].GetVx()*TIMEUNIT/GRIDDIST,2));
			//   cout << " x = " << xnew << " y  = " << ynew << " distance = "<< dist 
			// <<" Vx = " << carriers[j].GetVx() << " Vy = " << carriers[j].GetVy() << endl;

		    }


		    // check if charge is inside the  Gain Layer
		    
		    double yold =  carriers[j].Gety();
		    double xold =  carriers[j].Getx();
		    int ngain = 0;
		    d = 0;
		    bool ynewInside = false;
		    bool yoldInside = false;

		    if	(ynew> ygainlow && ynew< ygainhigh) ynewInside = true;		   
		    if	(yold> ygainlow && yold< ygainhigh) yoldInside = true;		   


		    // GAIN Cycle
		    
		    //Enters this cycle only if gain !=0 


		    if (gui->GetGainon() &&   (ynewInside || yoldInside) )
		      //			( (ynew> ygainlow && ynew< ygainhigh)  || (yold> ygainlow && yold< ygainhigh) ))
		      {
			// ynew AND yold inside
			if ( ynewInside && yoldInside )
			  {
			    //gain distance= new-old					    
			    d= fabs(ynew-carriers[j].Gety()) ;						    
			  }
			// ynew inside, yold outside
			    //			else if ( ynewInside && (yold< ygainlow || yold > ygainhigh) )
			else if ( ynewInside && !yoldInside )
			  {
			    if (yold< ygainlow) 
			      d = fabs(ygainlow-ynew);
			    else if (yold > ygainhigh) 
			      d = fabs(ygainhigh-ynew);
			  }
			// new outside, old inside
			    //			else if  ( ( ynew < ygainlow || ynew > ygainhigh)  && (yold > ygainlow && yold < ygainhigh) ) 
			else if  ( !ynewInside  && yoldInside ) 
			  {
			    if (ynew< ygainlow) 
			      d = fabs(ygainlow-yold);
			    else if (ynew > ygainhigh) 
			      d = fabs(ygainhigh-yold);
			  }
			
	
			//theory
			/*double Alpha_Const = 1.8*pow(10,-35);
			  float Alpha = pow(LocalE,7)*Alpha_Const*1e-4; //Alpha in micron
			  gain= gui->GetYGain()*(TMath::Exp(Alpha*d) -1.);*/
			//Alpha_Const = 1.8*pow(10,-5);
			//Alpha = 2*500*pow(LocalE,1)*Alpha_Const*1e-4; //Alpha in micron
			
			
			
			double LocalE = fabs(dfield[(int)carriers[j].Gety()][(int)carriers[j].Getx()].GetFieldy())/1e2;
			if (LocalE<200000) LocalE = 0;
			//			if (j == 200) cout << " Local E" << LocalE << endl;
			//if (j % 1000 == 0) cout << j << endl;
			
			//torino lab
			double Alpha_Const=0;
			float Alpha=0;
			Alpha_Const = .2*pow(10.f,-5.f); //Alpha in micron
			Alpha = LocalE*Alpha_Const*d;
		       	if (carriers[j].GetCharge()==1) Alpha *= GConst;

			//			float Gcoef = sqrt(0.083*log(pot.GetYMAX())-0.24);
			//			float gain2= sqrt(gui->GetYGain()-1.)*Alpha;
			//			gain = Gcoef*gain2*gain2+1.05*gain2;			
			//			float gain2 = (Gcoef*gain2*gain2+1.05*gain2)*Alpha;	
		
			gain = sqrt(gui->GetYGain()-1.)*Alpha;
			gain += carriers[j].GetLeftovergain();			
			ngain=gain;
			carriers[j].SetLeftovergain(gain-ngain);
			
			//  cout << " j = " << j << " Gain = " << gain << " ngain = " << ngain << " Leftover = " << gain-ngain << endl;
			// Deal with the left over gain

		
						
			if (ynew > ygainhigh || ynew < ygainlow ) 
			  {
			    if (xi->Uniform() < (gain-ngain))
			      {
				ngain++;					  
			      }
			    carriers[j].SetLeftovergain(0);
			  }
			 
		

			if (firstfreeslot > gui->GetDimMaxCarriers()) 
			  {
			    cout << " Warning: " << endl;
			    cout << " Carriers array at the limit, the program stops" << endl;
			    cout << " Reduce your Gain Scale " << endl;
			    cout << " You created more than "  << firstfreeslot/2 << " particles" << endl;
			    return;
			  }
			

			for (int k=0;k<ngain; k++)
			  {
			    if (ynew/2.+yold/2. > ELECTRODE_DEPTH+1 && ynew/2.+yold/2.< pot.GetYMAX()-ELECTRODE_DEPTH-1)
			    {
			   //cout<<"firstfreeslot= "<<firstfreeslot<<endl;
				carriers[firstfreeslot].SetCharge(-1);	//creates gain electrons
				carriers[firstfreeslot].SetGainPart(true);
				carriers[firstfreeslot].Setinside(1);
				carriers[firstfreeslot].Sety(yold/2.+ynew/2.);
				carriers[firstfreeslot].Setx(xold+gRandom->Gaus(0,0.5));
				
				carriers[firstfreeslot+1].SetCharge(1);	//creates gain holes
				carriers[firstfreeslot+1].SetGainPart(true);
				carriers[firstfreeslot+1].Setinside(1);
				carriers[firstfreeslot+1].Sety(yold/2.+ynew/2.);
				carriers[firstfreeslot+1].Setx(xold+gRandom->Gaus(0,0.5));
				firstfreeslot=firstfreeslot+2;
			    }
			  }//endfor
			//			cout << "Outside gain layer loop" << endl;
		//	ngain = 0;
			
		      } //end if gain is set active in gui

		    /*
		    if(gui->GetChargeCloud() && (gui->GetParticleType() != 4 && gui->GetParticleType() != 5))
		      {
			float deltax = gRandom->Gaus(0,0.3);
			if (ynew +deltax > ELECTRODE_DEPTH+1 && ynew+deltax< pot.GetYMAX()-ELECTRODE_DEPTH-1 && ( ynew+deltax < ygainlow || ynew+deltax > ygainhigh)  )
			  {
			    // needs smoothing
			    //xnew +=gRandom->Gaus(0,0.3);	  // otherwise update position
			    ynew +=deltax;
			  }
		      }
		*/    
		    if ((ynew<ELECTRODE_DEPTH)||(ynew>pot.GetYMAX()-ELECTRODE_DEPTH))
		      {
			carriers[j].Setinside(0);   // if yes, remove it from further calculations
			LostCarriers++;
			// cout << "Skipping j = " << j << " with ynew = " << ynew << endl;
         //Bianca
                  // unita' 10^14 neq/cm2ƒ
            // if trapping xxx the setinside(0)
                  // deltay = ynew-yoldƒ
                  // yold 0 carriers[j].Gety()
                  // deltay ==> trapping
//                    BBx1 = r.Gaus()*BB_Noise;
                  
                  
			continue;
		      }
              

             
                
                //IRRADIATION CAUSED TRAPPING
              if (fluence>0){
                  
                  double timeunit = TIMEUNIT;
                  double beta_electrons = 0.0; //proportionality constant for electrons (in untis of 10^-13 cm^2/ns, later will be corrected from ns to the loop time via the use of timeunit)
                  double beta_holes = 0.0; //proportionality constant for holes (in untis of 10^-13 cm^2/ns, later will be corrected from ns to the loop time via the use of timeunit)
                  double trapping_probability_electrons = 0.0;
                  double trapping_probability_holes = 0.0;
                  double effective_trapping_time_e = 0.0; // in in units given by timeunit
                  double effective_trapping_time_h = 0.0; // iin in units given by timeunit
                 // double t2 = 0.0; //time constant used in voltage dependent trapping probability
                  double Ke = -0.86;
                  double Kh = -1.52;
                  double temperature = gui->GetT();// in K
                  double T0 = 263.15; //in K

                  
                  if (first_loop_over==0){//print just once
		    cout << "Fluence = " << fluence << " [10^13 neq /cm^2]" << endl;
                      first_loop_over = 1;
                  }
                  

                  if (fluence>=0.1 && fluence<=10.0 ) {//from G. Kramberger, V. Cindro, I. Mandic, M. Mikuz, M. Zavrtanik, “Effective trapping time of electrons and holes in different silicon materials irradiated with neutrons, protons and pions”, Nuclear Instruments and Methods in Physics Research A 481 (2002) 297–305, (mixed irradaìiation) (263K)
                    beta_electrons = (4.1*pow(10.0, -3.0)*(timeunit/pow(10,-9)))*pow((temperature/T0), Ke); //(neutron)
                    beta_holes = (6*pow(10.0, -3.0)*(timeunit/pow(10,-9)))*pow((temperature/T0), Kh); //(neutron)
                    //beta_electrons = (5.6*pow(10.0, -3.0))*pow((temperature/263.15), Ke); //(charged hadron)
                    //beta_holes = (7.7*pow(10.0, -3.0))*pow((temperature/263.15), Ke); //(charged hadron)
                    trapping_probability_electrons = beta_electrons * fluence;
                    trapping_probability_holes = beta_holes * fluence;
                  }
                  if (fluence>10.0) {
                      if (first_loop_over==1){//print just once
                          cout << "ATTENTION: For fluences above 10^14 neq/cm^2 the trapping probability is not longer correctly parametrized only as linearly dependent on the fluence, in this case a voltage dependent term should be added" << endl;
                          first_loop_over = 2;
                      }
                      beta_electrons = (4.1*pow(10.0, -3.0)*timeunit/pow(10,-9))*pow((temperature/T0), Ke); //(neutron)
                      beta_holes = (6*pow(10.0, -3.0)*timeunit/pow(10,-9))*pow((temperature/T0), Kh); //(neutron)
                      //beta_electrons = (5.6*pow(10.0, -3.0))*pow((temperature/263.15), Ke); //(charged hadron)
                      //beta_holes = (7.7*pow(10.0, -3.0))*pow((temperature/263.15), Kh); //(charged hadron)
                      effective_trapping_time_e = 1/(beta_electrons * fluence); //+ t2*(pot.Getvbias() - pot.Getvdepl())/100;//include voltage dependance
                      effective_trapping_time_h = 1/(beta_holes * fluence); //+ t2*(pot.Getvbias() - pot.Getvdepl())/100;//include voltage dependance
                      trapping_probability_electrons = 1/effective_trapping_time_e;
                      trapping_probability_holes = 1/effective_trapping_time_h;
                  }
                  if (fluence<0.1 ) {
                      if (first_loop_over==1){//print just once
                          cout << "No significant trapping effects are present with such a low fluence" << endl;
                          first_loop_over = 2;
                      }
                      beta_electrons = (4.1*pow(10.0, -3.0)*(timeunit/pow(10,-9)))*pow((temperature/T0), Ke); //(neutron)
                      beta_holes = (6*pow(10.0, -3.0)*(timeunit/pow(10,-9)))*pow((temperature/T0), Kh); //(neutron)
                      //beta_electrons = 5.6*pow(10.0, -3.0); //(hadron)
                      //beta_holes = 7.7*pow(10.0, -3.0); //(hadron)
                      trapping_probability_electrons = beta_electrons * fluence;
                      trapping_probability_holes = beta_holes * fluence;
                  }
                  
                  /* 
                 //different betas for different fleunces (and articles), but the change in the currentoutput is minimal
                   if (fluence>2 && fluence<90) {//from Olaf Krasel, Claus Gößling, Reiner Klingenberg, Silke Rajek, Renate Wunstorf, “Measurement of Trapping Time Constants in Proton-Irradiated Silicon Pad Detectors”, , IEEE Transactions On Nuclear Science, 51-6 (2004) 3055-3062, (proton irradaìiation) (273K)
                 beta_electrons = 5.13*pow(10.0, -3.0);
                 beta_holes = 5.04*pow(10.0, -3.0);
                 }
                   if (fluence<pow(10, 2) && fluence>=10) {//from Jens Weber, Reiner Klingenberg, “Free Charge Carriers Trapping Properties in Neutron-Irradiated DOFZ Silicon Pad Detectors”, IEEE Transactions On Nuclear Science, 54-6 (2007) 2071-2075, (neutron irradiation) (223K to 293K range)
                 beta_electrons = 2.98*pow(10.0, -3.0);
                 beta_holes = 3.26*pow(10.0, -3.0);
                   }
                  if (fluence > 300, && fluence<10){ //J. Lange , J.Becker, D.Eckstein, E.Fretwurst, R.Klanner, G.Lindstrom, “Charge collection studies of proton-irradiated n- and p-type epitaxial silicon detectors”, Nuclear Instruments and Methods in Physics Research A 624 (2010) 405–409, (proton irradiation) (300K)
                 beta_electrons = 5.3*pow(10.0, -3.0); //(EPI, 5.1 FZ)
                 beta_holes = 7.4*pow(10.0, -3.0); //(EPI, 6.5 FZ)
                 }
                 
                  */
                  double random_number = ((double) rand() / (RAND_MAX));
              
                  //trapping: reasoning is that carrier number scales as exp(-tr_pr*t), so each ns the N is reduced by a fraction which is exp(-tr_pr), thus the probability of not being trapped can be considered as the remainig fraction of carriers N/No= exp(tr_pr), and so the probability of being trapped is 1-exp(-tr_pr*t)
                  if (carriers[j].GetCharge()==-1 && random_number <= (1-exp(-trapping_probability_electrons))){//for electrons
                    //cout<<trapping_probability_electrons<< random_number << endl;
                      carriers[j].Setinside(0);//the charge is trapped if the randmply generated number is lower than the trapping probability
		      LostCarriers++;
                  }
                  if (carriers[j].GetCharge()==1 && random_number <= (1-exp(-trapping_probability_holes))){//for holes
                      carriers[j].Setinside(0);//the charge is trapped if the randmply generated number is lower than the trapping probability
		      LostCarriers++;
                  }
                  
                  if (random_number > (1 - exp(-trapping_probability_holes)) && carriers[j].GetCharge()==1)
                  {
                      
                      carriers[j].Setx(xnew);	  // otherwise update position
                      carriers[j].Sety(ynew);
                  }
                  if (random_number > (1- exp(-trapping_probability_electrons)) && carriers[j].GetCharge()==-1)
                  {
                      
                      carriers[j].Setx(xnew);	  // otherwise update position
                      carriers[j].Sety(ynew);
                  }
                  
                  /*
                  //current is exponentially damped
                  ie[i] = ie[i]*exp(-i*trapping_probability_electrons);
                  ih[i] = ih[i]*exp(-i*trapping_probability_holes);
                  ieg[i] = ieg[i]*exp(-i*trapping_probability_electrons);
                  ihg[i] = ihg[i]*exp(-i*trapping_probability_holes);
                  */
                  
                  continue;
                }
              
              
           // }
            if ((ynew>=ELECTRODE_DEPTH) && (ynew<=pot.GetYMAX()-ELECTRODE_DEPTH) && fluence == 0)
		      {
			carriers[j].Setx(xnew);	  // otherwise update position
			carriers[j].Sety(ynew);			
		      }
		    		    

		  }//end if charges inside detector

	      }//end for carrierslimit drift

	    
	    float LocalEx = 0;
	    int Eradius = 50;

	    //	    // From a disk of uniform charge in 3D to its projection on the x-y plane (approximate)
	  
	    if ( gui->GetChargeCloud() || gui->GetShowCur()  )
	      {
		gui->GetExhist()->Reset();
		//	       	cout << " Time t = " << i << " " << yqmin << " < yq < " << yqmax << " " << xqmin << " < xq < " << xqmax << endl;
		
		int Calib = 50; // 100.; // parmeters to match TCAD
			      

		for (int yq = yqmin; yq<yqmax+1;yq++)
		  {
		    int LocalQy = 0;
		    for (int xq = xqmin; xq<xqmax+1;xq++)
		      {
			LocalQy = gui->Getctothist()->GetBinContent(xq,yq);
			if (LocalQy!=0)
			  {
			//  cout << " Time t = " << i << " at yq = " << yq << " Q = "  << LocalQy << endl;
			//LocalR = fabs(lxqmin-lxqmax)/4.;
		       
			    //			Meanx = LocalxQy/LocalQy;
			
			    //			float InsideQ = 0;
			for (int ilx = -Eradius;ilx<Eradius+1;ilx++)
			  if ( (xq+ilx) > 0 && (xq+ilx) <pot.GetXMAX() && ilx != 0)
			    {
			      // This is a E field in [V/m] generated by LocalQ in [Q/micron]
			      // E = 1/(2pi epsilon * epsilon_si) * 1/r * Lambda
			      // Field ~ 1/r (EField of a charge in a 2D plane, x-z)
			      // LocalQy = unit of Charge in a micron
			      // ilx = distance in unit of Micron
			      // Local Ex = Field in V/m
			      
			      LocalEx = Calib*240.*(ilx)/fabs(ilx)*LocalQy/fabs(ilx);
			      gui->GetExhist()->Fill((int) (xq+ilx), (int) (yq) , LocalEx );

			    }
			
			  }
			

		      }
		  }
		

	      } // endo of ChargeCloud
	    
	    if (gui->Getabscuron())	
	      {
		ie[i] = fabs(ie[i]);
		ih[i] = fabs(ih[i]);
		ieg[i] = fabs(ieg[i]);
		ihg[i] = fabs(ihg[i]);
	      }
	    
	    itot[i]=ie[i]+ih[i]+ieg[i]+ihg[i];		// calculate total sum
	    qtot+=itot[i]*TIMEUNIT;		// sum up total collected charge
	    //cout << ie[i] << " " << ih[i] << " " << ieg[i] << " " << ihg[i] << " " << qtot << endl;
	    qe+=ie[i]*TIMEUNIT;			// sum up collected charge due to electrons motion
	    qh+=ih[i]*TIMEUNIT;			// sum up collected charge due to gain holes motion
	    qhg+=ihg[i]*TIMEUNIT;			// sum up collected charge due to gain holes motion
	    qeg+=ieg[i]*TIMEUNIT;			// sum up collected charge due to gain holes motion
	    
	    VoscGraph[i]=0;
	    BBGraph[i]=0;
	    
	    IMax = i;
	    ActiveCarriers = carrierslimit -  LostCarriers;

	    
	    // cout <<"Active particles = " <<  ActiveCarriers << endl;
	    //	    if (fabs(itot[i-1]) < 1.e-9 && fabs(itot[i-10]) < 1.e-9 && i > 10 )
	    if (fabs(ie[i]) < 1.e-9 && fabs(ih[i]) < 1.e-9 && fabs(ieg[i]) < 1.e-9 && fabs(ihg[i]) < 1.e-9 && i > 10 && ActiveCarriers<500 && ActiveCarriers==ActiveCarriersm1 )
	      {
		if (gui->GetBatchOn()==false)
		  cout << " ========= Results =================" << endl;	
		cout << "Current at 0 in  = " << IMax*TIMEUNIT*1e9 << " nanoseconds. Still active carriers = " << ActiveCarriers << endl;
		
		break; // terminate loop as the current goes to 0
	      }
	    ActiveCarriersm1 =  ActiveCarriers;
	    
	    //	    if (fabs(itot[i]) > itot_max) itot_max = fabs(itot[i]); 
	    
	    if (gui->GetBatchOn()==false)
	      {	
		//		if (fabs(itot[i])>1e-9)  gui->GetProgressBar()->Increment(TIMEUNIT*1e9);

		if (i%20==0)
		  {
		    std::stringstream ss;
		    ss << " Calculating current: Time   "<< i*TIMEUNIT*1e9 << " [ns] " ;
		    gui->GetCalculatingLabel()->SetTitle(ss.str().c_str()); // update progess label title	
		  }
		//	CalculatingLabel->SetBackgroundColor(0x00ff00);
		// CalculatingLabel->SetTitle("Done.");
	      }

	    bool ShowField = false; // set to true to see the field from charge cloud
	    int  IntStep = pot.GetYMAX()/30;
	    if (gui->GetShowCur())
	      {


		if( (int)i%IntStep==0)
		  {
		    if (i==0)
		      {
			gui->Getcanvaspc()->Clear();
			if (ShowField) gui->Getcanvaspc()->Divide(3,1,0.004,0.004);
			else gui->Getcanvaspc()->Divide(2,1,0.004,0.004);
			gui->Getcanvaspc()->cd(1);
			gPad->SetLogy(0);
			gPad->SetLogz(1);
			gui->Getcanvaspc()->cd(2);
			gPad->SetLogy(0);
			gPad->SetLogz(1);
			gPad->SetRightMargin(0.1);

			if (ShowField)
			  {
			    gui->Getcanvaspc()->cd(3);
			    gPad->SetLogy(0);
			    gPad->SetLogz(1);
			    gPad->SetRightMargin(0.2);
			    gui->GetExhist()->SetStats(0);
			    gui->GetExhist()->SetTitle("Charge Cloud E Field  V/m");

			  }
			HistoMax = 35*gui->GetYGain();
			  //gui->Getchist()->GetBinContent(gui->Getchist()->GetMaximumBin())*gui->GetYGain()/2;

			gui->Getchist()->SetMaximum(HistoMax);
			gui->Getchist()->SetStats(0);
			gui->Getchist()->SetTitle(" e^{-} Charge Distribution"); 
			gui->Getchist()->GetXaxis()->SetTitleSize(0.05);		// set title of x-axis
		       	gui->Getchist()->GetYaxis()->SetTitleSize(0.05);		// set title of x-axis	
			gui->Getchist()->GetXaxis()->SetLabelSize(0.045);		// set title of x-axis
			gui->Getchist()->GetYaxis()->SetLabelSize(0.045);		// set title of x-axis

			gui->Getchhist()->SetMaximum(HistoMax);
			gui->Getchhist()->SetStats(0);
			gui->Getchhist()->SetTitle("h^{+} Charge Distribution");
			gui->Getchhist()->GetXaxis()->SetTitleSize(0.05);		// set title of x-axis
			gui->Getchhist()->GetYaxis()->SetTitleSize(0.05);		// set title of x-axis	
			gui->Getchhist()->GetXaxis()->SetLabelSize(0.045);		// set title of x-axis
			gui->Getchhist()->GetYaxis()->SetLabelSize(0.045);		// set title of x-axis

			//			if ( gui->GetChargeCloud() && (gui->GetParticleType() != 4 && gui->GetParticleType() != 5)) Hoffset = 40;
			//else if (!gui->GetChargeCloud() )
			Hoffset = (  pot.Getpitch()/2. > 30)? pot.Getpitch()/2 : 30;
			
			gui->Getchist()->GetXaxis()->SetRange((int) xqmin-Hoffset, (int) xqmax+Hoffset);
			gui->Getchhist()->GetXaxis()->SetRange((int) xqmin-Hoffset, (int) xqmax+Hoffset);

			gui->Getchist()->GetYaxis()->SetRange((int) 0, (int)  pot.GetXMAX());
			gui->Getchhist()->GetYaxis()->SetRange((int) 0, (int) pot.GetXMAX());
			gui->GetExhist()->GetYaxis()->SetRange((int) 0, (int) pot.GetXMAX());

			int bx1 = pot.GetXMAX()/2-pot.Getpitch() - gui->GetSWidth()/2;
			int bx2 = pot.GetXMAX()/2-pot.Getpitch() + gui->GetSWidth()/2;
			
			int bx3 = pot.GetXMAX()/2-gui->GetSWidth()/2;
			int bx4 = pot.GetXMAX()/2+gui->GetSWidth()/2;
			
			int bx5 = pot.GetXMAX()/2+ pot.Getpitch() - gui->GetSWidth()/2;
			int bx6 = pot.GetXMAX()/2+ pot.Getpitch() + gui->GetSWidth()/2;

			

			if (bx1<xqmin-Hoffset) bx1 = xqmin-Hoffset;
			else if (bx1>xqmax+Hoffset) bx1 = xqmax+Hoffset;
			if (bx2<xqmin-Hoffset) bx2 = xqmin-Hoffset;
			else if (bx2>xqmax+Hoffset) bx2 = xqmax+Hoffset;

			if (bx3<xqmin-Hoffset) bx3 = xqmin-Hoffset;
			else if (bx3>xqmax+Hoffset) bx3 = xqmax+Hoffset;

			if (bx4<xqmin-Hoffset) bx4 = xqmin-Hoffset;
			else if (bx4>xqmax+Hoffset) bx4 = xqmax+Hoffset;

			if (bx5<xqmin-Hoffset) bx5 = xqmin-Hoffset;
			else if (bx5>xqmax+Hoffset) bx5 = xqmax+Hoffset;
			if (bx6<xqmin-Hoffset) bx6 = xqmin-Hoffset;
			else if (bx6>xqmax+Hoffset) bx6 = xqmax+Hoffset;
			
			bb1 = new TBox( bx1 , pot.GetYMAX(), bx2,pot.GetYMAX()+3);
			bb2 = new TBox( bx3 , pot.GetYMAX(), bx4,pot.GetYMAX()+3);
			bb3 = new TBox( bx5 , pot.GetYMAX(), bx6,pot.GetYMAX()+3);		

			bb1->SetFillColor(1);
			bb2->SetFillColor(1);
			bb3->SetFillColor(1);			
			
			xLat = xqmin-Hoffset+5;
			//bb = new TBox();
		      }

		    gui->Getcanvaspc()->cd(1);
		    gui->Getchist()->Draw("COLZ BB FB");
		    //		    gui->Getdhist()->Draw("SAME COT2N");

		    bb2->Draw();

		    if (gui->GetXMax()>=2) 
		      {
			bb1->Draw();
			bb3->Draw();
		      }		    
		    
		    std::stringstream ss;
		    ss << " Time = "<< i*TIMEUNIT*1E9 << " [ns]" ;
		    //cout <<  ss.str() << endl;		    
		    //		    string text = "Time = "+ std::to_string (i*TIMEUNIT*1E9) + " [ns]" ;
		    TLatex l;
		    l.DrawLatex( (xLat>0)? xLat : 0,10,ss.str().c_str());
		    
		    gui->Getcanvaspc()->cd(2);
		    gui->Getchhist()->Draw("COLZ");		

		    bb2->Draw();

		    if (gui->GetXMax()>=2) 
		      {
			bb1->Draw();
			bb3->Draw();
		      }		    
		    

		    if (ShowField)
		      {
			gui->Getcanvaspc()->cd(3);
			gui->GetExhist()->Draw("COLZ");
		      }
		    gui->Getcanvaspc()->Update();

		    //		    gui->Getcanvasp()->cd();
		    // l.DrawLatex( (xLat>0)? xLat : 0,10,ss.str().c_str());
		    //  gui->Getcanvasp()->Update();
		    
		  } // Draw every 10 steps	    	    
	      }
	    
	}//end for loop t

	//here endo of current
	//	std::stringstream ss;
	ss << " Done: Current = 0 at Time = "<< IMax*TIMEUNIT*1e9 << " ns " ;
	gui->GetCalculatingLabel()->SetBackgroundColor(0x00ff00);
	//	gui->GetCalculatingLabel()->SetTitle("Done.");
	gui->GetCalculatingLabel()->SetTitle(ss.str().c_str()); // update progess label title		
	if (!gui->GetShowCur() && gui->GetBatchOn()==false)
	  {
	    
	    //	    if ( gui->GetParticleType() != 4  || gui->GetParticleType() != 5 || gui->GetParticleType() != 9 ) // no plots for alpha
	    if (gui->GetParticleType() <4 || gui->GetParticleType() == 6 ||  gui->GetParticleType() == 8)
	      {
		gui->Getcanvaspc()->Clear();				
		//		gui->Getcanvaspc()->Divide(1,1,0.004,0.004);   
		gui->Getcanvaspc()->cd(1);
		gPad->SetLogy(0);
		gui->GetEnhist()->SetTitle("# of e/h in each 5 micron interval");
		//	GetEnhist()->SetTitle("Average # of e/h per micron");
		gui->GetEnhist()->GetXaxis()->SetTitle("Average # of e/h in each 5 micron interval");
		gui->GetEnhist()->GetYaxis()->SetTitle("Entries");			
		gui->GetEnhist()->Draw();
	      }
	    gui->Getcanvaspc()->Update();
	    // return;
	      
	  }


	//scale gain value

	//	if ( gui->GetConstQFlag() && !gui->GetUniformQFlag())

	//{
	if( gui->CallGetDetType() == 0 ||  gui->CallGetDetType() == 2)
	      {
		Pair = LaserPairsPerMicron;
		if (gui->GetParticleType() ==  6 || gui->GetParticleType() == 8 ) Pair = gui->GetChargeEntry();
	      }
	    else
	      {
		Pair = 36;
	      }
	    ChargeScale = 1;
	    if (gui->GetParticleType() ==  2) // to be sure there is no timewalk due to amplitude fluctuations
	      {
		QAverage = gui->GetNumberP()*ECHARGE*float (Pair*pot.GetYMAX()/cos(TMath::Pi()/180*gui->GetAngle()) );
		ChargeScale = fabs(QAverage/(qe+qh));
	      }
	    // cout  << QAverage << " Qtot = " << qtot << " Qe = " << qe << " Qh = " << qh << " Qe + Qh = " << qe+qh  <<" qeg = " << qeg << " " << qhg << endl;
	    //	    GainScale =  (gui->GetYGain()-1.)/( (float) (qtot/(ChargeScale*(qe+qh))-1.);
	    if ( fabs((qtot/(qe+qh))-1.) < 0.5)  GainScale =  1;
	    else 	    GainScale =  (gui->GetYGain()-1.)/( (float) (qtot/(qe+qh))-1.);

	    if (!gui->GetForceGain())
	      {
		GainScale = 1;
	      }
	    //	if (gui->CallSetPart() != 1)
	// {
	//   ChargeScale = 1;
	//  }
	
	    cout << "Gain in simulation = " << qtot/(qe+qh)  <<  " Gain after corrections  = " << GainScale*(qeg+qhg)/(qe+qh)+1 << " Gain requested = "<< gui->GetYGain()  << endl;
	
	qe=0;	
	qh = 0; 
	qeh = 0; 
	qeg = 0; 
	qhg = 0; 
	qehg = 0; 
	qetot = 0; 
	qhtot = 0; 
	qtot = 0; 


	for (int i=0;i<IMax+1;i++)
	  {
	    ie[i] *=ChargeScale;
	    ih[i] *=ChargeScale;
	    ieg[i] *=GainScale*ChargeScale;
	    ihg[i] *=GainScale*ChargeScale;
	    itot[i]=ie[i]+ih[i]+ieg[i]+ihg[i];		// calculate total sum
	    ietot[i]=ie[i]+ieg[i];
	    ihtot[i]=ih[i]+ihg[i];
	    qe+=ie[i]*TIMEUNIT;			// sum up collected charge due to electrons motion
	    qh+=ih[i]*TIMEUNIT;			// sum up collected charge due to holes motion
	    qeh+=(ie[i]+ih[i])*TIMEUNIT;
	    qeg+=ieg[i]*TIMEUNIT;			// sum up collected charge due to electrons motion
	    qhg+=ihg[i]*TIMEUNIT;			// sum up collected charge due to gain holes motion
	    qehg+=(ieg[i]+ihg[i])*TIMEUNIT;
	    qetot+=ietot[i]*TIMEUNIT;
	    qhtot+=ihtot[i]*TIMEUNIT;
	    qtot+=itot[i]*TIMEUNIT;		// sum up total collected charge
	    qtot_sh[i] = qtot; 
	    if (fabs(itot[i]) > itot_max) itot_max = fabs(itot[i]); 
	  }
	//    cout << "Charge collected = " << qtot << endl;

	//	cout  << "Qtot = " << qtot << " Qe = " << qe << " Qh = " << qh << " Qe + Qh = " << qe+qh  <<" qeg = " << qeg << " " << qhg << endl;


	IMax =TMath::Min((int) (IMax+3.*tau/TIMEUNIT), TMAX); // It's an integer	
	TimeMax = IMax*TIMEUNIT; //It's a time 


 CALIB:
	if (gui->GetCalibFlag())
	  {
	    qtot = 0;
	    for (int i=0; i<TMAX;i++)
	      {
		if (i*(TIMEUNIT*1e9)<1.) 		 
		  itot[i]=0;	       
		else if (i*(TIMEUNIT*1e9)<1.+gui->GetCalibEntry()) 
		    itot[i]=1e-6;
		else itot[i]=0;

		qtot+=itot[i]*TIMEUNIT;		// sum up total collected charge
		qtot_sh[i] = qtot; 	       

	      }
	    itot_max = 1e-6;
	    IMax =TMAX; // It's an integer	
	    TimeMax = IMax*TIMEUNIT; //It's a time
	    ss << " Done: Current pulse is "<< gui->GetCalibEntry() << " ns long " ;
	    gui->GetCalculatingLabel()->SetBackgroundColor(0x00ff00);
	    //	gui->GetCalculatingLabel()->SetTitle("Done.");
	    gui->GetCalculatingLabel()->SetTitle(ss.str().c_str()); // update progess label title		
	    
	  }

	double* q; 				// x coordinate for graph Current(time)	
	q = new double[IMax]; 		// allocate memory	
	for(int k=0;k<IMax;k++)  q[k]=TIMEUNIT*(double)k; // set t coordinate
	
	double* qSh = 0; 	      	// x coordinate for graph Current(time)	
	double* qSh_Norm_sh = 0; 	      	// x coordinate for graph Current(time)	
	double* qSh_Norm_BB = 0; 	      	// x coordinate for graph Current(time)	
	double *ShaperOut_Q= 0;
	double *ShaperOut_V = 0;
	double *PreAmp_Q = 0;
	double *ShaperOut_der = 0;
	double *BBGraph_der = 0;
	double *Iout = 0 ;
	double *Iout_Scope = 0;
	double *Iout_C50 = 0;
	double *Iout_BB_RC = 0;
	double *CSAOut = 0;
	float MaxOsc = 0;
	float MinOsc = 0;
	float MaxElec = 0;
	float MinElec = 0;
	int NMax_sh = 0;
	int Nmin_sh = 0;
	int NMax_sc = 0;
	int NMax_BB = 0;
	int NMax04_BB = 0;
	int Nmin04_BB = 0;
	int Nmin_BB = 0;


	
	

	//oscilloscope loop
	//	double CSATransImp = 22.2e15; //Transimpedance amplifier 0.5*4 mV/fC		  

	if (gui->GetOscOn()==true)  //if scope=on,
	  {
	    
	    // nicolo

	    IMaxSh = min( (IMax+IintTime), int(TIMEMAX)); // Maximum number of bins for electronics (limited at 30 ns)


	    qSh = new double[IMaxSh]; 		// allocate memory
	    qSh_Norm_sh = new double[IMaxSh]; 		// allocate memory
	    qSh_Norm_BB = new double[IMaxSh]; 		// allocate memory
	    ShaperOut_Q=new double[IMaxSh];//output voltage
	    ShaperOut_V=new double[IMaxSh];//output voltage
	    PreAmp_Q=new double[IMaxSh];//output voltage
	    ShaperOut_der=new double[IMaxSh];//output voltage
	    BBGraph_der=new double[IMaxSh];//output voltage
	    Iout=new double[IMaxSh];//output voltage
	    Iout_Scope=new double[IMaxSh];//output voltage
	    Iout_C50=new double[IMaxSh];//output voltage
	    Iout_BB_RC=new double[IMaxSh];//output voltage
	    CSAOut=new double[IMaxSh];//output voltage

	    
	    
	    for(int k=0;k<IMaxSh;k++)
	      {
		qSh[k]=TIMEUNIT*(double)k; // set t coordinate
		ShaperOut_Q[k]=0.0;
		ShaperOut_V[k]=0.0;
		PreAmp_Q[k]=0.0;
		ShaperOut_der[k]=0.0;
		BBGraph_der[k]=0.0;		    
		Iout_Scope[k]=0.0;
		Iout_C50[k]=0.0;
		Iout_BB_RC[k]=0.0;
		Iout[k]=0.0;
		if ( (k<IMax && fabs(qtot_sh[k])>0) || ((k<IMax && fabs(qtot_sh[k])>=0) && gui->GetCalibFlag()))  CSAOut[k]=fabs(qtot_sh[k])*1e15;
		else   CSAOut[k] = fabs(qtot)*1e15;
	      }
	  	    
	    
	    int Step = 1;
	    int DStep = 3 ;
	    for (int i=0;i<IMaxSh;i+=Step)
	      {

		float Idif = 0;
		float Idif_BB = 0;
		float Idif_OSC = 0;
		float Qdif_Shaper = 0;
		if (i== 0 ) Idif  = itot[0] ; 
		else if (i>0 && i< IMax)    Idif  = itot[i] - itot[i-Step];
		else Idif = 0;
		

		// Iout_temp reproduces correctly Speiler pag 127//
		// It's the current signal due to the RC of the circuit, WITHOUT the oscilloscope BW

		float Itot10 = 0;
		for (int il = 0;il<Step; il++) Itot10 +=itot[i-il]; 
		
		for (int ll = 0; ll<IMaxSh-i;ll+=Step)  // valid only up to IMaxSh 
		  {		    

		    //CSA Shaper This  is the transfer of charge from the detector to the preamp, integrated over "step"
		    if (alpha_CSA_LCR>omega_CSA_LCR) Iout[i+ll] += Idif*(1.- 0.5*(TMath::Exp(ll*TIMEUNIT*s_1_CSA_LCR)+TMath::Exp(ll*TIMEUNIT*s_2_CSA_LCR) ));
		    else Iout[i+ll] += Idif*(1.-(TMath::Exp(-ll*TIMEUNIT*alpha_CSA_LCR) ));


		    // Oscilloscope R=50 * Cdet transfer of Current, for plotting the Oscilloscope output
		    if (alpha_Osc_LCR>omega_Osc_LCR) Iout_C50[i+ll] += Idif*(1.- 0.5*(TMath::Exp(ll*TIMEUNIT*s_1_Osc_LCR)+TMath::Exp(ll*TIMEUNIT*s_2_Osc_LCR) ));
		    else Iout_C50[i+ll] += Idif*(1.-(TMath::Exp(-ll*TIMEUNIT*alpha_Osc_LCR) ));
		    
		    //BB amplifier
		    if (alpha_BB_LCR>omega_BB_LCR) Iout_BB_RC[i+ll] += Idif*(1.- 0.5*(TMath::Exp(ll*TIMEUNIT*s_1_BB_LCR)+TMath::Exp(ll*TIMEUNIT*s_2_BB_LCR) ));
		    else Iout_BB_RC[i+ll] += Idif*(1.-(TMath::Exp(-ll*TIMEUNIT*alpha_BB_LCR) ));
		    
		    
		    
		    // PreAmp_Q[i+ll] += Itot10*TIMEUNIT*(1.-TMath::Exp(-ll*TIMEUNIT/tau_RC)); //HS pag 101 transfer of Charge
		    if (alpha_CSA_LCR>omega_CSA_LCR)  PreAmp_Q[i+ll] += Itot10*TIMEUNIT*(1.- 0.5*(TMath::Exp(ll*TIMEUNIT*s_1_CSA_LCR)+TMath::Exp(ll*TIMEUNIT*s_2_CSA_LCR) ));
		    else PreAmp_Q[i+ll] += Itot10*TIMEUNIT*(1.-(TMath::Exp(-ll*TIMEUNIT*alpha_CSA_LCR) ));

		  }

		if (i >= Step)
		  {
		    Idif_OSC  = Iout_C50[i] - Iout_C50[i-Step];
		    Idif_BB  = Iout_BB_RC[i] - Iout_BB_RC[i-Step];
		    Qdif_Shaper  = PreAmp_Q[i] - PreAmp_Q[i-Step];
		  }
		else
		  {
		    Idif_OSC = 0;
		    Idif_BB = 0;
		    Qdif_Shaper  = 0;
		  }

		for (int ll = 0; ll<IMaxSh-i;ll+=Step)  // valid only up to IMaxSh 
		  {		    
		    Iout_Scope[i+ll] += Idif_OSC*(1.-(TMath::Exp( (float) -ll*TIMEUNIT/tau_BW))); // This is the convolution with the Scope BW
		    BBGraph[i+ll] += 1e3*BBImp*BBGain*Idif_BB*(1.-(TMath::Exp(-ll*TIMEUNIT/tau_BB_BW))); // This is the convolution with the BB BW
		    ShaperOut_Q[i+ll] += TauFall/(TauFall+TauRise)*Qdif_Shaper*
		      (TMath::Exp(-ll*TIMEUNIT/TauFall)-TMath::Exp(-ll*TIMEUNIT/TauRise)); // [Q] HS eq 4.3 This is the shaper
		  }
		// if (i<100)
		  // cout << i << " " << Step << " " << Idif << " " << Iout_C50[i] << " " <<  Iout_C50[i-Step] << " " << Iout_Scope[i] << endl;
		
		if (fabs(ShaperOut_Q[i])>fabs(sh_max)) 
		  {
		    sh_max = ShaperOut_Q[i];		    
		  }		
	      }

	    //	    cout << "Shaper Max " << sh_max << endl;
	    
	    for (int i=0;i<IMaxSh;i+=Step) 
	      {
		ShaperOut_V[i] = ShaperOut_Q[i]*CSATransImp*1e15*qtot/sh_max; // [mV/fQ *Q/Q]
		//cout << ShaperOut_V[i] << endl;
	      }
	    // Fill in interpolation
	    
	    Iout_max_Scope = 0;
	    sh_max = 0.;
	    
	    for (int i=Step;i<IMaxSh;i+=Step)
	      {
		
		for (int ml = 0;ml<Step;ml++) 
		  if(i>ml) 
		    {
		      Iout_Scope[i-ml] = Iout_Scope[i]-ml*(Iout_Scope[i]-Iout_Scope[i-Step])/Step;
		      ShaperOut_V[i-ml] = ShaperOut_V[i]-ml*(ShaperOut_V[i]-ShaperOut_V[i-Step])/Step;
		      CSAOut[i-ml] = CSAOut[i]-ml*(CSAOut[i]-CSAOut[i-Step])/Step;
		      BBGraph[i-ml] = BBGraph[i]-ml*(BBGraph[i]-BBGraph[i-Step])/Step;
		    }
	     
	    //
	    
		if (fabs(CSAOut[i])>fabs(csa_max))  csa_max = CSAOut[i];		    

		if (ShaperOut_V[i]> sh_max) 
		  {
		    sh_max = ShaperOut_V[i];		    
		    NMax_sh = i;
		  }
		
		if ( ShaperOut_V[i] < sh_min) 
		  {
		    sh_min = ShaperOut_V[i];		    
		    Nmin_sh = i;
		  }
		if (fabs(Iout_Scope[i]) > Iout_max_Scope)    
		  {
		    Iout_max_Scope = fabs(Iout_Scope[i]); 
		    NMax_sc = i;
		  }

		if (BBGraph[i] > BBout_max )    
		  {
		    //		    BBout_max = fabs(BBGraph[i]); 
		    BBout_max = BBGraph[i]; 
		    NMax_BB = i;
		    if (i*TIMEUNIT*1e9>0.37 && i*TIMEUNIT*1e9<0.4)    
		      {
			BBout04_max = (BBGraph[i]); 
			NMax04_BB = i;
			//    cout << Iout04 << " " << NMax04_BB << endl;
		      }
		  }


		if (BBGraph[i] < BBout_min)    
		  {
		    BBout_min = BBGraph[i]; 
		    Nmin_BB = i;
		    if (i*TIMEUNIT*1e9>0.37 && i*TIMEUNIT*1e9<0.4)    
		      {
			BBout04_min = BBGraph[i]; 
			Nmin04_BB = i;
			//    cout << Iout04 << " " << NMax04_BB << endl;
		      }
		  }


	      } // end interpolation
	    
	    int t_sys10 =0;
	    int t_sys90 =0;
	    int t_scope10 =0;
	    int t_scope90 =0;

	    float OscArea = 0;
	    float Iout_max_System = 0;
	    for (int io = 0; io<IMaxSh ; io++) 
	      {
		//		cout << itot_max << " scope " << Iout_max_Scope << endl;
		VoscGraph[io] = Iout_Scope[io]*(itot_max/Iout_max_Scope);	// Technically is a Current, 
		OscArea +=Iout_Scope[io];
		if (fabs(Iout[io])>fabs(Iout_max_System))
		  {
		    Iout_max_System=Iout[io];
		    NIsysMax = io;
		  }
		if (io<=DStep)
		  {
		    ShaperOut_der[io] = (ShaperOut_V[io+DStep]-ShaperOut_V[0])/(1e9*TIMEUNIT*(io+DStep)); //[mV/ns]
		    BBGraph_der[io] = (BBGraph[io+DStep]-BBGraph[0])/(1e9*TIMEUNIT*(io+DStep)); //[mV/ns]
		  }
		    if (io>DStep)
		  {
		    ShaperOut_der[io] = (ShaperOut_V[io+DStep]-ShaperOut_V[io-DStep])/(1e9*TIMEUNIT*2*DStep); //[mV/ns]
		    BBGraph_der[io] = (BBGraph[io+DStep]-BBGraph[io-DStep])/(1e9*TIMEUNIT*2*DStep); //[mV/ns]

		    if (ShaperOut_der[io]> sh_der_max) 
		      {
			sh_der_max = ShaperOut_der[io];
			Nsh_der_max = io;		    
		      }
		    if (ShaperOut_der[io]< sh_der_min) 
		      { 
			sh_der_min = ShaperOut_der[io];
			Nsh_der_min = io;		    
		      }
		    if (BBGraph_der[io] > BBGraph_der_max )    
		      {
			BBGraph_der_max = BBGraph_der[io]; 
		      }
		    if (BBGraph_der[io] < BBGraph_der_min )    
		      {
			BBGraph_der_min = BBGraph_der[io]; 
		      }
		  }

		if (fabs(Iout[io])>fabs(0.1*Iout_max_System) && t_sys10 == 0) t_sys10 = io;
		if (fabs(Iout[io])>fabs(0.9*Iout_max_System) && t_sys90 == 0) t_sys90 = io;
		if (fabs(Iout_Scope[io])>fabs(0.1*Iout_max_Scope) && t_scope10 == 0) t_scope10 = io;
		if (fabs(Iout_Scope[io])>fabs(0.9*Iout_max_Scope) && t_scope90 == 0) t_scope90 = io;
	      }
	    
	    
	    //	    float BBTr =  0.35/2.5;
	    float TotTr =  1e9*TIMEUNIT*(t_scope90-t_scope10);
	    float SysTr =  1e9*TIMEUNIT*(t_sys90-t_sys10);
	    float SysBW =  0.35/SysTr;
	    float SysTau =  SysTr/2.2;


	    //	    cout << BB_Noise << " " << NMax04_BB << " " << BBout04 << endl;

	    float CSA_dVdt =  (fabs(sh_max)> fabs(sh_min))?  sh_max/(NMax_sh*TIMEUNIT*1e9) :  sh_min/(Nmin_sh*TIMEUNIT*1e9);
	    float BB_dVdt = (fabs(BBout_max)> fabs(BBout_min)) ?  BBout_max/(NMax_BB*TIMEUNIT*1e9) :  BBout_min/(Nmin_BB*TIMEUNIT*1e9);
	    float BB04_dVdt = (fabs(BBout04_max)> fabs(BBout04_min)) ?  BBout04_max/(NMax04_BB*TIMEUNIT*1e9) :  BBout04_min/(Nmin04_BB*TIMEUNIT*1e9);
	    float BBJitter =  fabs(BB_Noise/BB04_dVdt);
	    //	    cout << BBout_max << " " << NMax_BB << " " << BBout04 << endl;
	    // cout << BBout_min << " " << Nmin_BB << " " << BBout04 << endl;

	    OscArea *=TIMEUNIT*50*1e12;   

	    cout << "Signal Area = " << fabs(OscArea) <<  " [pVs] " << endl;
	    cout << "Collected charge = " << OscArea/50*1e3 <<  " [fC] "  << endl;
	    cout << "Average Current = " << OscArea/50/(IMax*TIMEUNIT)*1e-6 <<  " [uA] "  << endl;
	    cout << "Preamp. Current Rise time 10%-90% (R = 50 Ohm)  = " << SysTr <<  " [ns] " << endl;
	    cout << "System Rise tau: predicted (from RC) = "<<tau_RC*1e9 << " simulated = " << SysTau <<  " [ns] " << endl;
	    cout << "Simul. System Rise BW  = " << SysBW <<  " [GHz] " << endl;
	    cout << "Scope Rise time 10%-90% (RC+BW)  = " << TotTr <<  " [ns] " << endl;
	    //	    cout << "Optimized Jitter  2 mV*sqrt(BB Tr)/sqrt(Sys Tr)/dV/dt = " << OPJitter <<  " [ns] " << endl;


	    if (bStop==true) 
	      {
		cout<<"Exiting currents calculation"<<endl;
		return;
	      }
	    TRandom3 r;	  
	    // std::time_t t = std::time(0);  // t is an integer type  
	    //time_t t = time(0);  // t is an integer type  
	    // int t = GetTime();  // t is an integer type  
	    // time = new TDatime();
	    // xi = new TRandom3(time->TDatime::GetTime());  //seed = current time

	    r.SetSeed(time->TDatime::GetTime());


	    float CSA_Vt80 = 0;
	    float CSA_Vt20 = 0;
	    float CSA_t80 = 0;
	    float CSA_t20 = 0;


	    float BB_Vt80 = 0;
	    float BB_Vt20 = 0;
	    float BB_t80 = 0;
	    float BB_t20 = 0;
		    		    
	    

	    double BBx1 = 0;
	    double BBx2 = 0;

	    double CSAx1 = 0;
	    double CSAx2 = 0;

	    BBx1 = r.Gaus()*BB_Noise;
	    BBx2 = r.Gaus()*BB_Noise;
	    CSAx1 = r.Gaus()*CSA_Noise;
	    CSAx2 = r.Gaus()*CSA_Noise;

	    // cout << "Noise CSA = " << CSAx1 << " , " <<CSAx2 << endl;
	    // cout << "Noise BB = " << BBx1 << " , " << BBx2 << endl;
	    float BBArea = 0;
	    float CSAArea = 0;
	    gui->SetCSATVth(0.);
	    gui->SetCSAFTVth(0.);
	    gui->SetBBTVth(0.);
	    gui->SetBBFTVth(0.);
	    NDer0_sh = (fabs(sh_max)> fabs(sh_min)) ?  NMax_sh:  Nmin_sh;
	    NDer0_BB = (fabs(BBout_max)> fabs(BBout_min) ) ? NMax_BB :  Nmin_BB;


	    if(Nsh_der_min < Nsh_der_max)
		Der_min = (fabs(BBGraph_der_min)> fabs(sh_der_min) ) ? BBGraph_der_min : sh_der_min ;
	    else	      
		Der_max = (fabs(BBGraph_der_max)> fabs(sh_der_max) ) ? BBGraph_der_max : sh_der_max ;	      

	    
	    float CSA_dVdt_atVth = 0;
	    float BB_dVdt_atVth = 0;
	    float CSA_dVdt_at50pc = 0;
	    float CSA_dVdt_at20pc = 0;	    
	    float BB_dVdt_at20pc = 0;
	    float BB_dVdt_at50pc = 0;
	    float BB_der0 = (fabs(BBout_max)>fabs(BBout_min)) ? BBout_max : BBout_min;
	    float sh_der0 = (fabs(sh_max)>fabs(sh_min)) ? sh_max : sh_min;
	    
	    double BBVth = gui->GetBBVth(); // in mV
	    double CSAVth = gui->GetCSAVth(); // in mV
	    if (BBVth <1) // to use with a CFD
		BBVth *= BB_der0;

	    if (CSAVth <1)
		CSAVth *= sh_der0;
	    
	    cout << "BB Vth = " << BBVth << endl;
	    cout << "CSA Vth = " << CSAVth << endl; 
	    for (int i=0; i<totaltime;i++)
	      {
		BBArea +=BBGraph[i]*TIMEUNIT;
		CSAArea +=ShaperOut_V[i]*TIMEUNIT;
		
		if ((fabs(ShaperOut_V[i])+CSAx1 ) >fabs(CSAVth) && FVTh && i < NDer0_sh-20)
		  {
		   
		    FVTh = false;
		    STime = (double) (i)*TIMEUNIT*1e9;
		    float dVdt = (ShaperOut_V[i+DStep]-ShaperOut_V[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/nSec
		    CSA_dVdt_atVth = dVdt; //mV /nSec
		    Jitter = CSA_Noise/dVdt; // in ns
		    gui->SetCSATVth(STime);
		    gui->SetJitter(Jitter);
		  }

		
		if ( (fabs(BBGraph[i])+BBx1) > fabs(BBVth) && BBFVTh && i < NDer0_BB-20)
		  {

		    BBFVTh = false;
		    STime = (double) i*TIMEUNIT*1e9;
		    float dVdt = (BBGraph[i+DStep]-BBGraph[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/Sec
		    BB_dVdt_atVth = dVdt;
		    //	    cout <<  "i =  " << i << " BBX1 " << BBx1 <<" "  << BBGraph[i+DStep] << " " << BBGraph[i-DStep] << endl;
		  
		    BBJitter = BB_Noise/dVdt; // in ns
		    gui->SetBBJitter(BBJitter);				  
		    gui->SetBBTVth(STime);
		    // break;
		  }

		if ( (fabs(ShaperOut_V[i])+CSAx2)<fabs(CSAVth) && !FVTh &&  i > NDer0_sh + 20)
		  {

		    FVTh = true;
		    float dVdt = (ShaperOut_V[i-DStep]-ShaperOut_V[i+DStep])/(TIMEUNIT*1e9*2.*DStep);
		    gui->SetCSAFTVth((double) i*TIMEUNIT*1e9);
		    gui->SetFJitter( CSA_Noise/dVdt);
		  }


		if ( (fabs(BBGraph[i])+BBx2) <fabs(BBVth) && !BBFVTh &&  i > NDer0_BB+20)
		  {
		    BBFVTh = true;
		    //float dVdt = (BBGraph[i-10]-BBGraph[i+10])/(TIMEUNIT*20);
		    gui->SetBBFTVth((double) i*TIMEUNIT*1e9);
		  }



		if (fabs(ShaperOut_V[i])>0.2*fabs(sh_der0) &&  CSA_Vt20==0)
		  {
		    CSA_t20 = i*TIMEUNIT*1e9;
		    CSA_Vt20 = ShaperOut_V[i];
		    CSA_dVdt_at20pc =  (ShaperOut_V[i+DStep]-ShaperOut_V[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/nSec
		  }
		if (fabs(ShaperOut_V[i])>0.5*fabs(sh_der0) &&  CSA_dVdt_at50pc==0)
		  {
		    CSA_dVdt_at50pc =  (ShaperOut_V[i+DStep]-ShaperOut_V[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/nSec
		  }

		
		if (fabs(ShaperOut_V[i])>0.8*fabs(sh_der0) &&  CSA_Vt80==0)
		  {
		    CSA_t80 = i*TIMEUNIT*1e9;
		    CSA_Vt80 = ShaperOut_V[i];		  
		  }				    



		if (fabs(BBGraph[i])>0.2*fabs(BB_der0) &&  BB_Vt20==0)
		  {
	
		    BB_t20 = i*TIMEUNIT*1e9;
		    BB_Vt20 = BBGraph[i];
		    BB_dVdt_at20pc =  (BBGraph[i+DStep]-BBGraph[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/nSec
		  }
		if (fabs(BBGraph[i])>0.5*fabs(BB_der0) && BB_dVdt_at50pc == 0)
		  {
		    BB_dVdt_at50pc =  (BBGraph[i+DStep]-BBGraph[i-DStep])/(TIMEUNIT*1e9*2.*DStep); //mV/nSec		    
	
		  }
		
		if (fabs(BBGraph[i])>0.8*fabs(BB_der0) &&  BB_Vt80==0)
		  {
		    BB_t80 = i*TIMEUNIT*1e9;
		    BB_Vt80 = BBGraph[i];		  
		  }				    
	      }

	    
	    float CSA_dVdt20_80 = (CSA_Vt80-CSA_Vt20)/(CSA_t80-CSA_t20);
	    float BB_dVdt20_80 = (BB_Vt80-BB_Vt20)/(BB_t80-BB_t20);
	    
	    cout << endl;
	    cout << "Charge Sensitive Amplifier:" << endl;
	    cout << "CSA dV/dt  = " <<  CSA_dVdt <<  " [mV/ns] " << endl;
	    cout << "CSA dV/dt (80-20)% = " <<  CSA_dVdt20_80 << " [mV/ns]" << endl;
	    cout << "CSA dV/dt at Vth Crossing = " <<  CSA_dVdt_atVth << " [mV/ns] (including the effect of noise)" << endl;
	    cout << "CSA dV/dt at 20%  = " <<    CSA_dVdt_at20pc << " [mV/ns]" << endl;
	    cout << "CSA dV/dt at 50%  = " <<    CSA_dVdt_at50pc << " [mV/ns]" << endl;
	    cout << "CSA Jitter (80-20)% = " << fabs(CSA_Noise/CSA_dVdt20_80) <<  " [ns] " << endl;
	    cout << "CSA Jitter at Vth = " << fabs(CSA_Noise/CSA_dVdt_atVth) <<  " [ns] " << endl;	    
	    cout << "CSA VTh Time Crossing =  " << gui->GetCSATVth() <<  " and " << gui->GetCSAFTVth() << " [ns]" << endl;
	    cout << "CSA peak at = " << NDer0_sh*TIMEUNIT*1e9 <<  " [ns]"<< endl;
	    cout << "CSA Maximum = " <<( (fabs(sh_max)> fabs(sh_min)) ?  sh_max:  sh_min) <<  " [mV]"<< endl;	   
	    cout << "CSA Area = " << fabs(CSAArea*1e9) << " [pVs]" << endl;
	    cout << endl;
	    cout << "Broadband:" << endl;
	    cout << "BB dV/dt = " << BB_dVdt <<  " [mV/ns] " << endl;
	    cout << "BB dV/dt (80-20)% = " <<  BB_dVdt20_80 << " [mV/ns]" << endl;
	    cout << "BB dV/dt (t < 0.4 ns) = " << BB04_dVdt <<  " [mV/ns] " << endl;
	    cout << "BB dV/dt at Vth Crossing = " <<  BB_dVdt_atVth << " [mV/ns]  (including the effect of noise)" << endl;
	    cout << "BB dV/dt at 20% = " <<  BB_dVdt_at20pc << " [mV/ns]" << endl;
	    cout << "BB dV/dt at 50% = " <<  BB_dVdt_at50pc << " [mV/ns]" << endl;
	    cout << "BB Jitter (t < 0.4 ns) = " << fabs(BB_Noise/BB04_dVdt) <<  " [ns] " << endl;
	    cout << "BB Jitter (80-20)% = " << fabs(BB_Noise/BB_dVdt20_80) <<  " [ns] " << endl;
	    cout << "BB Jitter at Vth = " << fabs(BB_Noise/BB_dVdt_atVth) <<  " [ns] " << endl;
	    cout << "BB VTh Time Crossing =  " << gui->GetBBTVth() <<  " and " << gui->GetBBFTVth() << " [ns]" << endl;
	    cout << "BB peak at = " << NDer0_BB*TIMEUNIT*1e9 <<  " [ns]"<< endl;
	    cout << "BB Maximum = " << ((fabs(BBout_max)> fabs(BBout_min) ) ? BBout_max :  BBout_min) <<  " [mV]"<< endl;	    
	    cout << "BB Area = " << fabs(BBArea*1e9) << " [pVs]" << endl;




	  } // end of scope 

	for (int io = 0; io<IMax ; io++)  
	  {
	    if (gui->GetOscOn()==true)  //if scope=on,
	      if ( VoscGraph[io]> MaxOsc) MaxOsc =  VoscGraph[io];
	    if ( itot[io]> MaxOsc) MaxOsc =  itot[io];
	    if ( ih[io]> MaxOsc) MaxOsc =  ih[io];
	    if ( ie[io]> MaxOsc) MaxOsc =  ie[io];
	    if ( ihg[io]> MaxOsc) MaxOsc =  ihg[io];
	    if ( ieg[io]> MaxOsc) MaxOsc =  ieg[io];

	    if (gui->GetOscOn()==true)  //if scope=on,
	      if ( VoscGraph[io]< MinOsc) MinOsc =  VoscGraph[io];		
	    if ( itot[io]< MinOsc) MinOsc =  itot[io];
	    if ( ih[io]< MinOsc) MinOsc =  ih[io];
	    if ( ie[io]< MinOsc) MinOsc =  ie[io];
	    if ( ihg[io]< MinOsc) MinOsc =  ihg[io];
	    if ( ieg[io]< MinOsc) MinOsc =  ieg[io];
	  }	    


	if ( gui->GetFileNameOn())
	  {
	    	    
	    Int_t IPrint = 0 ;
	    IPrint = (gui->GetOscOn()) ? IMaxSh : IMax;

	    IPrint = (IPrint >TMAX) ? TMAX : IPrint; // At most, print TMAX point

	    
	    for (Int_t i=0;i<IPrint;i++)
	      {  
		if (i == 0)
		  {
		    if (gui->GetOscOn()==true) fprintf(pfile,"    TIME[ns],       Itot[uA],       Ie[uA],       Ih[uA],       Ieg[uA],       Ihg[uA],       BB[mV],       CSA[mV],       Charge[fQ]\n");
		    else  fprintf(pfile,"TIME[ns],     Itot[uA],      Ie[uA],     Ih[uA],      Ieg[uA],      Ihg[uA] \n");
		  }
		if (i % PRINTFREQUENCY == 0 )
		  {


		    if (gui->GetOscOn()==true) fprintf(pfile,"%E    %E    %E    %E    %E    %E    %E    %E    %E\n",(i)*TIMEUNIT*1e9,
		    				       itot[i]*1e6,ie[i]*1e6,ih[i]*1e6,ieg[i]*1e6,ihg[i]*1e6,
		    			       BBGraph[i],ShaperOut_V[i],CSAOut[i]);
		     else  fprintf(pfile,"%E    %E   %E    %E   %E   %E \n",(i)*TIMEUNIT*1e9,itot[i]*1e6,ie[i]*1e6,ih[i]*1e6,ieg[i]*1e6,ihg[i]*1e6);
		    //fprintf(pfile,"%E %E\n",(i)*TIMEUNIT+ 2.000000E-11,itot[i]);
		  }
	      }
	    return;  // when writing out the files, do not plot anything
	  }
	// previous batchon position
	
	cout << endl;
	if (gui->GetParticleType() != 7 )
	  {
	    cout << "Total number of e-h pair simulated = " << firstfreeslot/2 << endl;	  
	    cout << "Total number of e-h pair  = " << firstfreeslot/2*chargescale << endl;
	  }
	else
	  cout << "Electronic pulse" << endl;

	cout<<"================================================================="<<endl;
	
	TGraph *gr;							
	gr = new TGraph(IMax,q,itot); // Graph for total current
	gr->GetXaxis()->SetTitleSize(0.05);		// set title of x-axis
	gr->GetYaxis()->SetTitleSize(0.05);		// set title of x-axis	
	gr->GetXaxis()->SetLabelSize(0.045);		// set title of x-axis
	gr->GetYaxis()->SetLabelSize(0.045);		// set title of x-axis
	gr->GetXaxis()->SetTitle("Time [s]");		// set title of x-axis
	gr->GetYaxis()->SetTitle("Current [A]");	// set title of y-axis
	gr->SetLineColor(3);					// set line color to green
	gr->SetTitle(" ");						// set title
	gui->Getcurcanvas()->cd();				// change to canvas
	gr->SetLineWidth(5);
	gr->GetXaxis()->SetRangeUser(0,TimeMax*1.1); 
	//	    if (gui->GetOscOn()==true)
	gr->GetYaxis()->SetRangeUser( MinOsc*1.1, MaxOsc*1.1); 
	gui->Getcurcanvas()->cd();				// change to canvas
	gr->Draw("AL");						// draw graph (axis and line)
	
	
	TGraph *gr1;				// Graph for current of holes
	gr1 = new TGraph(IMax,q,ih);
	gr1->SetLineColor(4);		// set line color to blue
	gr1->SetLineWidth(2);
	gui->Getcurcanvas()->cd();
	gr1->Draw("L");
	
	TGraph *gr2;				// Graph for current of electrons
	gr2 = new TGraph(IMax,q,ie);
	gr2->SetLineColor(2);		// set line color to red
	gr2->SetLineWidth(2);
	gui->Getcurcanvas()->cd();
	gr2->Draw("L");
	
	
	if (gui->GetGainon()==true)
	  {
	    TGraph *gr3;				// Graph for current of gain electrons
	    gr3 = new TGraph(IMax,q,ieg);
	    gr3->SetLineColor(6);		// set line color to lightblue
	    gr3->SetLineWidth(2);
	    gui->Getcurcanvas()->cd();
	    gr3->Draw("L");
	    
	    TGraph *gr4;				// Graph for current of gain holes
	    gr4 = new TGraph(IMax,q,ihg);
	    gr4->SetLineColor(7);		// set line color to purple
	    gr4->SetLineWidth(2);
	    gui->Getcurcanvas()->cd();
	    gr4->Draw("L");

	  }
	
	if (gui->GetOscOn()==true)
	  {
	    
	    TGraph *gr20;				// Graph for Current on input impedance
	    gr20 = new TGraph(IMax,q,Iout);
	    gr20->SetLineColor(1);		// set line color to black
	    gr20->SetLineWidth(2);
	    gr20->SetLineStyle(2);
	    gui->Getcurcanvas()->cd();
	    gr20->Draw("L");
	    
	    
	    
	    TGraph *gr5;				// Graph for output voltage
	    gr5 = new TGraph(IMax,q,VoscGraph);
	    gui->Getcurcanvas()->cd();
	    gr5->SetLineColor(1);		// set line color to 
	    gr5->SetLineWidth(1);
	    gr5->SetLineStyle(1);
	    gr5->Draw("L");
	    
	    
	    //		VoscGraph[io] = Iout_Scope[io]*(itot_max/Iout_max_Scope);
	    //		cout <<	"  Iout_max_Scope "  << Iout_max_Scope << " , "<< itot_max << endl;
	    TGaxis*axis;
	    float OscScale = Iout_max_Scope*50; // Current into 50 Ohm
	    if (MinOsc>=0 && MaxOsc>0 ) axis = new TGaxis(TimeMax*1.1,0.1*MaxOsc*1.1,TimeMax*1.1,MaxOsc*1.1,
							  0.1*1e3*OscScale*1.1,1e3*OscScale*1.1,510,"+");
	    
	    if (MinOsc<0 && MaxOsc<=0)
	      axis = new TGaxis(TimeMax*1.1,0.9*MinOsc*1.1 ,TimeMax*1.1,0, 
				-0.9*1e3*OscScale*1.1,0,510,"+");
	    
	    if (MinOsc<0 && MaxOsc> 0)
	      {
		if (fabs(MaxOsc)>fabs(MinOsc))
		  axis = new TGaxis(TimeMax*1.1, 0.9*MinOsc*1.1,TimeMax*1.1,MaxOsc*1.1,
				    -0.9*1e3*Iout_max_Scope*fabs(MinOsc/MaxOsc)*1.1*50,1e3*Iout_max_Scope*1.1*50,510,"+");
		else
		  axis = new TGaxis(TimeMax*1.1, 0.9*MinOsc*1.1,TimeMax*1.1,MaxOsc*1.1,
				    -0.9*1e3*Iout_max_Scope*1.1*50,1e3*Iout_max_Scope*fabs(MaxOsc/MinOsc)*1.1*50,510,"+");
	      }
	    
	    //Draw Total current again, to superpose it
	    gui->Getcurcanvas()->cd();
	    //gr->Draw("L");						// draw graph (axis and line)
	    
	    axis->SetTitle("Voltage [mV]");
	    axis->SetLabelOffset(0.07);
	    axis->SetTitleOffset(1.35);
	    axis->Draw();
	  }
	
	// Shaper  canvas
	if(gui->GetBatchOn()==false)
	  {
	    
	    if (gui->GetOscOn()==true)
	      {
		
		gui->Getosccanvas()->Clear();
		gui->Getosccanvas()->Divide(1,2,0.002,0.002);
		gui->Getosccanvas()->Update();
		TGraph *gr6;							
		gr6 = new TGraph(IMaxSh,qSh,ShaperOut_V);			
		gr6->SetLineColor(4);					// set line color to green
		gr6->SetTitle("CSA (blue) and BB Amplifier (red)");						// set title
		gr6->SetLineWidth(3);
		gui->Getosccanvas()->cd(1);
		MaxElec =  (sh_max > BBout_max) ? sh_max : BBout_max;
		MinElec =  (sh_min < BBout_min) ? sh_min : BBout_min;

		gr6->GetYaxis()->SetRangeUser(MinElec*1.1, MaxElec*1.1); 

		TGraph *grD;			
		grD = new TGraph(IMaxSh,qSh,BBGraph);
		grD->SetLineColor(2);		// set line color to 
		grD->SetLineWidth(2);
		//		gui->Getosccanvas()->cd(1);
		  
		gr6->Draw("AL");
		gr6->GetXaxis()->SetTitleSize(0.05);		// set title of x-axis
		gr6->GetYaxis()->SetTitleSize(0.05);		// set title of x-axis		
		gr6->GetXaxis()->SetLabelSize(0.045);		// set title of x-axis
		gr6->GetYaxis()->SetLabelSize(0.045);		// set title of x-axis

		gr6->GetXaxis()->SetTitle("Time [s]");		// set title of x-axis
		gr6->GetYaxis()->SetTitle("Amplitude [mV]");	// set title of y-axis
		grD->Draw("L");

		//		TGaxis*axis1;
		//axis1 = new TGaxis(IMaxSh*TIMEUNIT*0.8,0.1*sh_max*1.1,IMaxSh*TIMEUNIT*0.8,sh_max*1.1,
		//			 sh_max*0.1*1e-12*CSATransImp/TIMEUNIT*1.1,sh_max*1e-12*CSATransImp/TIMEUNIT*1.1,510,"+");
		
		// axis1->SetTitle("Voltage [mV]");
		// axis1->SetLabelOffset(0.07);
		// axis1->SetTitleOffset(1.4);
		// axis1->Draw();

		
		//		gui->Getosccanvas()->cd(2);
		TVirtualPad *c1_1 = gui->Getosccanvas()->cd(2);
		c1_1->Divide(2,1);
		c1_1->cd(1);
		//	for(int k=0;k<NDer0_sh;k++) qSh_Norm_sh[k] =qSh[k]/qSh[NDer0_sh]	;
		// for(int k=0;k<NDer0_BB;k++) qSh_Norm_BB[k] =qSh[k]/qSh[NDer0_BB]	;

		for(int k=0;k<NDer0_sh;k++) qSh_Norm_sh[k] =ShaperOut_V[k]/ShaperOut_V[NDer0_sh]	;
		 for(int k=0;k<NDer0_BB;k++) qSh_Norm_BB[k] =BBGraph[k]/BBGraph[NDer0_BB]	;

		
		TGraph *gr7;							
		gr7 = new TGraph(NDer0_sh,qSh_Norm_sh,ShaperOut_der);
		//	gr7 = new TGraph(IMax,qSh,CSAOut);
		gr7->SetLineColor(4);					// set line color to green
		gr7->SetTitle("Rising edge derivative CSA (blue) and BB (red)");						// set title
		gr7->SetLineWidth(2);
		gr7->GetXaxis()->SetTitle("Amplitude fraction");		// set title of x-axis
		gr7->GetYaxis()->SetTitle("dV/dt (mV/ns)");	// set title of y-axis
		gr7->GetYaxis()->SetTitleOffset(1.5);
		gr7->Draw("AL");	
		gr7->GetXaxis()->SetRangeUser(0.,sh_max);		// set title of x-axis
		//		gr7->GetYaxis()->SetRangeUser(1.2*Der_min,1.2*Der_max);		// set y range
		if (Der_max>0 && Der_max<70 ) gr7->GetYaxis()->SetRangeUser(0,70);		// set y range
		else if (Der_max<0 && Der_max> -70 ) gr7->GetYaxis()->SetRangeUser(-70,0.);		// set y range	       
		else 	 gr7->GetYaxis()->SetRangeUser(1.2*Der_min, 1.2*Der_max);		// set y range
		TGraph *gr71;							
		gr71 = new TGraph(NDer0_BB,qSh_Norm_BB,BBGraph_der);
		gr71->SetLineColor(2);					// set line color to red
		gr71->SetLineWidth(2);
		gr71->Draw("L");	

		//gui->Getosccanvas()->cd(2);
		//		gPad->cd(2);
		c1_1->cd(2);
		TGraph *grSh;			
		grSh = new TGraph(IMax,qSh,CSAOut);
		grSh->SetTitle("Charge ");						// set title
		grSh->SetLineColor(3);		// set line color to 
		grSh->SetLineWidth(2);
		grSh->GetXaxis()->SetLabelSize(0.045);		// set title of x-axis
		grSh->GetYaxis()->SetLabelSize(0.045);		// set title of x-axis
		grSh->GetXaxis()->SetTitleSize(0.05);		// set title of x-axis
		grSh->GetYaxis()->SetTitleSize(0.05);		// set title of x-axis		
		grSh->GetXaxis()->SetTitle("Time [s]");		// set title of x-axis
		grSh->GetYaxis()->SetTitle("Charge [fQ]");	// set title of y-axis
		grSh->Draw("AL");
		gui->Getosccanvas()->Update();
		/*
		gui->Getosccanvas()->cd(4);
		grD = new TGraph(IMax,q,BBGraph);
		grD->SetTitle("BraodBand 2.5 GHz, 50 Ohm and 40db");						// set title
		grD->GetXaxis()->SetTitle("Time (s)");		// set title of x-axis
		grD->GetYaxis()->SetTitle(" Amplitude [mV] ");	// set title of y-axis
		grD->Draw("AL");
		//		gui->Getosccanvas()->Update();
		gui->Getosccanvas()->Update();

		//TLegend *pieleg = gr6->MakeLegend();
		// pieleg->SetY1(.56); pieleg->SetY2(.86);
*/
	      }
	    
	    
	    char * qestring;		// char array for output of number of collected electrons in gui
	    qestring = new char[20];
	    sprintf(qestring, "%.0lf", qe/ECHARGE);
	    
	    char * qhstring;		// char array for output of number of collected holes in gui
	    qhstring = new char[20];
	    sprintf(qhstring, "%.0lf", qh/ECHARGE);
	    
	    char * qehstring;		
	    qehstring = new char[20];
	    sprintf(qehstring, "%.0lf", qeh/ECHARGE);
	    
	    char * qegstring;		// char array for output of number of collected electrons in gui
	    qegstring = new char[20];
	    sprintf(qegstring, "%.0lf", qeg/ECHARGE);
	    
	    char * qhgstring;		// char array for output of number of collected holes in gui
	    qhgstring = new char[20];
	    sprintf(qhgstring, "%.0lf", qhg/ECHARGE);
	    
	    char * qehgstring;		
	    qehgstring = new char[20];
	    sprintf(qehgstring, "%.0lf", qehg/ECHARGE);
	    
	    char * qetotstring;		// char array for output of number of collected holes in gui
	    qetotstring = new char[20];
	    sprintf(qetotstring, "%.0lf", qetot/ECHARGE);
	    
	    char * qhtotstring;		
	    qhtotstring = new char[20];
	    sprintf(qhtotstring, "%.0lf", qhtot/ECHARGE);
	    
	    char * qtotstring;		// char array for output of total number of collected charges in gui
	    qtotstring = new char[20];
	    sprintf(qtotstring, "%.0lf", qtot/ECHARGE);
	    char * ethetastring;	// char array for output of lorentz angle of electrons in degree in gui
	    ethetastring = new char[20];
	    sprintf(ethetastring, "%.2lf", theta1*180/M_PI);
	    
	    char * hthetastring;	// char array for output of lorentz angle of holes in degree in gui
	    hthetastring = new char[20];
	    sprintf(hthetastring, "%.2lf", theta2*180/M_PI);
	    
	    //gui->SetQLabel(qestring,qhstring,qehstring,qegstring,qhgstring,qehgstring);	// Update Labels in gui
	    gui->SetQLabel(qestring,qhstring,qehstring,qegstring,qhgstring,qehgstring,qetotstring,qhtotstring,qtotstring);
	    gui->SetAngleLabel(ethetastring,hthetastring);


	    
	  }//end if batchon=false
	
	if(gui->GetFileNameOn())
	  {
	    fclose(pfile);
	    //	    fclose(pfileTW);
	  }

	delete bb1;
	delete bb2;
	delete bb3;
	delete xi;

	delete[] q;
	delete[] itot;
	delete[] ih;
	delete[] ie;
	delete[] ihg;
	delete[] ieg;
	delete[] ietot;
	delete[] ihtot;
	delete[] VoscGraph;
	delete[] BBGraph;
	delete[] qtot_sh;
	
	//delete[] tempdf1;
	
	for (int i = 0; i < (pot.GetYMAX()); i++)
	  delete[] tempdf1[i]; //delete Field
	delete[] tempdf1; 
	
	
	//delete[] tempdf2;
	
	for (int i = 0; i < (pot.GetYMAX()); i++){
           if (tempdf2[i] != NULL) delete[] tempdf2[i]; //delete Field
	}
	  
	delete[] tempdf2; 
	

	delete[] qSh;
	delete[] ShaperOut_Q;
	delete[] ShaperOut_V;
	delete[] PreAmp_Q;
	delete[] ShaperOut_der;
	delete[] BBGraph_der;
	delete[] Iout; 
	delete[] Iout_Scope;
	delete[] Iout_C50;
	delete[] Iout_BB_RC;
	delete[] CSAOut;

	/*	grSh->Delete();
	grD->Delete();
	gr6->Delete();
	gr5->Delete();
	gr4->Delete();
	gr3->Delete();
	gr2->Delete();
	gr1->Delete();
	gr->Delete();					
*/	
	
	
}

Carriers::~Carriers() {
}
