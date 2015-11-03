//#include "Potentials.h"
#include "WFGUI.h"
//////////////////////////////////////////////////////////////////////
Potentials::Potentials()  //constructor
{	
  multig=0;
  ref=0;
  XMAX=0;
  YMAX=0;
  pitch=0;
  width=0;
  count=0;
  vbias=0;
  vdepl=0;
  poissonf=0;
  **dpot=0;
  **wpot=0;
  strips=0;	
  bulk=1;		
  mipcharge=0;
}
//////////////////////////////////////////////////////////////////////
Potentials::Potentials(int dimy, int dimx)	// constructor 
{

  multig=(int)((log10(dimy*dimx/3)/log(4)))+1;
  ref=0;
  XMAX=dimx;
  YMAX=dimy;
  finest=dimx;
  pitch=0;
  width=0;
  count=0;
  poissonf=0;
  strips=0;
  bulk=1;
  mipcharge=75*YMAX; 

  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];
  
  fix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) fix[i] = new int[XMAX];
  
  for(int i=0; i<XMAX; i++) {			// reset potentials to zero as a precaution
    for(int j=0; j<YMAX; j++) {
      dpot[j][i]=0.0;
      wpot[j][i]=0.0;
      fix[j][i]=0;
    }
  }



}
//////////////////////////////////////////////////////////////////////
Potentials::Potentials(int dimy, int dimx, double p, double w) // constructor 
{
  SetPitchWidthXY(dimy, dimx,p,w);
  return;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetwPotential(int posy, int posx, double val) {
  wpot[posy][posx]=val;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetdPotential(int posy, int posx, double val) {
  dpot[posy][posx]=val;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetV(double bias, double depl) {
  vbias=bias;
  vdepl=depl;
}

/////////////////////////////////////////////////////////////////////
int Potentials::GetXMAX() {
  return XMAX;
}
//////////////////////////////////////////////////////////////////////
int Potentials::GetYMAX() {
  return YMAX;
}

///////////////////////////////////////////////////////////////////////
double Potentials::Getdopdensity(int posy) {
  return dopdensity[posy]; 
}
///////////////////////////////////////////////////////////////////////
double Potentials::Getdpot(int posy, int posx) {
  return dpot[posy][posx];
}
///////////////////////////////////////////////////////////////////////
double Potentials::Getwpot(int posy, int posx) {
  return wpot[posy][posx];
}
//////////////////////////////////////////////////////////////////////
void Potentials::Setmipcharge(int ch) {
  mipcharge=ch;
  return;
}
//////////////////////////////////////////////////////////////////////
int Potentials::Getmipcharge() {
  return mipcharge;
}
/////////////////////////////////////////////////////////////////////
int Potentials::Getfix(int posy, int posx) {
  return fix[posy][posx];
}
//////////////////////////////////////////////////////////////////////
int Potentials::Getref() {
  return ref;
}
//////////////////////////////////////////////////////////////////////
double Potentials::Getvbias() {
  return vbias;
}
//////////////////////////////////////////////////////////////////////
double Potentials::Getpitch() {
  return pitch;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetDoping(unsigned char s, unsigned char b) {
  strips=s;
  bulk=b;	
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetPitchWidthXY(int dimy, int dimx,double p, double w)
{

  ref=0;
  pitch=p;
  width=w;
  XMAX=dimx*p;
  YMAX=dimy;
  multig=(int)((log10(YMAX*XMAX/3)/log(4)))+1;   // determine the number of multigrids used
  
  if((int)pitch%2==0) pitch++;
  if((int)width%2==0) width++;      
  if(YMAX%2==0) YMAX--;
  if(XMAX%2==0) XMAX--;
  
  count = (double)XMAX/pitch;    //number of strips
  
    
  if(count-(int)count) {
    count=(int)count+1; 
  }
  
  if (((int)count)%2==0) count++;		
  

  int tempxmax=XMAX;
  int tempymax=YMAX;
  // cout<<"1: tempxmax= "<<tempxmax<<", tempymax= "<<tempymax<<endl;
  for(int i=0; i<multig; i++) { 		//calculates finer grid
    tempxmax=tempxmax/2+1;	
    if((tempxmax)%2==0) XMAX=XMAX+pow(2.0,(i+1));
    tempymax=tempymax/2+1;
    if((tempymax)%2==0) YMAX=YMAX+pow(2.0,(i+1)*1.);
  }

  //cout<<"2: tempxmax= "<<tempxmax<<", tempymax= "<<tempymax<<endl;


  finest=XMAX;


  //  cout << "Actual dimensions: X = " << XMAX<< ", Y =  " << YMAX << endl;


  //mipcharge=75*dimy;    // prima era mipcharge=75*YMAX;
  mipcharge=75*YMAX;

  dopdensity = new double[YMAX];
  for (int i = 0; i < YMAX; ++i) dopdensity[i] = 0;
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];
  
  fix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) fix[i] = new int[XMAX];
  
  for(int i=0; i<XMAX; i++) {
    for(int j=0; j<YMAX; j++) {
      wpot[j][i]=0.0;
      dpot[j][i]=0.0;
      fix[j][i]=0;
    }
  }



}
//////////////////////////////////////////////////////////////////////
void Potentials::SetBoundaryConditions( bool ReadOutTopFlag)   // reset electrodes and potentials according to user input
{			  
  for (int i=0; i<XMAX; i++)
    {	// bottom electrode (backplane): set to Vbias for p-type strips, otherwise 0	
      dpot[0][i]=( strips==PTYPE) ? vbias : 0.0;
      wpot[0][i]=(ReadOutTopFlag)? 0.0: 1.0;										             

      fix[0][i]=1;             
    }
  // 
  for(int j=0;j<(int)count;j++)	// set strips
    {
      for(int k=0;k<width;k++)
	{
	  dpot[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2 + j*(int)pitch+(int)pitch/2-(int)width/2+k]=(strips==NTYPE) ? vbias : 0.0;
	  wpot[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2 + j*(int)pitch+(int)pitch/2-(int)width/2+k]=0.0;			
	  
	  if(j==((int)count/2))
	    {
	      fix[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=2;
	      if (ReadOutTopFlag)										            
		wpot[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=1.0;
	      else
		wpot[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=0.0;
	    }
	  else
	    {
	      fix[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=1;			
	    }
	}
    }
  /*
    if ( (int) count ==1)
      {
	cout << "One single strip" << endl;
	for(int k=10;k<20;k++)
	  {
	    if ((int)(XMAX-(int)pitch/2) + (int)width/2+k < XMAX)
	      dpot[YMAX-1][(int)(XMAX-(int)pitch/2) + (int)width/2+k]=(strips==NTYPE) ? vbias : 0.0;
	    cout << (int)(XMAX/2)<< " "  << (int)width/2 << " " << k << endl;
	  }
      }

    */
}
//////////////////////////////////////////////////////////////////////
void Potentials::Iteration(void *wfgui)	// 
{				// method for iterative calculation, see: http://en.wikipedia.org/wiki/Relaxation_%28iterative_method%29
  //int r=1;
  double sum=0;		// [(old potential) - (new potential)]^2/(XMAX*YMAX)
  double err=0.001;
  long it=0;	// iteration index 
  int N=1;		// every N iterations do plot update
  double DJCoef = 0;
  
  double poissondens = 0;
  double ehratio = 0.;
  double DJZero = 0.;
  
  int **tempfix;			// allocate memory for fix matrix for current grid
  tempfix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) tempfix[i] = new int[XMAX];
  for (int y=1; y<(YMAX-1); y++) dopdensity[y] = 0;
  
  //tempfix=FixRestriktor();	// determine fix matrix for current grid
  FixRestriktor(tempfix);
  
  WFGUI* gui= (WFGUI*) wfgui; 
  
  if(XMAX==finest) N=1000;		// at the finest grid, do plot updates every 600th interation
  else {
    if(XMAX*2-1==finest) N=700;	// at the first coarser grid, do plot updates every 400th interation
    else N=100;				// else, every 100th iteration
  }

  float YMIN =1;
  float YMAXVD =YMAX;

  if (vdepl>vbias && !gui->GetDJOn() && ( ( bulk==PTYPE && strips == NTYPE ) || ( bulk==NTYPE && strips == PTYPE ))) YMIN = YMAX-YMAX*sqrt(vbias)/sqrt(vdepl);
  if (vdepl>vbias && !gui->GetDJOn() &&  ( ( bulk==PTYPE && strips == PTYPE ) || ( bulk==NTYPE && strips == NTYPE ))) YMAXVD = YMAX*sqrt(vbias)/sqrt(vdepl);

  //   if (vdepl>vbias) YMIN = YMAX-YMAX*sqrt(vbias)/sqrt(vdepl);
  poissonf=((bulk==PTYPE) ? +1.0 : -1.0)*(2.0*vdepl)/(YMAX*YMAX);	 // right side of Poisson equation => rho =  eN/epsilon
  poissondens=((strips==NTYPE) ? +1.0 : -1.0)*(2.0*30)/(YMAX*YMAX);	 // 300 micron, Vdepl = 30 V ==> Density for 10 kOhm*cm ==>P-doping= 4.5*10^11 n/cm3

  //  cout << poissondens << " dens " << YMAX << endl;


  
  while(1)		// calculate until accuracy is reached
    {
      if(gui->Getstopped()==1) {
	break;
      }
      
      if(XMAX==finest || XMAX==finest/2+1) err=0.005; // if calculating on finest grid, set accuracy to 0.001
      
      //      if (gui->GetDJOn())
      //	err *=5;

      Potentials newpot(YMAX,XMAX);	// create temporary new object newpot, used for iterative calculation	   
      // inside the grid - use the given equations to average around neighbors

      if (gui->GetDJOn())
	{
	  DJCoef = gui->GetDJValue();
	  ehratio = gui->GetDJehValue();
	  DJZero = 0.5;
	  //	  cout << "DJ ON" << DJCoef << " CallgetDJ= " << gui->CallGetDJ() << endl;
	}	  
      for (int y=1; y<(YMAX-1); y++)
	{

	  if  (!gui->GetDJOn())   dopdensity[y] = -poissonf;
	  else if (gui->CallGetDJType() == 0 && gui->GetDJOn())
	    {
	      if (y< DJZero*YMAX) dopdensity[y] = 10./2.1*DJCoef*(poissondens*DJZero-poissondens*y/YMAX)-poissonf;// h density
	      else	    dopdensity[y] = ehratio*10./2.1*DJCoef*(poissondens*DJZero-poissondens*y/YMAX)-poissonf; // e density
	    }
	  else
	    {
	      if (y<(DJZero-0.1)*YMAX)  dopdensity[y] = -1./2.1*DJCoef*poissondens*cbrt(1.*y- DJZero*YMAX)-poissonf;
	      else if (y>(DJZero-0.1)*YMAX && y<YMAX*(DJZero+0.1)) dopdensity[y] =0.;
	      else   dopdensity[y] = -1./2.1*ehratio*DJCoef*poissondens*cbrt(1.*y- (DJZero+0.1)*YMAX)-poissonf;
	    }
	      //	    cout << " y = " << y << " " << dopdensity[y]<< endl;
	}
      //      cout << gui->CallGetDoping0() << endl;
      
      for (int x=0; x<XMAX; x++)
	{		
	  
	  //	  for (int y=1; y<(YMAX-1); y++)
	  for (int y=YMIN; y<(YMAXVD-1); y++)
	    {
	      //	      cout << poissonf*y/YMAX << endl;
	      if (!tempfix[y][x])
		{
		  // weighting potential
		  newpot.wpot[y][x]=0.25*(wpot[y+1][x]+wpot[y-1][x]+wpot[y][(x+1+XMAX)%(XMAX)]+wpot[y][(x-1+XMAX)%(XMAX)]);
		  // drift potential
		  if (gui->CallGetDetType() == 0 || gui->CallGetDetType() == 2)
		    {
		      newpot.dpot[y][x]=0.25*(dpot[y+1][x]+dpot[y-1][x]+dpot[y][(x+1+XMAX)%(XMAX)]+dpot[y][(x-1+XMAX)%(XMAX)]+dopdensity[y]);
		    }
		  else
		    {
		      newpot.dpot[y][x]=0.25*(dpot[y+1][x]+dpot[y-1][x]+dpot[y][(x+1+XMAX)%(XMAX)]+dpot[y][(x-1+XMAX)%(XMAX)]);
		      
		    }
		}
	      else
		{
		  newpot.wpot[y][x]=wpot[y][x];		// weighting potential
		  newpot.dpot[y][x]=dpot[y][x];		// drift potential 
		}
	    }

	  // special treatment for top and bottom rows (potential above/below is replaced by cell itself)
	  if(tempfix[0][x])
	    {
	      newpot.wpot[0][x]=wpot[0][x];			// weighting potential
	      newpot.dpot[0][x]=dpot[0][x];
	    }
	  
	  if (tempfix[YMAX-1][x]==0)
	    {
	      newpot.wpot[YMAX-1][x]=0.25*(wpot[YMAX-1][(x-1+XMAX)%(XMAX)]+wpot[YMAX-1][(x+1+XMAX)%(XMAX)]+wpot[YMAX-2][x]+wpot[YMAX-1][x]);			 // weighting potential
	      newpot.dpot[YMAX-1][x]=0.25*(dpot[YMAX-1][(x-1+XMAX)%(XMAX)]+dpot[YMAX-1][(x+1+XMAX)%(XMAX)]+dpot[YMAX-2][x]+dpot[YMAX-1][x]-dopdensity[YMAX-1]);   // drift potential 
	    }
	  
	  if(tempfix[YMAX-1][x])
	    {
	      newpot.wpot[YMAX-1][x]=wpot[YMAX-1][x];	// weighting potential	
	      newpot.dpot[YMAX-1][x]=dpot[YMAX-1][x];	// drift potential 
	    } 
	}
      if(it%100==0) // check every 100 iterations wether break condition fulfilled
	{
	  sum=0.0;
	  for(int i=0; i<XMAX; i++) {		// calculate sum
	    for(int j=0; j<YMAX; j++) {
	      sum+=fabs(dpot[j][i]-newpot.dpot[j][i]);
	    }
	  }
	  if(it%N==0 && gui->Getplotupdate()==1 && gui->Getstopped()==0)	// do plot update every N iterations
	    {
	      //gui->DrawHist();
	      gui->Getdhist()->Reset();	//Reset histogram. dhist1 = histogram of drift potential
	      //      gui->Getdhist()->TH2F::SetBins((XMAX),-(XMAX)*0.5,(XMAX)*0.5,(YMAX),-(YMAX)*0.5,(YMAX)*0.5); // set bins of histogram
	      gui->Getdhist()->TH2F::SetBins((XMAX),0.,XMAX,YMAX,0,YMAX); // set bins of histogram
	      gui->Getdhist()->GetXaxis()->SetTitle("x [um]");
	      gui->Getdhist()->GetYaxis()->SetTitle("y [um]");
	      
	      gui->Getwhist()->Reset();	//whist1 = histogram of weighting potential					
	      gui->Getwhist()->TH2F::SetBins(XMAX,0,XMAX,YMAX, 0,YMAX);
	      
	      for(int i=0;i<XMAX;i++) {		// fill histogram
		for(int j=0;j<YMAX;j++) {
		  gui->Getdhist()->SetBinContent(i+1,j+1,newpot.dpot[j][i]);
		  gui->Getwhist()->SetBinContent(i+1,j+1,newpot.wpot[j][i]);
		}
	      }	
	      
	      cout << " Potentials 2DPlot " << gui->GetLess2DPlot() << endl;
	      gui->DrawAllGraph(0);
	      
	      /*	      if (!gui->GetLess2DPlot()) 
			      {
			      gui->Getcanvasp()->cd();
			      gui->Getdhist()->SetStats(0);	// hide statistics box
			      gui->Getdhist()->Draw("COLZ"); // draw histogram				
			      
			      
			      gui->Getcanvasw()->cd();
			      gui->Getwhist()->SetStats(0);
			      gui->Getwhist()->Draw("COLZ");
			      
			      DriftPal();
			      gui->Getcanvasp()->Update();
			      WeightPal();
			      gui->Getcanvasw()->Update();
			      }
	      */
	      
	    }//End of if(it%N==0)
	  
	  if(sum/((double)(XMAX*YMAX))<err)	//check wether accuracy reached
	    {
	      for(int i=0;i<YMAX;i++) {	// overwrite potential with newpotential
		for(int j=0; j<XMAX;j++) {
		  dpot[i][j]=newpot.dpot[i][j];		  
		  wpot[i][j]=newpot.wpot[i][j];
		}
	      }
	      
	   
	      break;	//break out of while loop if accuracy reached 
	    }
	}//end of if(it%100)
      
      for(int i=0;i<YMAX;i++) {	// overwrite potential with newpotential	
	for(int j=0; j<XMAX;j++) {
	  dpot[i][j]=newpot.dpot[i][j];
	  wpot[i][j]=newpot.wpot[i][j];
	}
      }
      it++;
      
      ///} // end of if(gui-<Getstopped()==0)
    }//end of while loop

//	for (int j = 0; j < YMAX; j++) delete [] tempfix[j] ;	    	
//	delete [] tempfix;
}
//////////////////////////////////////////////////////////////////////
Potentials::~Potentials() {	// destructor
  for (int j = 0; j < YMAX; j++) delete[] dpot[j] ;	    	
  delete[] dpot;
  
  for (int j = 0; j < YMAX; j++) delete[] wpot[j] ;
  delete[] wpot;
  
  for (int j = 0; j < YMAX; j++) delete[] fix[j] ;
  delete[] fix;
  
  //delete[] dopdensity;
}
//////////////////////////////////////////////////////////////////////
void Potentials::DeleteDopdensity(){
  delete[] dopdensity;
}
//////////////////////////////////////////////////////////////////////
void Potentials::Restriktor()    //method to restrict potentials to a coarser grid, with XMAX/2+1
{
  int z=0,s=0;	// z: lines, s: columns
  ref=ref+1;	// increase ref 
  Potentials newpot(YMAX/2+1,XMAX/2+1); // temporary new object newpot on coarser grid
  
  for(int i=0;i<YMAX/2+1;i++)	// copy mutual points from 'old' potential to new potential
    {
      s=0;			
      for(int j=0; j<XMAX/2+1;j++)
	{
	  newpot.dpot[i][j]=dpot[z][s];
	  newpot.wpot[i][j]=wpot[z][s];
	  s=s+2;
	}
      z=z+2;
    }
  
  XMAX=XMAX/2+1;		// set XMAX and YMAX to new values
  YMAX=YMAX/2+1;		
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];
  
  for(int i=0;i<YMAX;i++)	 // overwriting dpot and wpot with dpot and wpot of new potential
    {		
      for(int j=0; j<XMAX;j++)
	{
	  dpot[i][j]=newpot.dpot[i][j];
	  wpot[i][j]=newpot.wpot[i][j];
	}
    }	
}
//////////////////////////////////////////////////////////////////////
//int** Potentials::FixRestriktor()  
void Potentials::FixRestriktor(int** nfix)
{
  int z=0,s=0;   // lines and columns counter 
//  int **nfix=0;
  
//  nfix = new int*[YMAX];	// allocate memory for restricted fix matrix
//  for (int i = 0; i<YMAX; i++) nfix[i] = new int[XMAX];
  
  for(int i=0;i<YMAX;i++)	// copy mutual points from old fix matrix to nfix
    {
      s=0;			
      for(int j=0; j<XMAX;j++)
	{
	  nfix[i][j]=fix[z][s];	
	  s=s+(int)pow(2.0,ref*1.);
	}
      z=z+(int)pow(2.0,ref*1.);
    }		
  return;
}
//////////////////////////////////////////////////////////////////////
void Potentials::Prolongation(bool ReadOutTopFlag) // method to prolongate potentials to finer grid with XMAX*2-1
{			
  ref=ref-1;	//decrease ref (going from coarser to finer grid)											
  int z=0,s=0;	// columns and lines counter for loop
  YMAX=YMAX*2-1;	// setting YMAX and XMAX
  XMAX=XMAX*2-1;
  Potentials newpot(YMAX,XMAX);	// temporary Potentials object
  
  int **tempfix=0;	// allocate memory for temporary fix matrix 
  tempfix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) tempfix[i] = new int[XMAX];
  
//   tempfix=FixRestriktor();	// set fix matrix
  FixRestriktor(tempfix);
  
  for(int i=0;i<YMAX;i=i+2)    // copy mutual points from coarser grid to finer grid
    {		
      s=0;	
      for(int j=0; j<XMAX;j=j+2)
	{
	  newpot.dpot[i][j]=dpot[z][s];
	  newpot.wpot[i][j]=wpot[z][s];	
	  s++;
	}
      z++;
    }
  
  for(int i=1;i<YMAX;i=i+2)	// calculate mean value for grid points without 4 neighbours
    {							
      for(int j=1;j<XMAX;j=j+2)
	{
	  newpot.dpot[i][j]=0.25*(newpot.dpot[i-1][j-1]+newpot.dpot[i-1][j+1]+newpot.dpot[i+1][j+1]+newpot.dpot[i+1][j-1]);
	  newpot.wpot[i][j]=0.25*(newpot.wpot[i-1][j-1]+newpot.wpot[i-1][j+1]+newpot.wpot[i+1][j+1]+newpot.wpot[i+1][j-1]);
	}
    }
  // set backplane to potential
  for (int j=0; j<XMAX; j++)
    {
      newpot.dpot[0][j]=(strips==PTYPE) ? vbias : 0.0; // bottom electrode (backplane): set to Vbias for p-type strips, otherwise 
      newpot.wpot[0][j]=(ReadOutTopFlag)? 0.0: 1.0;										             
    } 
  int l=0,f=0;  // auxiliary variable to calculate boundary points	
  for(int i=1;i<YMAX;i++)	// average over neighbours of grid points which are not set yet
    {						
      for(int j=(i+1)%2;j<XMAX;j=j+2)  //// depending on line (i), counter j starts at 0 or 1				
	{							
	  if(tempfix[i][j]==1) // fix jenes gitters auf das erweitert wird
	    {
	      newpot.dpot[i][j]=(strips==NTYPE) ? vbias : 0.0;	
	      newpot.wpot[i][j]=(ReadOutTopFlag)? 0.0: 0.0;										             
	    }
	  
	  if(tempfix[i][j]==2)
	    {
	      newpot.dpot[i][j]=(strips==NTYPE) ? vbias : 0.0;
	      newpot.wpot[i][j]=(ReadOutTopFlag)? 1.0: 0.0;										             

	    }
	  
	  if(tempfix[i][j]==0)
	    {
	      f=0;	//special treatment for bottom and top (potentials above/below is replaced by cell itself)	
	      l=0;							
	      
	      if(i==YMAX-1) f=1;
	      if(i==0) l=1;
	      if (XMAX != 1)
          {
              newpot.dpot[i][j]=0.25*(newpot.dpot[i][(j-2+XMAX) % (XMAX-1)]+newpot.dpot[i+1-f][j]+newpot.dpot[i-1+l][j]+newpot.dpot[i][(j+XMAX)%(XMAX-1)]);
	      newpot.wpot[i][j]=0.25*(newpot.wpot[i][(j-2+XMAX)% (XMAX-1)]+newpot.wpot[i+1-f][j]+newpot.wpot[i-1+l][j]+newpot.wpot[i][(j+XMAX)%(XMAX-1)]);
          }
          }
	}
    }
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];
  
  for(int i=0;i<YMAX;i++) {			// overwriting old dpot and wpot with new ones
    for(int j=0;j<XMAX;j++) {
      wpot[i][j]=newpot.wpot[i][j];
      dpot[i][j]=newpot.dpot[i][j];
    }
  }

 // delete[] tempfix;
  for (int i = 0; i < YMAX; ++i) delete[] tempfix[i];
  delete[] tempfix;
 
}
//////////////////////////////////////////////////////////////////////////
void Potentials::Multigrid(void *wfgui, bool ReadOutTopFlag)
{
  WFGUI* gui= (WFGUI*) wfgui;
  
  for(int i=0; i<multig; i++) { // starting with calculation on coarsest grid, we need to restrict potentials
    Restriktor();
  }
  
  char * gridlabel;
  gridlabel = new char[50];
  sprintf(gridlabel, "Calculating Potentials: grid number: %d/%d",1,multig+1);
  
  char * gridlabel2;
  gridlabel2 = new char[50];
  sprintf(gridlabel2, "Calculating Potentials: grid number: %d/%d",1,multig+1);
  

  Iteration(gui);
  
  gui->GetCalculatingLabel2()->SetTitle(gridlabel2);
  gui->GetCalculatingLabel()->SetTitle(gridlabel);
  
  for(int i=0; i<multig; i++) { 
    Prolongation(ReadOutTopFlag);
    sprintf(gridlabel, "Calculating Potentials: grid number: %d/%d",i+2,multig+1);
    sprintf(gridlabel2, "Calculating Potentials: grid number: %d/%d",i+2,multig+1);	
    gui->GetCalculatingLabel2()->SetTitle(gridlabel2);
    gui->GetCalculatingLabel()->SetTitle(gridlabel);
    Iteration(gui);
  }
  
  gui->Getdhist()->GetXaxis()->SetLabelColor(1);	
  gui->Getwhist()->GetYaxis()->SetLabelColor(1);

  delete [] gridlabel;
  delete [] gridlabel2;
  
}
//////////////////////////////////////////////////////////////////////////
unsigned char Potentials::GetDoping() {
  return strips;
}
//////////////////////////////////////////////////////////////////////////
void Potentials::DriftPal()
{
  static Int_t  colors[999];
  static Bool_t initialized = kFALSE;
  gStyle->SetNumberContours(999);
  
  Double_t r[]    = {0.0, 0.0, 0.0, 1.0, 1.0}; //0.54 1 1 0 0 0
  Double_t g[]    = {0.0, 1.0, 1.0, 1.0, 0.0}; //0.17 0 1 1 1 0
  Double_t b[]    = {1.0, 1.0, 0.0, 0.0, 0.0}; //0.89 0 0 0 1 1
  Double_t stop[] = {0.0, 0.25, 0.50,.75,1.0};
  
  if(!initialized)
    {
      Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 999);
      for(int i=0;i<999;i++) colors[i] = FI+i;
      initialized = kTRUE;
      return;
    }
  gStyle->SetPalette(999,colors);
}
//////////////////////////////////////////////////////////////////////////
void Potentials::WeightPal()
{
  static Int_t  colors[999];
  static Bool_t initialized = kFALSE;
  gStyle->SetNumberContours(999);
  
  Double_t r[]    = {0.0, 0.0, 0.0, 1.0, 1.0}; //0.54 1 1 0 0 0
  Double_t g[]    = {0.0, 1.0, 1.0, 1.0, 0.0}; //0.17 0 1 1 1 0
  Double_t b[]    = {1.0, 1.0, 0.0, 0.0, 0.0}; //0.89 0 0 0 1 1
  Double_t stop[] = {0.0, 0.02, 0.20, .30, 1.0};
  
  if(!initialized)
    {
      Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 999);
      for(int i=0;i<999;i++) colors[i] = FI+i;
      initialized = kTRUE;
      return;
    }
  gStyle->SetPalette(999,colors);
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetAbove(bool above) {
  alpha_above=above;
}
//////////////////////////////////////////////////////////////////////////////////////

bool Potentials::GetAbove() {
  return alpha_above;
}
