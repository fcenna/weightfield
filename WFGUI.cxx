#include "WFGUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


///-----------------------------------------------------\\ VARIABLES INITIAL VALUES //-------------------------------------------------------------///

std::map<std::string, double> GetParameters(const char* ifname = "/parameters/parameters.dat")
{
	std::map<std::string, double> valueMap;

	// prepare map

	//#########################	
	//#--Detector Properties--#
	//#########################
	//--Dimensions--
	valueMap["STRIP_NUMB"] = 3.;
	valueMap["DETECT_HEIGHT"] = 200.;
	valueMap["STR_PITCH"] = 300.;
	valueMap["STR_WIDTH"] = 200.;
	valueMap["GAIN_SCL"] = 1.;
	valueMap["HE_GAIN_RAT"] = 0.;
	valueMap["GAIN_LYR_RSS"] = 10.;
	valueMap["YPOSITION"] = 100.;
	
	valueMap["IRRADIATION"] = 1.;
	valueMap["DOUBLEJUNCTION"] = 1.;
	valueMap["NA_OVER_ND"] = 0.5;

	//--Voltage--
	valueMap["BIAS_VOLTAGE"] = 790.;
	valueMap["DEPL_VOLTAGE"] = 50.;
 
	//--Electronics--
	valueMap["CAPACITANCE"] = 2.;
	valueMap["INDUCTANCE"] = 2.;
	valueMap["IMPEDANCE"] = 50.;
	valueMap["OSCOPE_BW"] = 2.;
	valueMap["SHPR_INT_TIME"] = 3.5; 
	valueMap["SHPR_DCY_TIME"] = 5.;
	valueMap["SHPR_TRANS"] = 4.;
	valueMap["SHPR_NOISE"] = 1.;
	valueMap["VTH"] = 10.;
	valueMap["BBBW"] = 2.5;
	valueMap["BBGAIN"] = 100.;
	valueMap["BBVTH"] = 10.;
	valueMap["BB_NOISE"] = 2.;
	valueMap["BB_IMP"] = 50.;
	
	//#############
	//#--Control--#
	//#############
	valueMap["PRECISION"] = 1.;
	valueMap["SAMPLING"] = 100.;

	//--Select Particles--
	valueMap["SET_RANGE"] = 10.;
	valueMap["CALIB"] = 5.;
	valueMap["USERQ"] = 73;
	valueMap["NUMBERP"] = 1;
    
    //--Irradiation
	valueMap["IRRADIATION"] = 0.;

	//--Currents--
	valueMap["TEMPERATURE"] = 300.;

	cout<< "Processing parameters from file " << ifname <<endl;

	std::ifstream ifile(ifname);
	if (!ifile) {
		cout<< "\n***WARNING: Could not open parameter file " << ifname <<endl;
		cout<< "using DEFAULT values\n" <<endl;
		return valueMap;
	}

	std::string line;

	while (getline(ifile,line)) {
        std::stringstream ss(line);
		std::string key = "";
		ss >> key;
		if (key.size() == 0) continue;// this line is empty
		if (key.c_str()[0] == '#') continue;// this is a comment
        std::map<std::string,double>::iterator found = valueMap.find(key);
		if (found != valueMap.end()) {
            ss >> found->second;
		}
	}

	return valueMap;
}

///---------------------------------------------------------------------------------------------------------------------------------------------///


///------------------------------------------------------------\\ GUI SETUP //--------------------------------------------------------------------///

WFGUI::WFGUI(const TGWindow *p,UInt_t w,UInt_t h): TGMainFrame(p,w,h), dwpot(Potentials(300,1,200,30)) {

    //take intial varible names from parameters file
	const char* ifname = "parameters/parameters.dat";
	std::map<std::string, double> valueMap = GetParameters(ifname);

	// mainframes initial settings
	radiobuttonstatus=MIPunif;//radio button mip set as default
	BatchOn=false;
	fileName = "";
	Connect("CloseWindow()", "WFGUI", this, "CloseWindow()");// connect exit-button of window to CloseWindow()

	SetLayoutManager(new TGHorizontalLayout(this));
	
    ///**************************************************** GRAPHS ********************************************************///
    
	PotentialTab = new TGTab(this,600,220);//General Tab

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////DRIFT POTENTIAL TAB//////////////////////////////////////////////////////////////////////
   
    DriftPTab = PotentialTab->AddTab("Drift Potential");
    DriftPTab->SetLayoutManager(new TGVerticalLayout(DriftPTab));
    
    /////////CALCULATING LABEL/////////////
    //  CalculatingLabel = new TGLabel(DriftPTab, new TGString("First press 'Set' then 'Potential' and finally 'Currents' "));
    // DriftPTab->AddFrame(CalculatingLabel, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////DRIFT CANVAS/////////////
    driftcanvas = new TRootEmbeddedCanvas("DriftCanvas",DriftPTab,600,140); //canvas for drift potential
    DriftPTab->AddFrame(driftcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
    ////////////////////////////////////////////
    
    DriftPTabFrame = new TGHorizontalFrame(DriftPTab,600,400);
    
    ////////////////////////////////////////////
    /////////DRIFT CANVAS/////////////
    driftpcanvas = new TRootEmbeddedCanvas("DriftPCanvas",DriftPTabFrame,600,380); // canvas for cut potential
    DriftPTabFrame->AddFrame(driftpcanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    DriftPTab->AddFrame(DriftPTabFrame,new TGLayoutHints( kLHintsBottom | kLHintsExpandX,1,1,1,1));
    
    ////////////////////////////////////////////
    /////////LABELS TAB FRAME/////////////
    //create frames
    LabelsTabFrame = new TGHorizontalFrame(DriftPTab, 600,20);
    
    //create entries
    DriftCutLabel = new TGLabel(LabelsTabFrame, new TGString("Plotting at:"));
    OnStripsButton = new TGTextButton(LabelsTabFrame,new TGHotString("On Strips"));
    BetweenStripsButton = new TGTextButton(LabelsTabFrame,new TGHotString("Between Strips"));
    WhereCut = new TGNumberEntry(LabelsTabFrame, (Double_t) 0,8,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax);
    DrawCutsUserEntry = new TGTextButton(LabelsTabFrame, new TGHotString("Draw"));
    FieldLabel = new TGLabel(LabelsTabFrame, new TGString("Field:"));
    ExButton = new TGTextButton(LabelsTabFrame,new TGHotString("Ex"));
    EyButton = new TGTextButton(LabelsTabFrame,new TGHotString("Ey"));

    //develop entries
    OnStripsButton->SetTextColor(0xff0000);
    OnStripsButton->Connect("Clicked()", "WFGUI", this, "SetCutOnStrips()");
    BetweenStripsButton->Connect("Clicked()", "WFGUI", this, "SetCutBetweenStrips()");
    BetweenStripsButton->SetTextColor(0x0000ff);
    DrawCutsUserEntry->Connect("Clicked()", "WFGUI", this, "DrawAll()");
    ExButton->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsX()");
    EyButton->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsY()");
    OnStripsButton->SetEnabled(kFALSE);
    BetweenStripsButton->SetEnabled(kFALSE);
    ExButton->SetEnabled(kFALSE);
    EyButton->SetEnabled(kFALSE);
    DrawCutsUserEntry->SetEnabled(kFALSE);
    
    //add entries
    LabelsTabFrame->AddFrame(DriftCutLabel, new TGLayoutHints(kLHintsLeft,1,1,5,1));
    LabelsTabFrame->AddFrame(OnStripsButton, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(BetweenStripsButton, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(WhereCut, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(DrawCutsUserEntry, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LabelsTabFrame->AddFrame(ExButton, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame->AddFrame(EyButton, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame->AddFrame(FieldLabel, new TGLayoutHints(kLHintsRight,30,1,5,1));

    //add frames
    DriftPTab->AddFrame(LabelsTabFrame, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //////END OF DRIFT TAB/////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
  
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////WEIGHTING POTENTIAL TAB//////////////////////////////////////////////////////////////////
   
    WeightingPTab = PotentialTab->AddTab("Weighting Potential");
    WeightingPTab->SetLayoutManager(new TGVerticalLayout(WeightingPTab));

    ////////////////////////////////////////////
    /////////CALCULATING LABEL/////////////
    CalculatingLabel2 = new TGLabel(WeightingPTab, new TGString(" Press Button 'Calculate' to calculate Potentials and Fields"));
    // WeightingPTab->AddFrame(CalculatingLabel2, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    /////////WEIGHTING CANVAS/////////////
    weightcanvas = new TRootEmbeddedCanvas("weightCanvas",WeightingPTab,600,220); // canvas for cut potential
    WeightingPTab->AddFrame(weightcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
    ////////////////////////////////////////////

    WeightPTabFrame = new TGHorizontalFrame(WeightingPTab,600,400);
    
    ////////////////////////////////////////////
    /////////LABELS TAB FRAME/////////////
    //create frames
    LabelsTabFrame2 = new TGHorizontalFrame(WeightingPTab, 600,20);
    
    //create entries
    WeightingCutLabel = new TGLabel(LabelsTabFrame2, new TGString("Plotting at:"));
    OnStripsButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("On Strips"));
    BetweenStripsButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("Between Strips"));
    WhereCut2 = new TGNumberEntry(LabelsTabFrame2, (Double_t) 0,8,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax);
    DrawCutsUserEntry2 = new TGTextButton(LabelsTabFrame2, new TGHotString("Draw"));
    FieldLabel2 = new TGLabel(LabelsTabFrame2, new TGString("Field:"));
    ExButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("|Ex|"));
    EyButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("|Ey|"));
    
    //develop entries
    OnStripsButton2->SetTextColor(0xff0000);
    OnStripsButton2->Connect("Clicked()", "WFGUI", this, "SetCutOnStrips()");
    BetweenStripsButton2->SetTextColor(0x0000ff);
    BetweenStripsButton2->Connect("Clicked()", "WFGUI", this, "SetCutBetweenStrips()");
    DrawCutsUserEntry2->Connect("Clicked()", "WFGUI", this, "DrawAllw()");
    ExButton2->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsX()");
    EyButton2->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsY()");
    OnStripsButton2->SetEnabled(kFALSE);
    BetweenStripsButton2->SetEnabled(kFALSE);
    ExButton2->SetEnabled(kFALSE);
    EyButton2->SetEnabled(kFALSE);
    DrawCutsUserEntry2->SetEnabled(kFALSE);
    
    //add entries
    LabelsTabFrame2->AddFrame(WeightingCutLabel, new TGLayoutHints(kLHintsLeft,1,1,5,1));
    LabelsTabFrame2->AddFrame(OnStripsButton2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(BetweenStripsButton2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(WhereCut2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(DrawCutsUserEntry2, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LabelsTabFrame2->AddFrame(ExButton2, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame2->AddFrame(EyButton2, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame2->AddFrame(FieldLabel2, new TGLayoutHints(kLHintsRight,30,1,5,1));
    
    //add frames
    WeightingPTab->AddFrame(LabelsTabFrame2, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    /////////WEIGHTING CANVAS/////////////
    weightpcanvas = new TRootEmbeddedCanvas("weightPCanvas",WeightPTabFrame,600,300); // canvas for cut potential
    WeightPTabFrame->AddFrame(weightpcanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    WeightingPTab->AddFrame(WeightPTabFrame, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    
    //////END OF WEIGHTING POTENTIAL TAB///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
	
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////CURRENTS TAB/////////////////////////////////////////////////////////////////////////////
    
    CurrentsTab = PotentialTab->AddTab("Currents and Oscilloscope");
	CurrentsTab->SetLayoutManager(new TGVerticalLayout(CurrentsTab));
    
    ////////////////////////////////////////////
    /////////CURRENTS CANVAS/////////////
	currentscanvas = new TRootEmbeddedCanvas("CurrentsCanvas",CurrentsTab,600,520);	// canvas for graphs of currents
	CurrentsTab->AddFrame(currentscanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));	
    ///////////////////////////////////////////
    
    CurrentsInfoFrame = new TGVerticalFrame(CurrentsTab,100,200);

    ////////////////////////////////////////////
    /////////CURRENTS LABEL FRAME/////////////
    //create frames
    CurrentsLabelFrame = new TGHorizontalFrame(CurrentsInfoFrame,100,50);
    
    //create entries
    ElectronsLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Electrons"));
    GainElectronsLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Gain El."));
    HolesLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Holes"));
    GainHolesLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Gain Holes"));
    TotalLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Total"));
    
    //develop entries
    ElectronsLabel->SetBackgroundColor(0xff0000);
    GainElectronsLabel->SetBackgroundColor(0xff00ff);
    HolesLabel->SetBackgroundColor(0x0066ff);
    GainHolesLabel->SetBackgroundColor(0x00ffff);
    TotalLabel->SetBackgroundColor(0x00ff00);
    
    //add entries
    CurrentsLabelFrame->AddFrame(ElectronsLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(GainElectronsLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(HolesLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(GainHolesLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(TotalLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(CurrentsLabelFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////ELECTRONICS LABEL FRAME/////////////
    //create frames
    ElectronicsLabelFrame = new TGHorizontalFrame(CurrentsInfoFrame,100,50);
    
    //create entries
    RCLabel =  new TGLabel(ElectronicsLabelFrame, new TGString("- - -  Current at CSA input [A]"));
    ScopeLabel =  new TGLabel(ElectronicsLabelFrame, new TGString("- Oscilloscope [mV]"));
    
    //develop entries
    RCLabel->SetBackgroundColor(0xffffff);
    ScopeLabel->SetBackgroundColor(0xffffff);
    
    //add entries
    ElectronicsLabelFrame->AddFrame(RCLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    ElectronicsLabelFrame->AddFrame(ScopeLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(ElectronicsLabelFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////CURRENT PROGRESS BAR FRAME/////////////
    // CurrentsProgressBar = new TGHProgressBar(CurrentsInfoFrame,TGProgressBar::kStandard,500);
    //    CurrentsProgressBar->ShowPosition(kTRUE,kTRUE);
    //CurrentsProgressBar->Percent(0);
    //CurrentsInfoFrame->AddFrame(CurrentsProgressBar,new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
 
    
    ////////////////////////////////////////////
    /////////CHARGE COLLECTION FRAME/////////////
    //create frames
    ChargeCollectionFrame = new TGGroupFrame(CurrentsInfoFrame, "Charge Collection", kHorizontalFrame);
    ChargeLabelFrame1 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeLabelFrame2 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeLabelFrame3 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeNumberFrame1 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);
    ChargeNumberFrame2 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);
    ChargeNumberFrame3 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);

	//create entries
    QETextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("e- charges (e): "));//electrons charge label
    QELabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" h+ charges (e): "));//holes charge label
    QHLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QEHTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" e- + h+ charges (e): "));//gain electrons charge label
    QEHLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    QEGTextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("Gain e- charges (e): "));//gain holes charge label
    QEGLabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHGTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" Gain h+ charges (e): "));//total charge label
    QHGLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QEHGTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" Gain e- + h+ charges (e): "));//gain electrons charge label
    QEHGLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    QETotTextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("Total e- charges (e): "));//gain holes charge label
    QETotLabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHTotTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" Total h+ charges (e): "));//total charge label
    QHTotLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QTotTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" Total Charges (e): "));
    QTotLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    
	//develop entries
    QELabel->SetBackgroundColor(0xffffff);
    QHLabel->SetBackgroundColor(0xffffff);
    QEHLabel->SetBackgroundColor(0xffffff);
    QEGLabel->SetBackgroundColor(0xffffff);
    QHGLabel->SetBackgroundColor(0xffffff);
    QEHGLabel->SetBackgroundColor(0xffffff);
    QETotLabel->SetBackgroundColor(0xffffff);
    QHTotLabel->SetBackgroundColor(0xffffff);
    QTotLabel->SetBackgroundColor(0xffffff);
    QELabel->Resize(QELabel->GetWidth()+220, QELabel->GetHeight());
    
    /*QHLabel->SetBackgroundColor(0xffffff);
    QEHLabel->SetBackgroundColor(0xffffff);
    QEGLabel->SetBackgroundColor(0xffffff);
    QHGLabel->SetBackgroundColor(0xffffff);
    QEHGLabel->SetBackgroundColor(0xffffff);
    QETotLabel->SetBackgroundColor(0xffffff);
    QHTotLabel->SetBackgroundColor(0xffffff);
    QTotLabel->SetBackgroundColor(0xffffff);*/

    //add entries
    ChargeLabelFrame1->AddFrame(QETextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame1->AddFrame(QELabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame2->AddFrame(QHTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame2->AddFrame(QHLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop | kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame3->AddFrame(QEHTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame3->AddFrame(QEHLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame1->AddFrame(QEGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame1->AddFrame(QEGLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame2->AddFrame(QHGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame2->AddFrame(QHGLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame3->AddFrame(QEHGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame3->AddFrame(QEHGLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame1->AddFrame(QETotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX,1,1,1,1));
	ChargeNumberFrame1->AddFrame(QETotLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame2->AddFrame(QHTotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame2->AddFrame(QHTotLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
	ChargeLabelFrame3->AddFrame(QTotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
	ChargeNumberFrame3->AddFrame(QTotLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
    
    //add frames
    ChargeCollectionFrame->AddFrame(ChargeLabelFrame1, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame1, new TGLayoutHints(kLHintsLeft | kLHintsExpandX ,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeLabelFrame2, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame2, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,0,0,0,0));
	ChargeCollectionFrame->AddFrame(ChargeLabelFrame3, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame3, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,0,0,0,0));
	CurrentsInfoFrame->AddFrame(ChargeCollectionFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////LORENTZ FRAME/////////////
    //create frames
	LorentzInfoFrame = new TGGroupFrame(CurrentsInfoFrame, "Lorentz Drift", kHorizontalFrame);
    
    //create entries
	LorentzeTextLabel =  new TGLabel(LorentzInfoFrame, new TGString("e- Lorentz Angle [D]: "));
	LorentzeLabel = new TGLabel(LorentzInfoFrame, new TGString("0"));
    LorentzhTextLabel =  new TGLabel(LorentzInfoFrame, new TGString("h+ Lorentz Angle [D]: "));
    LorentzhLabel = new TGLabel(LorentzInfoFrame, new TGString("0"));

	//develop entries
	LorentzeLabel->SetBackgroundColor(0xffffff);
    LorentzhLabel->SetBackgroundColor(0xffffff);

    //add entries
    LorentzInfoFrame->AddFrame(LorentzeTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
	LorentzInfoFrame->AddFrame(LorentzeLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX,1,1,1,1));
	LorentzInfoFrame->AddFrame(LorentzhTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
	LorentzInfoFrame->AddFrame(LorentzhLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(LorentzInfoFrame,new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

    CurrentsTab->AddFrame(CurrentsInfoFrame,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    
    //////END OF CURRENTS TAB//////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////OSCILLOSCOPE TAB/////////////////////////////////////////////////////////////////////////
    
    OscilloscopeTab = PotentialTab->AddTab("Electronics");
    OscilloscopeTab->SetLayoutManager(new TGVerticalLayout(OscilloscopeTab));
    PotentialTab->SetTab(0);

    ////////////////////////////////////////////
    /////////OSCILLOSCOPE CANVAS/////////////
	oscilloscopecanvas = new TRootEmbeddedCanvas("OscilloscopeCanvas",OscilloscopeTab,600,520);
	OscilloscopeTab->AddFrame(oscilloscopecanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //////END OF OSCILLOSCOPE TAB///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    AddFrame(PotentialTab,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); // Add tabs to mainframe

    ///************************************************* END OF GRAPHS *****************************************************///

    
    
    
	//da leggere
	//http://root.cern.ch/root/html/TGCompositeFrame.html
	//http://root.cern.ch/root/html/TGLayoutManager.html
	//http://www.eclipsezone.com/eclipse/forums/t99010.html
	//http://www.eclipse.org/eclipse/platform-core/documents/3.1/debug.html
    
    
    
    
    ///*************************************************** SETTINGS *******************************************************///
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///START OF SETTINGS FRAME////////////////////////////////////////////////////////////////////////////////////////////////
    
    SettingsGlobalFrame = new TGVerticalFrame(this,500,90);
    SettingsFrame2 = new TGHorizontalFrame(SettingsGlobalFrame, 500, 90);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////START OF CONTROL FRAME///////////////////////////////////////////////////////////////////
   
    ControlFrame = new TGGroupFrame(SettingsFrame2," Control",kVerticalFrame);

    ////////////////////////////////////////////
    /////////RUN FRAME/////////////
    //create frames
    ButtonPotFrame = new TGGroupFrame(ControlFrame,"Run",kHorizontalFrame);
    //
    // place here "run", "done"..

    CalculatingLabel = new TGLabel(ControlFrame, new TGString("Press 'Potential' and then 'Currents' "));
    CalculatingLabel->SetBackgroundColor(0x00ff00);
    ControlFrame->AddFrame(CalculatingLabel, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    //create entries
    SetButton = new TGTextButton(ButtonPotFrame,"Set");
    CalcPotButton = new TGTextButton(ButtonPotFrame, "Potentials");
    CalculateButton = new TGTextButton(ButtonPotFrame,"Currents");
    StopButton = new TGTextButton(ButtonPotFrame," Stop ");
    ExitButton = new TGTextButton(ButtonPotFrame," Exit ");

    //develop entries
	SetButton->Associate(this);
    SetButton->SetTextColor(1,kFALSE);
    SetButton->Connect("Clicked()","WFGUI",this,"CallBoundaryConditions())");//Connect Set Button to CallBoundary Conditions()
    SetButton->SetEnabled(kTRUE);
    CalcPotButton->SetTextColor(1,kFALSE);
    CalcPotButton->Connect("Clicked()","WFGUI",this,"ThreadstartPotential()");//Connect button with threadstart method
    CalcPotButton->SetEnabled(kTRUE);
    CalculateButton->SetTextColor(1, kFALSE);
    CalculateButton->Connect("Clicked()","WFGUI",this,"ThreadstartCurrents()"); // connect calculate button to threadstart
    CalculateButton->SetEnabled(kFALSE);
    StopButton->Associate(this);
    StopButton->SetBackgroundColor(0xff0000);
    StopButton->Connect("Clicked()","WFGUI",this,"SetStopOn()");
    StopButton->SetEnabled(kTRUE);
    ExitButton->Associate(this);
    ExitButton->SetBackgroundColor(0x66cc66);
    ExitButton->SetCommand("gApplication->Terminate(0)");
    ExitButton->SetEnabled(kTRUE);

    //add entries
    ButtonPotFrame->AddFrame(SetButton, new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(CalcPotButton, new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(CalculateButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(StopButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(ExitButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
    
    //add frames
	ControlFrame->AddFrame(ButtonPotFrame,new TGLayoutHints(kLHintsTop | kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////PRECISION FRAME/////////////
    //create frames
    PrecisionGroupFrame = new TGGroupFrame(ControlFrame,"Precision",kVerticalFrame);
    PrecisionFrame = new TGHorizontalFrame(PrecisionGroupFrame);
    SamplingFrame = new TGHorizontalFrame(PrecisionGroupFrame);

    //create entries
    PrecisionLabel = new TGLabel(PrecisionFrame,"eh pairs followed (1=best, 100=fastest):");
    PrecisionEntry = new TGNumberEntry(PrecisionFrame, valueMap["PRECISION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1,100);
    SamplingLabel = new TGLabel(SamplingFrame,"Sampling [GigaSample]:");
    SamplingEntry = new TGNumberEntry(SamplingFrame, valueMap["SAMPLING"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1,1000);
    
    //develop entries
    SamplingEntry->SetState(kTRUE);
    PrecisionEntry->SetState(kTRUE);

    //add entries
    PrecisionFrame->AddFrame(PrecisionLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
	PrecisionFrame->AddFrame(PrecisionEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    SamplingFrame->AddFrame(SamplingLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    SamplingFrame->AddFrame(SamplingEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    
    //add frames
    PrecisionGroupFrame->AddFrame(PrecisionFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
    PrecisionGroupFrame->AddFrame(SamplingFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
	ControlFrame->AddFrame(PrecisionGroupFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////OUTPUT FILE FRAME/////////////
    //create frames
	NameFrame = new TGGroupFrame(ControlFrame,"Output File",kHorizontalFrame);
    
    //create entries
	FileNameOnButton = new TGCheckButton(NameFrame,"ON",0);
    FileNameLabel = new TGLabel(NameFrame, "Name:");
    FileNameEntry = new TGTextEntry(NameFrame, new TGTextBuffer(5),kSunkenFrame | kDoubleBorder | kOwnBackground);

    //develop entries
	FileNameOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetFileNameOn(Bool_t)");
	FileNameEntry->SetText("wf");
	FileNameEntry->SetMaxLength(4096);
	FileNameEntry->SetAlignment(kTextLeft);
	FileNameEntry->Resize(104,FileNameEntry->GetDefaultHeight());
    FileNameEntry->MoveResize(56,80,150,22);
    FileNameEntry->SetState(kFALSE);
    FileNameLabel->Disable(kTRUE);
    
    //add entries
    NameFrame->AddFrame(FileNameOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(FileNameLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(FileNameEntry, new TGLayoutHints(kLHintsRight| kLHintsTop | kLHintsExpandX,1,1,1,1));
	
    //add frames
    ControlFrame->AddFrame(NameFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////BATCH MODE FRAME/////////////
    //create frames
	BatchFrame = new TGGroupFrame(ControlFrame,"Batch Mode",kHorizontalFrame);
    BatchOnButtonFrame = new TGHorizontalFrame(BatchFrame);
    BatchEventNumberFrame = new TGHorizontalFrame(BatchFrame);
    
    //create entries
    BatchOnButton = new TGCheckButton(BatchFrame, "ON",0);
    EventsLabel = new TGLabel(BatchFrame,"Number of events:");
    EventsEntry = new TGNumberEntry(BatchFrame, (Int_t) 1,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);

    //develop entries
	BatchOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetBatchOn(Bool_t)");
    EventsEntry->SetState(kFALSE);
    EventsLabel->Disable(kTRUE);

    //add entries
    BatchFrame->AddFrame(BatchOnButton, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    BatchFrame->AddFrame(EventsLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    BatchFrame->AddFrame(EventsEntry, new TGLayoutHints(kLHintsRight ,1,1,1,1));

    //add frames
    BatchFrame ->AddFrame(BatchOnButtonFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX ,0,0,0,0));
    BatchFrame ->AddFrame(BatchEventNumberFrame, new TGLayoutHints(kLHintsCenterX ,0,0,0,0));
	ControlFrame->AddFrame(BatchFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    
    ////////////////////////////////////////////
    /////////IMPACT FRAME/////////////
    //create frames
	
	/* 
   CarriersInFrame = new TGGroupFrame(ControlFrame,"Impact",kHorizontalFrame);
    CarriersInLabelFrame = new TGVerticalFrame (CarriersInFrame);
    CarriersInSetFrame = new TGHorizontalFrame (CarriersInFrame);

    //create entries
    CarriersInLabel =  new TGLabel(CarriersInLabelFrame, new TGString("Position [um], Angle [deg]:"));
    CarriersInNumberentry = new TGNumberEntry(CarriersInSetFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    CarriersAngleNumberentry = new TGNumberEntry(CarriersInSetFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,-45,45);
    BatchRandomButton = new TGCheckButton(CarriersInLabelFrame, "Random Impact",0);

    //develop entries
    BatchRandomButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetBatchRandomOn(Bool_t)");

    // add entries
	CarriersInLabelFrame->AddFrame(CarriersInLabel, new TGLayoutHints(kLHintsLeft ,1,1,2,2));
	CarriersInSetFrame->AddFrame(CarriersInNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
	CarriersInSetFrame->AddFrame(CarriersAngleNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
	CarriersInLabelFrame->AddFrame(BatchRandomButton, new TGLayoutHints(kLHintsLeft,1,1,2,0));
	
    //add frames
    ControlFrame->AddFrame(CarriersInFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInLabelFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInSetFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));

    */
    ////////////////////////////////////////////
    
    ////////////////////////////////////////////
    /////////SELECT PARTICLES FRAME/////////////
    //create frames
    ParticlesPropertiesFrame = new TGGroupFrame(ControlFrame,"Select Particles",kVerticalFrame);

    ParticlesKindFrame = new TGVerticalFrame(ParticlesPropertiesFrame);
    ParticlesSpecificsFrame = new TGHorizontalFrame(ParticlesPropertiesFrame, kHorizontalFrame);
    NumberFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    ParticlesSpecificsLabelFrame = new TGHorizontalFrame(ParticlesSpecificsFrame);
    ParticlesSpecificsSetFrame = new TGVerticalFrame(ParticlesSpecificsFrame);
    
    //create entries
    ParticleKind = new TGComboBox(ParticlesKindFrame);
    ParticleKind->AddEntry(new TGString("MIP: uniform Q, Qtot = 75*Height"), 1);
    ParticleKind->AddEntry(new TGString("MIP: NON uniform Q, Qtot = 75*Height"), 2);
    ParticleKind->AddEntry(new TGString("MIP: NON uniform Q, Qtot = Landau"), 3);
    ParticleKind->AddEntry(new TGString("Laser (1064 nm): Top-TCT, Q user"), 4);
    ParticleKind->AddEntry(new TGString("Laser (1064 nm): Edge-TCT, Q user"), 8);
    ParticleKind->AddEntry(new TGString("Laser (4-600 nm)/Alpha from top: E = 5MeV"), 5);
    ParticleKind->AddEntry(new TGString("Laser (4-600 nm)/Alpha from bottom: E = 5MeV"), 6);
    ParticleKind->AddEntry(new TGString("X-Ray"), 9);
    ParticleKind->AddEntry(new TGString("Current pulse"), 7);

    
    ParticleSpecificsEntry = new TGNumberEntry(ParticlesSpecificsSetFrame,  0,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ParticleSpecificsLabel1 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("Q/um"));
    ParticleSpecificsLabel2 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("Range [um]"));
    ParticleSpecificsLabel3 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("Duration [ns]"));
    ParticleSpecificsLabel4 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString(" E [keV]"));

    NumberLabel = new TGLabel(NumberFrame,"Number of Particles: ");
    NumberEntry = new TGNumberEntry(NumberFrame, valueMap["NUMBERP"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1,100);
    
    //develop entries
    ParticleKind->Select(1);
    ParticleKind->Connect("Selected(Int_t)","WFGUI", this, "CallSetPart(Int_t)");
    ParticleKind->Resize(ParticleKind->GetWidth()+240, ParticleKind->GetHeight()+10);
    ParticleSpecificsEntry->SetState(kFALSE);
    ParticleSpecificsLabel1->SetMargins(0,0,2,2);
    ParticleSpecificsLabel1->Disable(kTRUE);
    ParticleSpecificsLabel2->SetMargins(0,0,2,2);
    ParticleSpecificsLabel2->Disable(kTRUE);
    ParticleSpecificsLabel3->SetMargins(0,0,2,2);
    ParticleSpecificsLabel3->Disable(kTRUE);
    ParticleSpecificsLabel3->SetMargins(0,0,2,2);
    ParticleSpecificsLabel3->Disable(kTRUE);
    ParticleSpecificsLabel4->Disable(kTRUE);
    ParticleSpecificsLabel4->SetMargins(0,0,2,2);
    ParticleSpecificsLabel4->Disable(kTRUE);
    NumberEntry->SetState(kTRUE);
    
    
    //add entries
    ParticlesKindFrame->AddFrame(ParticleKind, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX, 1,1,1,1));
    ParticlesSpecificsSetFrame->AddFrame(ParticleSpecificsEntry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel1, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,6,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel2, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,6,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel3, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,1,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel4, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,1,2,1));
    NumberFrame->AddFrame(NumberLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    NumberFrame->AddFrame(NumberEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //add frames
    ParticlesPropertiesFrame -> AddFrame(ParticlesKindFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
    ParticlesPropertiesFrame -> AddFrame(ParticlesSpecificsFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0,0,0,0));
    ParticlesSpecificsFrame->AddFrame(ParticlesSpecificsLabelFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,0,0,0,0));
    ParticlesSpecificsFrame->AddFrame(ParticlesSpecificsSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    ControlFrame -> AddFrame(ParticlesPropertiesFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX , 1,1,1,1));

    ///////////////// top frame /////
    //        NumberFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    //CarriersInFrame = new TGGroupFrame(ParticlesPropertiesFrame,"Top",kHorizontalFrame);
     CarriersInFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    CarriersInLabelFrame = new TGHorizontalFrame (CarriersInFrame);
    CarriersInSetFrame = new TGHorizontalFrame (CarriersInFrame);
    
    CarriersInLabel =  new TGLabel(CarriersInLabelFrame, new TGString("X[um],Angle[D]"));
    CarriersInNumberentry = new TGNumberEntry(CarriersInLabelFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    CarriersAngleNumberentry = new TGNumberEntry(CarriersInLabelFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,-45,45);
    //    BatchRandomButton = new TGCheckButton(CarriersInLabelFrame, "Random Impact",0);

    EdgeNumberentry = new TGNumberEntry(CarriersInSetFrame, valueMap["YPOSITION"] ,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    BatchRandomButton = new TGCheckButton(CarriersInSetFrame, "Rnd",0);
    EdgeLabel =  new TGLabel(CarriersInLabelFrame, new TGString("Y[um]:"));
	
    //develop entries
    BatchRandomButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetBatchRandomOn(Bool_t)");
    EdgeNumberentry -> SetState(kFALSE);
    EdgeLabel ->Disable(kTRUE);
    
    // add entries
    CarriersInLabelFrame->AddFrame(CarriersInLabel, new TGLayoutHints(kLHintsLeft ,1,4,4,1));
    CarriersInLabelFrame->AddFrame(EdgeLabel, new TGLayoutHints(kLHintsLeft ,1,4,4,1));
    CarriersInLabelFrame->AddFrame(CarriersAngleNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    CarriersInLabelFrame->AddFrame(CarriersInNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));


    ////    CarriersInSetFrame->AddFrame(EdgeLabel, new TGLayoutHints(kLHintsRight,1,4,4,1));
    CarriersInSetFrame->AddFrame(BatchRandomButton, new TGLayoutHints(kLHintsRight,1,1,2,0));
    CarriersInSetFrame->AddFrame(EdgeNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    // CarriersInLabelFrame->AddFrame(BatchRandomButton, new TGLayoutHints(kLHintsLeft,1,1,2,0));
    

    
    ParticlesPropertiesFrame->AddFrame(CarriersInFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInLabelFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInSetFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));

    ///////////////// Edge frame /////
    // EdgeFrame = new TGGroupFrame(ParticlesPropertiesFrame,"Edge",kHorizontalFrame);
    //  EdgeFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    //EdgeLabelFrame = new TGVerticalFrame (EdgeFrame);
    //EdgeSetFrame = new TGHorizontalFrame (EdgeFrame);
    
    //  EdgeNumberentry = new TGNumberEntry(EdgeSetFrame, valueMap["YPOSITION"] ,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    //add entries
    //EdgeLabelFrame->AddFrame(EdgeLabel, new TGLayoutHints(kLHintsLeft ,1,1,2,2));
    //EdgeSetFrame->AddFrame(EdgeNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    //develop entries
    //

    ///Add frames //
    //ParticlesPropertiesFrame->AddFrame(EdgeFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    //EdgeFrame->AddFrame(EdgeLabelFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    //EdgeFrame->AddFrame(EdgeSetFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));

    ParticlesPropertiesFrame->AddFrame(NumberFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
    
    
    ////////////////////////////////////////////
    /////////IRRADIATION FRAME/////////////
    //create frames
    IrradiationFrame  = new TGGroupFrame(ControlFrame,"Irradiation",kVerticalFrame);
    
    //    IrradiationOnFrame = new TGVerticalFrame (IrradiationFrame);

    IrradiationValuesFrame = new TGHorizontalFrame (IrradiationFrame);
    IrradiationOnFrame = new TGVerticalFrame (IrradiationValuesFrame); //nc
    IrradiationLabelFrame = new TGVerticalFrame(IrradiationValuesFrame);
    IrradiationSetFrame = new TGVerticalFrame(IrradiationValuesFrame);
    
    //create entries
    IrradiationOnButton = new TGCheckButton(IrradiationOnFrame, "CCE Effects",0);
    IrradiationEntry = new TGNumberEntry(IrradiationSetFrame, valueMap["IRRADIATION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);
    IrradiationLabel = new TGLabel(IrradiationLabelFrame,new TGString("Fluence[10^13 neq /cm^2]:"));

    DJFrame = new TGHorizontalFrame(IrradiationFrame);
    DJDetailsFrame = new TGVerticalFrame(DJFrame);
    DJDetailsTopFrame= new TGHorizontalFrame(DJDetailsFrame);
    DJDetailsBottomFrame= new TGHorizontalFrame(DJDetailsFrame);
    DJOnButton = new TGCheckButton(DJFrame, "DJ ON ",0);

    DJButtonGroup = new TGButtonGroup(DJFrame, " ", kHorizontalFrame);
    DJButton[0] = new TGRadioButton(DJButtonGroup, new TGHotString("Linear"));
    DJButton[1] = new TGRadioButton(DJButtonGroup, new TGHotString("Step"));
    DJEntry = new TGNumberEntry(DJDetailsTopFrame, valueMap["DOUBLEJUNCTION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,100);
    DJehEntry = new TGNumberEntry(DJDetailsBottomFrame, valueMap["NA_OVER_ND"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,100);
    DJLabel = new TGLabel(DJDetailsTopFrame,new TGString("   N_{Eff}[0-100]:"));
    DJehLabel = new TGLabel(DJDetailsBottomFrame,new TGString("  N_A/N_D:"));

    //develop entries
    IrradiationOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetIrradiationOn(Bool_t)");
    IrradiationEntry -> SetState(kFALSE);
    IrradiationLabel ->Disable(kTRUE);
    DJOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetDJOn(Bool_t)");
    DJEntry -> SetState(kFALSE);
    DJehEntry -> SetState(kFALSE);
    DJLabel ->Disable(kTRUE);
    DJehLabel ->Disable(kTRUE);
    DJButton[0]->SetState(kButtonDown);
    DJButton[0]->SetOn(kTRUE);
    DJButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDJType()");
    DJButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDJType()");
    CallSetDJType();

    //add entries
    IrradiationOnFrame->AddFrame(IrradiationOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,5,5,1));
    IrradiationSetFrame->AddFrame(IrradiationEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    IrradiationLabelFrame->AddFrame(IrradiationLabel, new TGLayoutHints(kLHintsRight,1,5,5,1));

    DJDetailsTopFrame->AddFrame(DJEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DJDetailsTopFrame->AddFrame(DJLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,1,1,5,1));
    DJDetailsBottomFrame->AddFrame(DJehEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DJDetailsBottomFrame->AddFrame(DJehLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,1,1,1,1));
    DJDetailsFrame->AddFrame(DJDetailsTopFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandX,1,1,1,1));
    DJDetailsFrame->AddFrame(DJDetailsBottomFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandX,1,1,1,1));
    
    //add frames
    IrradiationValuesFrame -> AddFrame(IrradiationOnFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    IrradiationValuesFrame->AddFrame(IrradiationLabelFrame,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,0,0,0,0));
    IrradiationValuesFrame->AddFrame(IrradiationSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    IrradiationFrame -> AddFrame(IrradiationValuesFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0,0,0,0));
    ControlFrame->AddFrame(IrradiationFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX,1,1,1,1));
    IrradiationFrame -> AddFrame(DJFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    DJFrame -> AddFrame(DJOnButton, new TGLayoutHints(kLHintsLeft, 0,0,15,0));
    DJFrame -> AddFrame(DJButtonGroup, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    DJFrame -> AddFrame(DJDetailsFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    
    ////////////////////////////////

    ////////////////////////////////////////////
    /////////PLOT SETTINGS FRAME/////////////
    //create frames
	PlotGroupFrame = new TGGroupFrame(ControlFrame,"Plot Settings",kVerticalFrame); // GroupFrame: plot settings
    //UpdateFrame = new TGHorizontalFrame(PlotGroupFrame);

    //create entries
	Eyes = new TGCheckButton(PlotGroupFrame, "Draw Electric Field",0);
    AbsCur = new TGCheckButton(PlotGroupFrame, "Draw Current Absolute Value",0);
    ShowCur = new TGCheckButton(PlotGroupFrame, "Draw e/h motion",0);
    //LessPlotCheck = new TGCheckButton(UpdateFrame, "No 1D Plots",0);
    //Less2DPlotCheck = new TGCheckButton(UpdateFrame, "No 1D && 2D",0);

    //develop entries
	Eyes->Connect("Toggled(Bool_t)","WFGUI",this,"SetPlotFieldHist(Bool_t)");
	AbsCur->Connect("Toggled(Bool_t)","WFGUI",this,"SetAbsCur(Bool_t)");
	ShowCur->Connect("Toggled(Bool_t)","WFGUI",this,"SetShowCur(Bool_t)");
	//LessPlotCheck->Connect("Toggled(Bool_t)","WFGUI",this,"SetLessPlot(Bool_t)");
	//Less2DPlotCheck->Connect("Toggled(Bool_t)","WFGUI",this,"SetLess2DPlot(Bool_t)");

    //add entries
	PlotGroupFrame->AddFrame(Eyes, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
	PlotGroupFrame->AddFrame(AbsCur, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
	PlotGroupFrame->AddFrame(ShowCur, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    //UpdateFrame->AddFrame(LessPlotCheck, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    //UpdateFrame->AddFrame(Less2DPlotCheck, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    
    //add frames
	//PlotGroupFrame->AddFrame(UpdateFrame,new TGLayoutHints(kLHintsLeft,1,1,1,1));
	ControlFrame->AddFrame(PlotGroupFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////


	////////////////////////////////////////////
    /////////CURRENT SETTINGS FRAME/////////////
    //create frames
    CurrentsFrame = new TGGroupFrame(ControlFrame,"Current Settings",kVerticalFrame);
	BFieldFrame = new TGHorizontalFrame(CurrentsFrame);
    DiffusionAndDispersionFrame = new TGVerticalFrame(CurrentsFrame);
    TempSetFrame = new TGHorizontalFrame(CurrentsFrame);
    
    //create entries
    CurrentsButton[0] = new TGCheckButton(BFieldFrame,new TGHotString("B-Field on at[T]:"),0);
    BfieldEntry = new TGNumberEntry(BFieldFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEAAnyNumber);
    CurrentsButton[1] = new TGCheckButton(DiffusionAndDispersionFrame,new TGHotString("Diffusion"),0);
    CurrentsButton[2] = new TGCheckButton(DiffusionAndDispersionFrame,new TGHotString("Charge Cloud Dispersion (no Alpha)"),0);
    TempLabel = new TGLabel(TempSetFrame,"Temperature[K]:");
    TempEntry = new TGNumberEntry(TempSetFrame, valueMap["TEMPERATURE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
	
    //develop entries
    BfieldEntry->SetState(kFALSE);
    TempEntry->SetState(kTRUE);
    CurrentsButton[1]->Connect("Toggled(Bool_t)","WFGUI",this,"SetDiffusion(Bool_t)");
    CurrentsButton[0]->Connect("Toggled(Bool_t)","WFGUI",this,"SetBField(Bool_t)");
    CurrentsButton[2]->Connect("Toggled(Bool_t)","WFGUI",this,"SetChargeCloud(Bool_t)");
    
    //add entries
    BFieldFrame->AddFrame(CurrentsButton[0], new TGLayoutHints(kLHintsLeft,1,1,3,2));
    BFieldFrame->AddFrame(BfieldEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DiffusionAndDispersionFrame->AddFrame(CurrentsButton[1], new TGLayoutHints(kLHintsLeft,1,1,2,1));
    DiffusionAndDispersionFrame->AddFrame(CurrentsButton[2], new TGLayoutHints(kLHintsLeft,1,1,1,2));
    TempSetFrame->AddFrame(TempLabel, new TGLayoutHints(kLHintsLeft,1,1,3,2));
    TempSetFrame->AddFrame(TempEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    
    //add frames
    CurrentsFrame->AddFrame(BFieldFrame, new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
    CurrentsFrame->AddFrame(DiffusionAndDispersionFrame, new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
	CurrentsFrame->AddFrame(TempSetFrame,new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
    ControlFrame->AddFrame(CurrentsFrame,new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

	SettingsFrame2->AddFrame(ControlFrame, new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kLHintsExpandY,1,1,1,1));

    //////END OF CONTROL FRAME/////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////START OF DETECTOR PROPERTIES FRAME //////////////////////////////////////////////////////
    RightFrame = new TGVerticalFrame(SettingsFrame2);
    DetectorPropertiesFrame = new TGGroupFrame(RightFrame,"Detector Properties",kVerticalFrame);
	
    ////////////////////////////////////////////
    /////////TYPE FRAME/////////////
    //create frames
	TypeFrameGroup  = new TGButtonGroup(DetectorPropertiesFrame,"Type",kHorizontalFrame);
    
    //create entries
	TypeButton[0] = new TGRadioButton(TypeFrameGroup, new TGHotString("Si"));
	TypeButton[1] = new TGRadioButton(TypeFrameGroup, new TGHotString("Diamond"));
	TypeButton[2] = new TGRadioButton(TypeFrameGroup, new TGHotString("Free"));
	TypeButton[0]->SetState(kButtonDown);

    //develop entries
	TypeButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");	
	TypeButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");	
	TypeButton[2]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");
	CallSetDetType();
    
    //add entries
    TypeFrameGroup->AddFrame(TypeButton[0], new TGLayoutHints(kLHintsCenterX,-160,10,0,0));
    TypeFrameGroup->AddFrame(TypeButton[1], new TGLayoutHints(kLHintsCenterX,-40,10,0,0));
    TypeFrameGroup->AddFrame(TypeButton[2], new TGLayoutHints(kLHintsRight,5,10,0,0));
    
    //add frames
    DetectorPropertiesFrame->AddFrame(TypeFrameGroup ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////DOPING TYPE FRAME/////////////
    //create frames
    DopingFrame = new TGGroupFrame(DetectorPropertiesFrame, "Doping type", kHorizontalFrame);
    DopStripButtonGroup = new TGButtonGroup(DopingFrame, "Strips", kHorizontalFrame);
    DopBulkButtonGroup = new TGButtonGroup(DopingFrame, "Bulk", kHorizontalFrame);
    
    //create entries
    StripButton[0] = new TGRadioButton(DopStripButtonGroup, new TGHotString("n"));
    StripButton[1] = new TGRadioButton(DopStripButtonGroup, new TGHotString("p"));
    BulkButton[0] = new TGRadioButton(DopBulkButtonGroup, new TGHotString("n"));
    BulkButton[1] = new TGRadioButton(DopBulkButtonGroup, new TGHotString("p"));
    
    //develop entries
    StripButton[0]->SetState(kButtonDown);
    BulkButton[1]->SetState(kButtonDown);
    StripButton[0]->SetOn(kTRUE);
    BulkButton[1]->SetOn(kTRUE);
    StripButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDoping1()");
    StripButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDoping1()");
    BulkButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDoping0()");
    BulkButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDoping0()");
    CallSetDoping0();
    CallSetDoping1();
    
    //add entries
    DopStripButtonGroup->AddFrame(StripButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    DopStripButtonGroup->AddFrame(StripButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    DopBulkButtonGroup->AddFrame(BulkButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    DopBulkButtonGroup->AddFrame(BulkButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    
    //add frames
    DopingFrame->AddFrame(DopStripButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    DopingFrame->AddFrame(DopBulkButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    DetectorPropertiesFrame->AddFrame(DopingFrame,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////DIMENSIONS FRAME/////////////
    //create frames
    DimensionsFrame  = new TGGroupFrame(DetectorPropertiesFrame,"Dimensions",kHorizontalFrame);
	DimLabelFrame = new TGVerticalFrame(DimensionsFrame);//200 60
	DimSetFrame = new TGVerticalFrame(DimensionsFrame);
    WPFrame = new TGHorizontalFrame(DimSetFrame);//50 90

    
    //create entries
    XMAXentry = new TGNumberEntry(DimSetFrame, valueMap["STRIP_NUMB"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    XMAXLabel = new TGLabel(DimLabelFrame,new TGString("# of strips (1,3,5..):"));
    YMAXentry = new TGNumberEntry(DimSetFrame, valueMap["DETECT_HEIGHT"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    YMAXLabel = new TGLabel(DimLabelFrame,new TGString("Thickness[um]:"));
    PitchLabel = new TGLabel(DimLabelFrame,new TGString("Width[um), Pitch[um)]:"));
    Pitchentry = new TGNumberEntry(WPFrame, valueMap["STR_PITCH"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    Widthentry = new TGNumberEntry(WPFrame, valueMap["STR_WIDTH"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);

    //add entries
    DimSetFrame->AddFrame(XMAXentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    DimLabelFrame->AddFrame(XMAXLabel, new TGLayoutHints(kLHintsTop,1,1,1,1));
    DimSetFrame->AddFrame(YMAXentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    DimLabelFrame->AddFrame(YMAXLabel, new TGLayoutHints(kLHintsTop,1,1,1,1));
    DimLabelFrame->AddFrame(PitchLabel, new TGLayoutHints(kLHintsTop,1,1,3,1));
    WPFrame->AddFrame(Pitchentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    WPFrame->AddFrame(Widthentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));

    //develop entries
    PitchLabel->SetMargins(0,0,3,0);
    YMAXLabel->SetMargins(0,0,3,2);
    XMAXLabel->SetMargins(0,0,3,2);

    //add frames
    DimSetFrame->AddFrame(WPFrame, new TGLayoutHints(kLHintsRight | kLHintsTop,0,0,0,0));
    DimensionsFrame->AddFrame(DimLabelFrame,new TGLayoutHints(kLHintsLeft,0,0,0,0));
    DimensionsFrame->AddFrame(DimSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    DetectorPropertiesFrame->AddFrame(DimensionsFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////


    ////////////////////////////////////////////
    /////////GAIN FRAME/////////////
    //create frames
	GainFrame  = new TGGroupFrame(DetectorPropertiesFrame,"Gain",kHorizontalFrame); 
	GainSetFrame = new TGVerticalFrame(GainFrame);//200 60
	GainLabelFrame = new TGVerticalFrame(GainFrame);
    
    //create entries
	Gainentry = new TGNumberEntry(GainSetFrame, valueMap["GAIN_SCL"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1.,100.);
    GainLabel = new TGLabel(GainLabelFrame,new TGString("Gain (1 = no G):"));
    ForceGainButton = new TGCheckButton(GainSetFrame, "ON",0);
    ForceGainLabel = new TGLabel(GainLabelFrame,new TGString("Force Fixed Gain (Irr. OFF)"));
    GainRatioentry = new TGNumberEntry(GainSetFrame, valueMap["HE_GAIN_RAT"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,1.);
    GainRatioLabel = new TGLabel(GainLabelFrame,new TGString("h/e Gain ratio:"));
    GainIndententry = new TGNumberEntry(GainSetFrame, valueMap["GAIN_LYR_RSS"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,100.);
    GainIndentLabel = new TGLabel(GainLabelFrame,new TGString("Gain recess (um):"));

    //develop entries
    ForceGainButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetForceGain(Bool_t)");
    
    //add entries
	GainSetFrame->AddFrame(Gainentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	GainLabelFrame->AddFrame(GainLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	GainSetFrame->AddFrame(ForceGainButton, new TGLayoutHints(kLHintsRight,1,1,1,1));
	GainLabelFrame->AddFrame(ForceGainLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	GainSetFrame->AddFrame(GainRatioentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	GainLabelFrame->AddFrame(GainRatioLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	GainSetFrame->AddFrame(GainIndententry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	GainLabelFrame->AddFrame(GainIndentLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,1));

    //add frames
	GainFrame->AddFrame(GainSetFrame, new TGLayoutHints(kLHintsRight | kLHintsTop,0,0,0,0));
	GainFrame->AddFrame(GainLabelFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
    DetectorPropertiesFrame->AddFrame(GainFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////


    ////////////////////////////////////////////
    /////////VOLTAGE FRAME/////////////
    //create frames
    VoltageFrame  = new TGGroupFrame(DetectorPropertiesFrame,"Voltage",kHorizontalFrame);
    VolLabelFrame = new TGVerticalFrame(VoltageFrame);
    VolSetFrame = new TGHorizontalFrame(VoltageFrame);
    
    //create entries
	BiasLabel = new TGLabel(VolLabelFrame,new TGString("Bias[V], Depletion[V]:"));
    Biasentry = new TGNumberEntry(VolSetFrame, valueMap["BIAS_VOLTAGE"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    Depletionentry = new TGNumberEntry(VolSetFrame, valueMap["DEPL_VOLTAGE"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);

    //add entries
	VolLabelFrame->AddFrame(BiasLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,1,1));
	VolSetFrame->AddFrame(Biasentry, new TGLayoutHints(kLHintsCenterX | kLHintsTop,1,1,1,1));
	VolSetFrame->AddFrame(Depletionentry, new TGLayoutHints(kLHintsCenterX | kLHintsTop,1,1,1,1));											//
	
    //develop entries
    BiasLabel->SetMargins(0,0,3,0);
    
    //add frames
    VoltageFrame->AddFrame(VolLabelFrame,new TGLayoutHints(kLHintsLeft,0,0,0,0));
    VoltageFrame->AddFrame(VolSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    DetectorPropertiesFrame->AddFrame(VoltageFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    
    RightFrame->AddFrame(DetectorPropertiesFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 1,1,1,1));	
    
    //////END OF DETECTOR PROPERTIES FRAME/////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////START OF READ OUT FRAME /////////////////////////////////////////////////////////////////
    //create frames
    ReadOutButtonGroup = new TGButtonGroup(RightFrame, "Read-Out", kHorizontalFrame);

    //create entries
    ReadOutButton[0] = new TGRadioButton(ReadOutButtonGroup, new TGHotString(" Top Strip "));
    ReadOutButton[1] = new TGRadioButton(ReadOutButtonGroup, new TGHotString(" Backplane"));
    ReadOutButton[0]->SetState(kButtonDown);
    
    //develop entries
    ReadOutButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetReadOut()");
    ReadOutButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetReadOut()");
    CallSetReadOut();//why is this called even after it's connected?
    
    //add frames
    RightFrame->AddFrame(ReadOutButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    //////END OF READ OUT FRAME////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////START OF OSCILLOSCOPE FRAME /////////////////////////////////////////////////////////////
	//create frames
    ElectronicsFrame = new TGGroupFrame(RightFrame," Electronics",kVerticalFrame);
    OscilloscopeFrame = new TGHorizontalFrame(ElectronicsFrame);	
    OscilloscopeLeftFrame = new TGVerticalFrame(OscilloscopeFrame);	
    OscilloscopeRightFrame = new TGVerticalFrame(OscilloscopeFrame);	
    TRiseSetFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    ShImpFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    LCFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    ShNoiseFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    BBBWFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    BBNoiseFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);

    //create entries
    OscOnButton = new TGCheckButton(ElectronicsFrame, "ON",0);
    CDLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("Detector Cap[pF] Ind [nH]:"));
    CDEntry = new TGNumberEntry(LCFrame, valueMap["CAPACITANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1.,40.);
    LDEntry = new TGNumberEntry(LCFrame, valueMap["INDUCTANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1.,40.);

    OscBWLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("Scope (50 [Ohm]) BW[GHz]:"));
    OscBWEntry = new TGNumberEntry(OscilloscopeRightFrame, valueMap["OSCOPE_BW"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ImpLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA:Imp[Ohm],Tr.Imp[mV/fQ]:"));
    //    ImpEntry = new TGNumberEntry(OscilloscopeRightFrame, valueMap["IMPEDANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    TRiseLabel = new TGLabel(OscilloscopeLeftFrame, new TGString("CSA:T_r,T_f[ns]:"));
    TFallEntry = new TGNumberEntry(TRiseSetFrame, valueMap["SHPR_DCY_TIME"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,40.);
    TRiseEntry = new TGNumberEntry(TRiseSetFrame, valueMap["SHPR_INT_TIME"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,60.);
    //    ShTransLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA Trans Imp.[mV/fQ]:"));
    CSAImpEntry = new TGNumberEntry(ShImpFrame, valueMap["IMPEDANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShTransEntry = new TGNumberEntry(ShImpFrame, valueMap["SHPR_TRANS"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShNoiseLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA:Noise,Vth[mV,CFD if<1]:"));
    CSAVthEntry = new TGNumberEntry(ShNoiseFrame, valueMap["VTH"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShNoiseEntry = new TGNumberEntry(ShNoiseFrame, valueMap["SHPR_NOISE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBBWLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("BB:Imp[Ohm],BW,Gain:"));
    BBImpEntry = new TGNumberEntry(BBBWFrame, valueMap["BB_IMP"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBGainEntry = new TGNumberEntry(BBBWFrame, valueMap["BBGAIN"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBBWEntry = new TGNumberEntry(BBBWFrame, valueMap["BBBW"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBNoiseLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("BB:Noise,Vth[mV,CFD if<1]:"));
    BBVthEntry = new TGNumberEntry(BBNoiseFrame, valueMap["BBVTH"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBNoiseEntry = new TGNumberEntry(BBNoiseFrame, valueMap["BB_NOISE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);

    //develop entries
	OscOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetOscOn(Bool_t)");
    CDEntry->SetState(kFALSE);
    LDEntry->SetState(kFALSE);
    OscBWLabel->SetMargins(0,0,4,4);
    OscBWLabel->Disable(kTRUE);
    OscBWEntry->SetState(kFALSE);
    CDLabel->Disable(kTRUE);
    ImpLabel->SetMargins(0,0,4,4);
    ImpLabel->Disable(kTRUE);
    CSAImpEntry->SetState(kFALSE);
    TRiseLabel->SetMargins(0,0,4,4);
    TRiseLabel->Disable(kTRUE);
    TFallEntry->SetState(kFALSE);
    TRiseEntry->SetState(kFALSE);
    //    ShTransLabel->SetMargins(0,0,4,4);
    ShTransEntry->SetState(kFALSE);
    ShNoiseLabel->SetMargins(0,0,4,4);
    ShNoiseLabel->Disable(kTRUE);
    // ShTransLabel->Disable(kTRUE);
    CSAVthEntry->SetState(kFALSE);
    ShNoiseEntry->SetState(kFALSE);
    BBBWLabel->SetMargins(0,0,4,4);
    BBGainEntry->SetState(kFALSE);
    BBBWEntry->SetState(kFALSE);
    BBImpEntry->SetState(kFALSE);
    BBNoiseLabel->SetMargins(0,0,4,4);
    BBBWLabel->Disable(kTRUE);
    BBNoiseLabel->Disable(kTRUE);
    BBVthEntry->SetState(kFALSE);
    BBNoiseEntry->SetState(kFALSE);

    //add entries
    ElectronicsFrame->AddFrame(OscOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));
    OscilloscopeLeftFrame->AddFrame(CDLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(OscBWLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(ImpLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(TRiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    //    OscilloscopeLeftFrame->AddFrame(ShTransLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(ShNoiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(BBBWLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(BBNoiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));

    //    OscilloscopeRightFrame->AddFrame(CDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    //OscilloscopeRightFrame->AddFrame(LDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    LCFrame->AddFrame(LDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    LCFrame->AddFrame(CDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //    OscilloscopeRightFrame->AddFrame(ImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    TRiseSetFrame->AddFrame(TFallEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    TRiseSetFrame->AddFrame(TRiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShImpFrame->AddFrame(ShTransEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShImpFrame->AddFrame(CSAImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShNoiseFrame->AddFrame(CSAVthEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShNoiseFrame->AddFrame(ShNoiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    BBBWFrame->AddFrame(BBGainEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBBWFrame->AddFrame(BBBWEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBBWFrame->AddFrame(BBImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBNoiseFrame->AddFrame(BBVthEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBNoiseFrame->AddFrame(BBNoiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //add frames
    OscilloscopeRightFrame->AddFrame(LCFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));    
    OscilloscopeRightFrame->AddFrame(OscBWEntry, new TGLayoutHints(kLHintsRight ,1,1,1,1));    
    OscilloscopeRightFrame->AddFrame(ShImpFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(TRiseSetFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(ShNoiseFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(BBBWFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(BBNoiseFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeFrame->AddFrame(OscilloscopeLeftFrame, new TGLayoutHints(kLHintsLeft |  kLHintsExpandX, 1,1,1,1));
    OscilloscopeFrame->AddFrame(OscilloscopeRightFrame, new TGLayoutHints(kLHintsRight, 1,1,1,1));
    ElectronicsFrame->AddFrame(OscilloscopeFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX , 1,1,1,1));
    RightFrame->AddFrame(ElectronicsFrame, new TGLayoutHints(kLHintsExpandX| kLHintsTop , 0,0,0,0));
    SettingsFrame2->AddFrame(RightFrame, new TGLayoutHints(kLHintsCenterY , 1,1,1,1));
    //////END OF OSCILLOSOPE FRAME/////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    SettingsGlobalFrame->AddFrame(SettingsFrame2,new TGLayoutHints( kLHintsCenterY | kLHintsExpandY,1,1,1,1));
    AddFrame(SettingsGlobalFrame,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
    
    ///END OF SETTINGS FRAME///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    ///*********************************************** END OF SETTINGS ****************************************************///

    
    
    ///******************************************** CANVAS AND HISTOGRAMS *************************************************///
	
    //set canvas specifics
    canvasp = driftcanvas->GetCanvas();
    canvasp->SetBottomMargin(0.15);
    canvasp->SetTopMargin(0.1);
    canvasp->SetLeftMargin(0.15);
    canvasp->SetRightMargin(0.15);
    
    canvaspc = driftpcanvas->GetCanvas();
    canvaspc->Divide(2,2,0.004,0.004);
    canvaspc->SetBottomMargin(0.15);
    canvaspc->SetTopMargin(0.1);
    canvaspc->SetLeftMargin(0.15);
    canvaspc->SetRightMargin(0.15);
    canvaspc->cd(1);

    canvasw = weightcanvas->GetCanvas();
    canvasw->SetBottomMargin(0.15);
    canvasw->SetTopMargin(0.1);
    canvasw->SetLeftMargin(0.15);
    canvasw->SetRightMargin(0.15);
    
    canvaswc = weightpcanvas->GetCanvas();
    canvaswc->Divide(2,1,0.002,0.002);
    canvaswc->SetBottomMargin(0.15);
    canvaswc->SetTopMargin(0.1);
    canvaswc->SetLeftMargin(0.15);
    canvaswc->SetRightMargin(0.15);

    curcanvas=currentscanvas->GetCanvas();
    curcanvas->SetBottomMargin(0.15);
    curcanvas->SetTopMargin(0.1);
    curcanvas->SetLeftMargin(0.15);
    curcanvas->SetRightMargin(0.15);

    osccanvas=oscilloscopecanvas->GetCanvas();
    osccanvas->SetBottomMargin(0.15);
    osccanvas->SetTopMargin(0.1);
    osccanvas->SetLeftMargin(0.2);
    osccanvas->SetRightMargin(0.2);

    
    gPad->SetRightMargin(0.2);
    canvaspc->cd(2);
    gPad->SetRightMargin(0.2);
    
    
    // Create histograms for potentials and fields
    ellipse = new TEllipse();
    dhist = new TH2F("dhist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    whist = new TH2F("whist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    wfhist = new TH2F("wfhist"," ",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    dfhist = new TH2F("dfhist"," ",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );
    chist = new TH2F("chist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    chhist = new TH2F("chhist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );
    ctothist = new TH2F("ctothist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    Exhist = new TH2F("Exthist"," ; x [um]; y [um]",dwpot.GetXMAX(),0,dwpot.GetXMAX(),dwpot.GetYMAX()+2, 0,dwpot.GetYMAX()+2 );  
    
    // set offset of axistitles
    dhist->SetTitleOffset(1, "Y");
    dhist->SetTitleOffset(1, "X");
    whist->SetTitleOffset(1, "Y");
    whist->SetTitleOffset(1, "X");
    
    //create other histograms
    jhist = new TH1F("jhist","CSA Jitter; Time [ns]; ",250,0,0.5);
    Enhist = new TH1F("Enhist","Entries; # of e/h pairs;",1000,0,5000);  
    BBjhist = new TH1F("BBjhist","BB Jitter; Time [ns]; ",250,0,0.5);  
    tvthhist = new TH1F("tvthhist"," CSA Time Resolution; Time [ns];  ",800,0,8.);  
    BBtvthhist = new TH1F("BBtvthhist"," BB Time Resolution; Time [ns];  ",600,0,6.);  
    twtothist = new TH2F("twtothist","CSA Time Over Threshold; Time over Threshold [ns];  T_rise;  ",250,0,50., 250,0,5.);  
    BBtwtothist = new TH2F("BBtwtothist","BB Time Over Threshold; Time over Threshold [ns]; T_rise;  ",250,0,15., 250,0,3.);  
    
    //develop histograms
    twtothist->SetMarkerColor(2);
    twtothist->SetMarkerSize(0.4);
    twtothist->SetMarkerStyle(2);
    BBtwtothist->SetMarkerColor(4);
    BBtwtothist->SetMarkerSize(0.4);
    BBtwtothist->SetMarkerStyle(2);
    
    // Initialize variables
    weightcut = new TGraph();	
    driftcut = new TGraph();
    dopdens = new TGraph();	
    wherecut = dwpot.GetXMAX()/2+1;
    wherecut2 = dwpot.GetXMAX()/2+1;	
    WhereCut->SetNumber(wherecut);
    WhereCut2->SetNumber(wherecut2);
    stopped=false;
    PotentialThread=0;
    CurrentsThread=0;
    plotupdate=false;
    LessPlot=false;
    Less2DPlot=false;
    bfieldon=false;
    diffusionon=false;
    gainon = false;
    OscOn = false;
    FileNameOn = false;
    BatchOn = false;
    BatchRandomOn = false;
    ForceGain = false;
    IrradiationOn = false;
    DJOn = false;
    currentson=false;
    fieldyes=false;
    abscuryes=false;
    showcuryes=false;
    chargecloudon = false;
    stripdoping=1;
    bulkdoping=0;
    
    //changes to window
    MapSubwindows();
    SetWindowName("Weightfield2  Build 3.31");
    Resize(GetDefaultSize());
    MapWindow();
    
    ///***************************************** END OF CANVAS AND HISTOGRAMS **********************************************///

}

///----------------------------------------------------------------------------------------------------------------------------------------------///


///----------------------------------------------------------\\ ACTION METHODS //------------------------------------------------------------------///

void WFGUI::CallCalculatePotentials() {


	if(!GetLess2DPlot())
	  {
	    canvasw->Clear(); // when new calculation is started, clear all canvas
	    canvasp->Clear();
	    canvaswc->Clear();
	    canvaspc->Clear();
	    

	    canvasw->Update();
	    canvasp->Update();
	    canvaspc->Update();
	    canvaswc->Update();
	  }

	curcanvas->Clear();
	curcanvas->Update();
	osccanvas->Clear();
	osccanvas->Update();

	dhist->Reset();
	whist->Reset();
	twtothist->Reset();
	BBtwtothist->Reset();
	twtothist->SetStats(0);
	BBtwtothist->SetStats(0);
	tvthhist->Reset();
	BBtvthhist->Reset();
	jhist->Reset();
	BBjhist->Reset();
	SetDJValue(DJEntry->GetNumber());
	SetDJehValue(DJehEntry->GetNumber());
	SetIrradiation(IrradiationEntry->GetNumber());
	
	dwpot.SetDoping(stripdoping,bulkdoping);
	CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
	CalculatingLabel->SetTitle("Calculating Potentials ..."); // update progess label title
	//CalculatingLabel2->SetBackgroundColor(0xff0000); // set progress label color to red
	//CalculatingLabel2->SetTitle("Calculating Potentials ..."); // update progess label title


	//disable buttons

	
	//cout<<"calc potentials in: dimy= "<<YMAXentry->GetNumber()<<", dimx="<<XMAXentry->GetNumber()<<endl;
	// set pitch, width, XMAX and YMAX

	dwpot.SetPitchWidthXY(GetYMax(),GetXMax(),Pitchentry->GetNumber(),Widthentry->GetNumber());	

	//dwpot.SetGain(Gainentry->GetNumber());

	
	
	WhereCut->SetLimitValues(2, dwpot.GetXMAX()-2.);	
	WhereCut2->SetLimitValues(2,dwpot.GetXMAX()-2);	
	CarriersInNumberentry->SetLimitValues(0,dwpot.GetXMAX());	
	
	dwpot.SetV(Biasentry->GetNumber(),Depletionentry->GetNumber());  				// set depeletion and bias voltage

	for(int i=0; i<dwpot.GetXMAX(); i++) {			// reset potentials to zero as a precaution
			for(int j=0; j<dwpot.GetYMAX(); j++) {
				dwpot.SetwPotential(j,i,0.0);
				dwpot.SetdPotential(j,i,0.0);
			}
		}		
		
	dwpot.SetBoundaryConditions(ReadOutTopFlag);		// set boundary conditions
	dhist->GetXaxis()->SetLabelColor(0);	// set label color to white. because of multigrid approach, the tick labels of the axis are going to vary during calculation
	dhist->GetYaxis()->SetLabelColor(0);	

	dwpot.Multigrid(this, ReadOutTopFlag); // calculate potential



	

	///////////////// Add GAIN Layer by hand ////////////////////////


	double Scaledpot =0;
	SetGainon(false);
	if (Gainentry->GetNumber() > 1. && CallGetDetType() ==0 )
	  {
	    
	    SetGainon(true);
	    SetYGain( (double) Gainentry->GetNumber()) ;	    
	    float Gdist=0;

	    if(( StripButton[0]->IsOn() && BulkButton[1]->IsOn())  )
	      {
		for(int j= Getygainlow(); j<dwpot.GetYMAX(); j++) 
		  {
		    if (j <Getygainhigh() )    Gdist+=2.;

		    int Under = 0;
		    for(int i=0; i<dwpot.GetXMAX(); i++)
		      {		
			if (dwpot.Getdpot(dwpot.GetYMAX()-1,i) ==  Biasentry->GetNumber())
			  {
			    Under++;
			    //			    cout << "i = " <<  i << " Under =  " << Under <<  endl;
			    if (Under > GainIndententry->GetNumber() && Under <=Widthentry->GetNumber()-GainIndententry->GetNumber())
			      {
				// cout << Under  << endl;
				dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,2.1f));

			      }
			//	else   dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,2.1));
			  }
			else Under = 0;

		      }
		  }		
		Scaledpot = Biasentry->GetNumber()/dwpot.Getdpot(dwpot.GetYMAX()-1,dwpot.GetXMAX()/2);
		for(int i=0; i<dwpot.GetXMAX(); i++) 		
		  for(int j=0; j<dwpot.GetYMAX(); j++) 
			  dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)*Scaledpot);
	      }

	    else if(StripButton[0]->IsOn() && BulkButton[0]->IsOn())
	      {
		for(int j= Getygainlow(); j<dwpot.GetYMAX(); j++) 
		  {
		    if (j <Getygainhigh() )    Gdist+=2.;
		    
		    for(int i=0; i<dwpot.GetXMAX(); i++)  dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,2.1f));
		    
		  }		
		Scaledpot = Biasentry->GetNumber()/dwpot.Getdpot(dwpot.GetYMAX()-1,dwpot.GetXMAX()/2);
		for(int i=0; i<dwpot.GetXMAX(); i++) 		
		  for(int j=0; j<dwpot.GetYMAX(); j++) 
		    dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)*Scaledpot);
	      }
	    

	    else if(StripButton[1]->IsOn() && BulkButton[1]->IsOn())
	      {
		for(int j= Getygainhigh(); j>-1; j--) 
		  {
		    if (j >Getygainlow()-1 )    Gdist+=2.;

		    for(int i=0; i<dwpot.GetXMAX(); i++)
		      {		
			//			dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,2.1f));
			dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,1.8f));
		      }
		  
		  }
		Scaledpot = Biasentry->GetNumber()/dwpot.Getdpot(0,dwpot.GetXMAX()/2);
		for(int i=0; i<dwpot.GetXMAX(); i++) 		
		  for(int j=0; j<dwpot.GetYMAX(); j++) 
		      dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)*Scaledpot);
	      }
	  
	    else if(StripButton[1]->IsOn() && BulkButton[0]->IsOn())
	      {

		for(int j= Getygainhigh(); j>-1; j--) 
		  {
		    if (j >Getygainlow()-1 )    Gdist+=2.;

		    for(int i=0; i<dwpot.GetXMAX(); i++)
		      {		
			//			if (dwpot.Getdpot(dwpot.GetYMAX()-1,i) ==  0) // only under the electrode
			  dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)+pow(Gdist,2.3f));

		      }
		  }
		Scaledpot = Biasentry->GetNumber()/dwpot.Getdpot(0,dwpot.GetXMAX()/2);

		for(int i=0; i<dwpot.GetXMAX(); i++) 		
		  for(int j=0; j<dwpot.GetYMAX(); j++) 
		      dwpot.SetdPotential(j,i,dwpot.Getdpot(j,i)*Scaledpot);
	      }
	  	    	    	     
	   


	  } // end of Gain potential addition

	/////////////////////////////////////////

	CallCalculateFields(); 			// calculate electric field

	//

	if(stopped==true) {
		CalculatingLabel->SetTitle("Calculation has been stopped.");// set progress label in case of "stop"
		//	CalculatingLabel2->SetTitle("Calculation has been stopped.");// set progress label in case of "stop"
	}
	else {
		CalculatingLabel->SetBackgroundColor(0x00ff00);	// when calculation completed, set progress label color to green
		CalculatingLabel->SetTitle("Done.");
		//	CalculatingLabel2->SetBackgroundColor(0x00ff00);	// when calculation completed, set progress label color to green
		// CalculatingLabel2->SetTitle("Done.");
	}

	// enable buttons

//	return NULL;

	//Threadstop();

}	
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallCalculateCurrents() {	

  CalculateButton->SetEnabled(kFALSE);
  CalcPotButton->SetEnabled(kFALSE);
  SetButton->SetEnabled(kFALSE);
  

  CalculatingLabel->SetBackgroundColor(0xff0000);				
  CalculatingLabel->SetTitle("Calculating Currents ...");	// update progress label
  //CalculatingLabel2->SetBackgroundColor(0xff0000);				
  //CalculatingLabel2->SetTitle("Calculating Currents ...");	// update progress label
  B=0.0;
  Temp=0.0;
  float hity=0;
  
  jhist->Reset();
  BBjhist->Reset();
  tvthhist->Reset();
  BBtvthhist->Reset();
  
  //Setting variables from Input panels
  SetAlphaRange(ParticleSpecificsEntry->GetNumber());//MODIFIEDRangeEntry->GetNumber());
  SetXRayRange(XRayRangeDef*1.);// To be fixed with the appropriate energy dependance
  SetPrecision(PrecisionEntry->GetNumber());
  SetSampling(max(int(SamplingEntry->GetNumber()),50)); //below 50 the gain mechanism does not work
  // SetSampling(SamplingEntry->GetNumber());
  
  SetNumberP(NumberEntry->GetNumber());
  SetYGain(Gainentry->GetNumber());
  SetGainRatio(GainRatioentry->GetNumber());
  SetIrradiation(IrradiationEntry->GetNumber());
  SetDJValue(DJEntry->GetNumber());
  SetDJehValue(DJehEntry->GetNumber());
  
  SetShNoise(ShNoiseEntry->GetNumber());
	
  SetBBNoise(BBNoiseEntry->GetNumber());
  SetBBVth(BBVthEntry->GetNumber());
  SetBBBW(BBBWEntry->GetNumber());
  SetBBGain(BBGainEntry->GetNumber());
  SetBBImp(BBImpEntry->GetNumber());
  SetCSAImp(CSAImpEntry->GetNumber());
  
  SetOscBW(OscBWEntry->GetNumber());
  SetCDet(CDEntry->GetNumber());
  SetLDet(LDEntry->GetNumber());
  SetCSAVth(CSAVthEntry->GetNumber());
  SetShTrans(ShTransEntry->GetNumber());
  
  

	if(bfieldon==true) 
	  {
	    B = BfieldEntry->GetNumber();
	  }
	
	cout << " ========= Inputs to Simulation =================" << endl;	
	Temp = TempEntry->GetNumber();
	cout << "Temperature set to = "<< Temp << endl;		
	//cout<<"Flag gain ON = "<<GetGainon()<<endl;
	cout << "Upper detector dimension = "<<dwpot.GetYMAX()<<endl;
	int NBatch = 1.;

	dimMaxCarriers= 2*dwpot.Getmipcharge()+2000000;
	

	// dimMaxCarriers= 2*dwpot.Getmipcharge()+100000;


	int xRandomHit; 
	float MaxTWCSA = 0;
	float MaxTWBB = 0;
	//float NpairsMax = 0;

	float MinTWCSA = 1000;
	float MinTWBB = 1000;
	float Npairs  = 0;

	osccanvas->Divide(1,2,0.002,0.002);

	
	if (GetBatchOn() == true)
	{ 
	  //	      CurrentsProgressBar->SetMax(EventsEntry->GetNumber());
	  //   CurrentsProgressBar->Percent(0);
	      NBatch = EventsEntry->GetNumber();
	      GetEnhist()->Reset();
	}
	for (int e=1; e<= NBatch; e++)
	  {
	    carriers =new Carriers[dimMaxCarriers];
	    
	    cout << "Processing event " << e << endl;
	    xRandomHit = (CarriersInNumberentry->GetNumber() -(int)dwpot.Getpitch()/2) + (int)dwpot.Getpitch()*gRandom->Rndm(); //random hit x value for batch mode

	    for (int i=0;i<dimMaxCarriers;i++) carriers[i].Setinside(-1);	//initialize carriers array
	
	    if (GetBatchRandomOn() == true)
	      {
		carriersin = xRandomHit;
		hity = (dwpot.GetYMAX()-GetXRayRange()-ELECTRODE_DEPTH)*gRandom->Rndm()+GetXRayRange();
	      }
	    else 
	      {
		carriersin = CarriersInNumberentry->GetNumber();
		if (dwpot.GetYMAX() >EdgeNumberentry->GetNumber()) hity = EdgeNumberentry->GetNumber();
		else
		  {
		    //		      cout << "Y coordinate out of range, setting it to YMAX/2" << endl; 
		    hity = dwpot.GetYMAX()/2;
		  }
	      }
	    

	    //cout << "Status " << radiobuttonstatus<< endl; 
	    switch (radiobuttonstatus){

	    case MIPunif:
	      SetParticleType(1);
	      SetCalibFlag(0);
	      SetConstQFlag(1);
	      SetUniformQFlag(1);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(dwpot.GetYMAX()*75/cos(TMath::Pi()/180*GetAngle()));
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break;
	      
	    case MIPnonunif:
	      SetParticleType(2);
	      SetCalibFlag(0);
	      SetConstQFlag(1);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break; 
	      
	    case MIPlandau:
	      SetParticleType(3);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break;
	       
	    case ALPHA_TOP:
	      SetParticleType(4);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ALPHAENERGY/IONENERGY*GetAlphaRange()/RANGE_ALPHA);
	      CreateChargesAlphaTop(dwpot,carriers,carriersin,this);

	      break;
	      
	    case ALPHA_BOTTOM:
	      SetParticleType(5);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ALPHAENERGY/IONENERGY*GetAlphaRange()/RANGE_ALPHA);
	      CreateChargesAlphaBottom(dwpot,carriers,carriersin,this);
	      break;
	      
	    case USR_CHARGE: // laser Top
	      SetParticleType(6);
	      SetConstQFlag(1);
	      SetUniformQFlag(1);
	      SetUserUniformQFlag(1);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*dwpot.GetYMAX()/cos(TMath::Pi()/180*GetAngle()));//MODIFIED
	      CreateCharges(dwpot,carriers,carriersin,this);

	      break;

	    case CALIB:
	      SetParticleType(7);
	      SetCalibFlag(1);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      break;
	      
	    case EDGE:
	      SetParticleType(8);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(dwpot.GetXMAX()*LaserPairsPerMicron);
	      CreateChargesLaserSide(dwpot,carriers,hity,this);
	      
	      break;

	    case XRAY:
	      SetParticleType(9);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*1e3/IONENERGY);//MODIFIED
	      CreateChargesXRay(dwpot,carriers, carriersin,hity,this);
	      break;

	      
	    default: break;
	    }

	    if (GetParticleType() != 7) cout << "Particle hits at " << carriersin << endl;	    
	    SetLorentz(dwpot,carriers,B, Temp, CallGetDetType());
	    

	    
	    if (e%100 == 0)
	      {
		cout<<"Event Number: "<<e <<endl;
	      }
	    
	    CalculateCurrents(dwpot,df,wf,carriers,this,e);
	    delete[] carriers;

	    if(GetFileNameOn())
	      {
		std::stringstream ss;
		ss << " Event  #  "<< e << " " ;
		CalculatingLabel->SetTitle(ss.str().c_str()); // update progess label title	
		continue; //continue  loop if writing out 
	      }
	      if(Getstopped()) 
	      {
		Setstopped(false);
		break;
	      }
	    if (GetBatchOn() == true) 
	      {
		
		//		CurrentsProgressBar->Increment(1);
		std::stringstream ss;
		ss << " Event  #  "<< e << " " ;
		CalculatingLabel->SetTitle(ss.str().c_str()); // update progess label title	

		jhist->Fill(GetJitter());
		tvthhist->Fill(GetCSATVth());
		BBtvthhist->Fill(GetBBTVth());	     
		twtothist->Fill(GetCSAFTVth()-GetCSATVth(),GetCSATVth());
		if ( fabs(GetCSAFTVth()-GetCSATVth()) > MaxTWCSA)  MaxTWCSA = fabs(GetCSAFTVth()-GetCSATVth());
		if ( fabs(GetCSAFTVth()-GetCSATVth()) < MinTWCSA)  MinTWCSA = fabs(GetCSAFTVth()-GetCSATVth());
		twtothist->GetXaxis()->SetRangeUser(MinTWCSA*0.9, MaxTWCSA*1.1);
		  
		
		BBtwtothist->Fill(GetBBFTVth()-GetBBTVth(),GetBBTVth());
		if (fabs(GetBBFTVth()-GetBBTVth()) > MaxTWBB)  MaxTWBB = fabs(GetCSAFTVth()-GetCSATVth());
		if (fabs(GetBBFTVth()-GetBBTVth()) < MinTWBB)  MinTWBB = fabs(GetCSAFTVth()-GetCSATVth());
		BBtwtothist->GetXaxis()->SetRangeUser(0.9*MinTWBB, MaxTWBB*1.1);
		// cout << "histo limits = "<<MinTWBB << " " << MaxTWBB << endl;
		//		osccanvas->Clear();
		if (e == 1)
		  {
		    osccanvas->Clear();
		    osccanvas->Divide(1,2,0.002,0.002);
		  }
		osccanvas->cd(1);
		tvthhist->Draw();
		if (e%10 == 0)  	tvthhist->Fit("gaus");
		gStyle->SetOptFit(1);
		osccanvas->cd(2);
		BBtvthhist->Draw();
		if (e%10 == 0) BBtvthhist->Fit("gaus");
		gStyle->SetOptFit(1);

		osccanvas->Update();

		if (!GetShowCur())
		  {
		    Getcanvaspc()->Clear();
		    if (GetParticleType() <4 || GetParticleType() == 6 ||  GetParticleType() == 8)
		      {
			Getcanvaspc()->Divide(1,1,0.004,0.004);   
			Getcanvaspc()->cd(1);
			Npairs = GetNumPairs();

			// cout << "Npairs = " << GetNumPairs() << " " << NpairsMax << endl;
			if (e == 1)
			  {
			    //			    GetEnhist()->Reset();			
			    GetEnhist()->GetXaxis()->SetRangeUser(0.,  400);			
			    GetEnhist()->SetTitle("Average # of e/h per micron");
			    GetEnhist()->GetXaxis()->SetTitle("Average # of e/h per micron");
			    GetEnhist()->GetYaxis()->SetTitle("Events");			
			    gStyle->SetOptFit(1);
			    FitLandau = new TF1("FitLandau","landau",20.,300.);
			  }
			//	if ( GetParticleType() != 8  || GetParticleType() != 6)  	GetEnhist()->Fill(Npairs/dwpot.GetYMAX());

			GetEnhist()->Draw();

			if (e>20 && e%2 == 0 && GetParticleType()==3)  //	Enhist->Fit("landau","","SAME",0,400);
			  {
			    gStyle->SetOptFit(1); 
			    FitLandau->SetParameter(1,75.);
			    FitLandau->SetParameter(2,15.);
			    FitLandau->SetParLimits(1,30.,100.);
			    FitLandau->SetParLimits(2,10.,60.);			    
			    Enhist->Fit(FitLandau,"","SAME");
			  }

		      }
		    Getcanvaspc()->Update();
		    Getcurcanvas()->Update();
		    // return;
		      
		  }
	


		
	      }
	    if (GetBatchOn() == false) break; // exit from the batch loop if you de-select the check box
	  } // end of batch loop

	if (GetBatchOn() == true) cout<<"Run finished: "<< EventsEntry->GetNumber() <<" events"<<endl;
	
	//dwpot.DeleteDopdensity();

	//	CalculatingLabel->SetBackgroundColor(0x00ff00);
	// CalculatingLabel->SetTitle("Done.");
	//CalculatingLabel2->SetBackgroundColor(0x00ff00);
	//CalculatingLabel2->SetTitle("Done.");
	curcanvas->Update();
	osccanvas->Update();
	CurrentsInfoFrame->Layout();
	QELabel->Layout();
	CalculateButton->SetEnabled(kTRUE);
	CalculateButton->SetTextColor(1,kFALSE);
	CalcPotButton->SetEnabled(kTRUE);
	CalcPotButton->SetTextColor(1,kFALSE);
	SetButton->SetEnabled(kTRUE);
	SetButton->SetTextColor(1,kFALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallCalculateFields() {
	df = new Field*[(dwpot.GetYMAX())];
	for (int i = 0; i < (dwpot.GetYMAX()); i++) df[i] = new Field[(dwpot.GetXMAX())];
	
	wf = new Field*[(dwpot.GetYMAX())];
	for (int i = 0; i < (dwpot.GetYMAX()); i++) wf[i] = new Field[(dwpot.GetXMAX())];	
	CalculateFields(dwpot,wf,df);
	CalculateAbsFields(dwpot,wf,df);
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPlotFieldHist(Bool_t onoff) {
	fieldyes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAbsCur(Bool_t onoff) {
	abscuryes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShowCur(Bool_t onoff) {
	showcuryes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetShowCur() {
	return showcuryes;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetB() {
	return B;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetQLabel(const char* qetext,const char* qhtext,const char* qehtext,const char* qegtext,const char* qhgtext,const char* qehgtext,const char* qetottext,const char* qhtottext,const char* qtottext) {
	QELabel->SetTitle(qetext);
	QHLabel->SetTitle(qhtext);
	QEHLabel->SetTitle(qehtext);
	QEGLabel->SetTitle(qegtext);
	QHGLabel->SetTitle(qhgtext);
	QEHGLabel->SetTitle(qehgtext);
	QETotLabel->SetTitle(qetottext);
	QHTotLabel->SetTitle(qhtottext);
	QTotLabel->SetTitle(qtottext);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAngleLabel(const char* ethetatext, const char* hthetatext) {
	LorentzeLabel->SetTitle(ethetatext);
	LorentzhLabel->SetTitle(hthetatext);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbs(int LCol = 1) {
  // return;

	wherecut = WhereCut->GetNumber();
	//	cout << "Drawing FieldsAbs" << endl;

	double* q1;
	q1 = new double[dwpot.GetYMAX()];
	for(int k=0;k<dwpot.GetYMAX();k++) q1[k]=k; //(int)(pow(2,pot.Getref()));

	double *dabs; // Absolute value of drift field
	double *wabs; // absolute value of weighting field
	dabs = new double[dwpot.GetYMAX()];		
	wabs = new double[dwpot.GetYMAX()];		 				
	
	for(int i=0; i<dwpot.GetYMAX(); i++) {
		wabs[i]=0.0;
		dabs[i]=0.0;
	}		

	double Maxdabs = 0;
	double Mindabs = 100;
	double Maxwabs = 0;

	for(int k=0;k<dwpot.GetYMAX();k++) 
	  {
	    //	    wabs[k] = (wf[k][wherecut].Getabs() > 0.1)? wf[k][wherecut].Getabs() : 0. ;
	    //dabs[k] = (df[k][wherecut].Getabs()/1e5> 0.1)? df[k][wherecut].Getabs()/1e5 : 0;

	    wabs[k] = wf[k][wherecut].Getabs();
	    dabs[k] = df[k][wherecut].Getabs()/1e5;

	    if (Maxwabs < wabs[k]) Maxwabs = wabs[k];
	    if (Maxdabs < dabs[k]) Maxdabs = dabs[k];
	    if (dabs[k]<Mindabs) Mindabs = dabs[k];
	  }

	if (Mindabs == 0) cout << " Drift Field Amplitude = 0 !! Decrease DJ Current or Increase Bias" << endl;
	
	// cout << Maxdabs << endl;

	dfieldcut = new TGraph(dwpot.GetYMAX(),q1,dabs);
   	dfieldcut->GetXaxis()->SetTitle("y [um]");
   	dfieldcut->GetXaxis()->SetNdivisions(5);
	dfieldcut->SetTitle("Drift Field Amplitude |E| (kV/cm) ");
   	dfieldcut->GetYaxis()->SetLabelSize(0.05);
   	dfieldcut->GetYaxis()->SetLabelOffset(.01);
   	dfieldcut->GetXaxis()->SetLabelSize(0.05);
   	dfieldcut->GetXaxis()->SetLabelOffset(.01);
   	dfieldcut->GetXaxis()->SetTitleSize(0.05);
   	dfieldcut->GetYaxis()->SetRangeUser(0.0,Maxdabs*1.3);
   	dfieldcut->SetLineWidth(3);


	canvaspc->cd(2);
	dfieldcut->SetLineColor(LCol); // set line color to black
		if (GetGainon()) 
	  {
	    gPad->SetLogy(1);
	    dfieldcut->GetYaxis()->SetRangeUser(0.1,Maxdabs*2);
	  }
	dfieldcut->Draw();
	canvaspc->Update();

	wfieldcut = new TGraph(dwpot.GetYMAX(),q1,wabs);
   	wfieldcut->GetXaxis()->SetTitle("y (um)");
   	wfieldcut->GetXaxis()->SetNdivisions(5);
	wfieldcut->SetTitle("Weighting Field Ew (1/m) ");
   	wfieldcut->GetYaxis()->SetLabelSize(0.05);
   	wfieldcut->GetYaxis()->SetLabelOffset(.01);
   	wfieldcut->GetXaxis()->SetLabelSize(0.05);
   	wfieldcut->GetXaxis()->SetLabelOffset(.01);
   	wfieldcut->GetXaxis()->SetTitleSize(0.05);
   	wfieldcut->GetYaxis()->SetRangeUser(0.0,Maxwabs*1.3);
   	wfieldcut->SetLineWidth(3);
	//	canvaswf->Divide(1,1);
	//	canvaswf->cd();
	//	canvaswf->Clear(); //Nicolo1
	//	canvaswf->Clear(); //Nicolo1
	//	wfieldcut->Draw("ACP");
	//	gPad->SetLeftMargin(0.35);
	canvaswc->cd(2);
	wfieldcut->SetLineColor(LCol); // set line color to black
	wfieldcut->Draw("AL");
	wfieldcut->GetYaxis()->SetLimits(0., Maxwabs*1.2);
	canvaswc->Update();

	delete[] q1;
	delete[] dabs;
	delete[] wabs;
	
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbsX() {
  // return;
	wherecut = WhereCut->GetNumber();
	wherecut2 = WhereCut2->GetNumber();

	double* q2;
	q2 = new double[dwpot.GetYMAX()];
	for(int k=0;k<dwpot.GetYMAX();k++) q2[k]=k;

	double *dxabs; // Absolute value of drift field
	dxabs = new double[dwpot.GetYMAX()];	
	double *wxabs; // Absolute value of weighting field
	wxabs = new double[dwpot.GetYMAX()];	

	for(int i=0; i<dwpot.GetYMAX(); i++) {
		wxabs[i]=0.0;
		dxabs[i]=0.0;
	}			


	//	for(int k=0;k<dwpot.GetYMAX();k++)  dxabs[k] = (fabs(df[k][wherecut].GetFieldx())/1e5 > 0.1)? fabs(df[k][wherecut].GetFieldx()/1e5)  : 0. ;
	for(int k=0;k<dwpot.GetYMAX();k++)  dxabs[k] = df[k][wherecut].GetFieldx()/1e5;
	for(int k=0;k<dwpot.GetYMAX();k++)  wxabs[k] = ( fabs(wf[k][wherecut].GetFieldx())  > 0.1) ?  fabs(wf[k][wherecut].GetFieldx()) : 0;

	dfieldxcut = new TGraph(dwpot.GetYMAX(),q2,dxabs);
   	dfieldxcut->GetXaxis()->SetTitle("y [um]");
   	dfieldxcut->GetXaxis()->SetNdivisions(5);
	dfieldxcut->SetTitle("Drift Field Ex (kV/cm) ");

   	dfieldxcut->GetYaxis()->SetLabelSize(0.05);
   	dfieldxcut->GetYaxis()->SetLabelOffset(.01);
   	dfieldxcut->GetXaxis()->SetLabelSize(0.05);
   	dfieldxcut->GetXaxis()->SetLabelOffset(.01);
   	dfieldxcut->GetXaxis()->SetTitleSize(0.05);
   	dfieldxcut->SetLineWidth(3);

	//	canvaspf->Divide(1,1,0.002,0.002);
	//	canvaspf->cd();
	//canvaspf->Clear()
	// canvaspf->Update();

	canvaspc->cd(2);
	gPad->SetLogy(0);
	dfieldxcut->Draw("AL");
	canvaspc->Update();

	wfieldxcut = new TGraph(dwpot.GetYMAX(),q2,wxabs);
   	wfieldxcut->GetXaxis()->SetTitle("y (um)");
   	wfieldxcut->GetXaxis()->SetNdivisions(5);
	wfieldxcut->SetTitle("Weighting Field Ex (1/m) ");
   	wfieldxcut->GetYaxis()->SetLabelSize(0.05);
   	wfieldxcut->GetYaxis()->SetLabelOffset(.01);
   	wfieldxcut->GetXaxis()->SetLabelSize(0.05);
   	wfieldxcut->GetXaxis()->SetLabelOffset(.01);
   	wfieldxcut->GetXaxis()->SetTitleSize(0.05);
   	wfieldxcut->SetLineWidth(3);

	canvaswc->cd(2);
	wfieldxcut->Draw("AL");
	canvaswc->Update();
	delete[] q2;
	delete[] dxabs;
	delete[] wxabs;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbsY() {
  // return;
	wherecut = WhereCut->GetNumber();
	//	wherecut = wherecut+dwpot.GetXMAX()/2+1;
	wherecut2 = WhereCut2->GetNumber();
	// wherecut2 = wherecut2+dwpot.GetXMAX()/2+1;
	double MaxEy = 0;
	double MinEy = 0;
	
	double* q3;
	q3 = new double[dwpot.GetYMAX()];
	for(int k=0;k<dwpot.GetYMAX();k++) q3[k]=k;

	double *dyabs; // Absolute value of drift field in y direction
	dyabs = new double[dwpot.GetYMAX()];					
	double *wyabs; // Absolute value of weighting field in x direction
	wyabs = new double[dwpot.GetYMAX()];	

	for(int i=0; i<dwpot.GetYMAX(); i++) { // initialize
		wyabs[i]=0.0;
		dyabs[i]=0.0;
	}		
	//	for(int k=0;k<dwpot.GetYMAX();k++) dyabs[k] = (fabs(df[k][wherecut].GetFieldy())/1e5 > 0.1)? fabs(df[k][wherecut].GetFieldy()/1e5)  : 0. ;
	for(int k=0;k<dwpot.GetYMAX();k++)
	  {
	    dyabs[k] = df[k][wherecut].GetFieldy()/1e5 ;
	    if (dyabs[k] >MaxEy) MaxEy = dyabs[k];
	    if (dyabs[k] <MinEy) MinEy = dyabs[k];
	  }
	
	// dyabs[k] = fabs(df[k][wherecut].GetFieldy()/1e5);		
	for(int k=0;k<dwpot.GetYMAX();k++)  wyabs[k] = ( fabs(wf[k][wherecut].GetFieldy())  > 0.1) ?  fabs(wf[k][wherecut].GetFieldy()) : 0;
	// wyabs[k] = fabs(wf[k][wherecut].GetFieldy()); 



	dfieldycut = new TGraph(dwpot.GetYMAX(),q3,dyabs); 
   	dfieldycut->GetXaxis()->SetTitle("y (um)");
   	dfieldycut->GetXaxis()->SetNdivisions(5);
	dfieldycut->SetTitle("Drift Field Ey (kV/cm) ");

   	dfieldycut->GetYaxis()->SetLabelSize(0.05);
   	dfieldycut->GetYaxis()->SetLabelOffset(.01);
   	dfieldycut->GetXaxis()->SetLabelSize(0.05);
   	dfieldycut->GetXaxis()->SetLabelOffset(.01);
   	dfieldycut->GetXaxis()->SetTitleSize(0.05);
   	dfieldycut->SetLineWidth(3);

	canvaspc->cd(2);
	gPad->SetLogy(0);
	dfieldycut->Draw("AL");
	canvaspc->Update();

       
	wfieldycut = new TGraph(dwpot.GetYMAX(),q3,wyabs);
   	wfieldycut->GetXaxis()->SetTitle("y (um)");
   	wfieldycut->GetXaxis()->SetNdivisions(5);
	wfieldycut->SetTitle("Weighting Field Ey (1/m) ");
   	wfieldycut->GetYaxis()->SetLabelSize(0.05);
   	wfieldycut->GetYaxis()->SetLabelOffset(.01);
   	wfieldycut->GetXaxis()->SetLabelSize(0.05);
   	wfieldycut->GetXaxis()->SetLabelOffset(.01);
   	wfieldycut->GetXaxis()->SetTitleSize(0.05);
   	wfieldycut->SetLineWidth(3);
	//	canvaswf->Divide(1,1);
	//	canvaswf->cd();
	// canvaswf->Clear();  //Nicolo1
	//	wfieldycut->Draw("ACP");
	canvaswc->cd(2);
	wfieldycut->Draw("AL");
	canvaswc->Update();

	if (MaxEy*MinEy <0) cout << " Drift Field positive and negative!! Decrease Current or Increase Bias" << endl;

	delete[] q3;
	delete[] dyabs;
	delete[] wyabs;
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldHist() {
  if (GetLess2DPlot()) return;
  
	int p=1;
	int a=0;
	int h=0;
	int binsx=dwpot.GetXMAX(); // binsx = number of bins on x-axis
	int binsy=dwpot.GetYMAX(); // binsy = number of bins on y-axis
	dfhist->Reset();	 //dfhist = histogram of drift field 

	
	for(int i=0;i<(int) binsx;i++) {
	  h=  i;
	  // cout << h << " : " ;
	  for(int j=0;j< (int) binsy;j++) 
	    {
	      a= j;
	      dfhist->SetBinContent(i,j,df[a][h].Getabs()/1e5);	    	
	    }
	  
	}
	
	dfhist->SetStats(0);	
	canvasp->cd();
	dwpot.DriftPal();
	dfhist->Draw("SAMES" "CONT2"); //draw on same canvas as dhist and use arrows	
	canvasp->Update();
	return;

	//whist->Draw("COLZ");
	wfhist->Reset();	//wfhist = histogram of weighting field
	wfhist->TH2F::SetBins((binsx/p+1), 0,(binsx/p+1) ,(binsy/p+1),0.,(binsy/p+1) );
	//	canvasw->Divide(1,1,0.002,0.002);
	canvasw->cd();
					
	for(int i=0;i<binsx/p+1;i++) {
		for(int j=0;j<binsy/p+1;j++) {
			h=i*p;
			a=j*p;
			wfhist->SetBinContent(i+1,j+1,dwpot.Getwpot(a,h));
		}
	}
	wfhist->SetStats(0);
	dwpot.WeightPal();
	wfhist->Draw("SAMES" "ARR");	
	canvasw->Update();				
	// return;


}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getdiffusionon() {
	return diffusionon;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getabscuron() {
	return abscuryes;
}

/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::SetStopOn(){
      if (stopped==false) {
      stopped=true;
      return 0;
      }
      
     else return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getstopped() {
	return stopped;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::Setstopped(Bool_t x) {
	stopped=x;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGLabel * WFGUI::GetCalculatingLabel() {
	return CalculatingLabel;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGLabel * WFGUI::GetCalculatingLabel2() {
	return CalculatingLabel2;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPlotUpdate(Bool_t onoff) {
	plotupdate=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getplotupdate() {
	return plotupdate;
}	
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLess2DPlot(Bool_t onoff) {
	Less2DPlot=onoff;
	SetLessPlot(onoff);
	cout << GetLess2DPlot() << " get less 2D " << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetLess2DPlot() {
	return Less2DPlot;
}	

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLessPlot(Bool_t onoff) {
	LessPlot=onoff;
	if (onoff)
	  {
	    OnStripsButton2->SetEnabled(kFALSE);
	    BetweenStripsButton2->SetEnabled(kFALSE);		
	    ExButton2->SetEnabled(kFALSE);				
	    EyButton2->SetEnabled(kFALSE);				
	    DrawCutsUserEntry2->SetEnabled(kFALSE);
	    OnStripsButton->SetEnabled(kFALSE);
	    BetweenStripsButton->SetEnabled(kFALSE);		
	    ExButton->SetEnabled(kFALSE);				
	    EyButton->SetEnabled(kFALSE);		
	    DrawCutsUserEntry->SetEnabled(kFALSE);		
	  }
	else if(!GetLess2DPlot())
	  {
	    OnStripsButton2->SetEnabled(kTRUE);
	    BetweenStripsButton2->SetEnabled(kTRUE);		
	    ExButton2->SetEnabled(kTRUE);				
	    EyButton2->SetEnabled(kTRUE);				
	    DrawCutsUserEntry2->SetEnabled(kTRUE);
	    OnStripsButton->SetEnabled(kTRUE);
	    BetweenStripsButton->SetEnabled(kTRUE);		
	    ExButton->SetEnabled(kTRUE);				
	    EyButton->SetEnabled(kTRUE);	
	    DrawCutsUserEntry->SetEnabled(kTRUE);			
	  }

}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetLessPlot() {
	return LessPlot;
}	
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetYMax(){
  return  YMAXentry->GetNumber()*0.95;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetXMax(){
  int TempXMAX = XMAXentry->GetNumber();
  if (TempXMAX%2==0) 
    {
       TempXMAX++;
       //cout << "Number of strips increased to be an  odd number " << endl;
    }
  return TempXMAX;
}

////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetChargeEntry(){
    return ParticleSpecificsEntry->GetNumber();//MODIFIEDChargeentry->GetNumber();
}

////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCalibEntry(){
    return ParticleSpecificsEntry->GetNumber();//MODIFIEDCalibentry->GetNumber();
}

////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallBoundaryConditions() {

  CallSetDoping0();
  CallSetDoping1();

  XMAXentry->GetNumber();
  YMAXentry->GetNumber();
  Pitchentry->GetNumber();
  Widthentry->GetNumber();
  TempEntry->GetNumber();
  SetSWidth(Widthentry->GetNumber());
	if(PotentialThread==0)
	{	
		
    	  dwpot.SetPitchWidthXY(GetYMax(),GetXMax(),Pitchentry->GetNumber(),Widthentry->GetNumber());      // set pitch, width, XMAX and YMAX
		dwpot.SetV(Biasentry->GetNumber(),Depletionentry->GetNumber());  		// set depletion and bias voltage
		dwpot.SetDoping(stripdoping,bulkdoping);

		for(int i=0; i<dwpot.GetXMAX(); i++) {			// reset potentials to zero as a precaution
			for(int j=0; j<dwpot.GetYMAX(); j++) {
				dwpot.SetwPotential(j,i,0.0);
				dwpot.SetdPotential(j,i,0.0);
			}
		}
		// set histogram bins:

		dhist->Reset(); // Reset histograms before filling them again
		whist->Reset();

		dhist->TH2F::SetBins( dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		whist->TH2F::SetBins(dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );				     
		dfhist->TH2F::SetBins(dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );				      
		wfhist->TH2F::SetBins(dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		chist->TH2F::SetBins( dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		chhist->TH2F::SetBins( dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		ctothist->TH2F::SetBins( dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		Exhist->TH2F::SetBins( dwpot.GetXMAX(),0, dwpot.GetXMAX(), dwpot.GetYMAX()+2,0, dwpot.GetYMAX()+2 );
		//cout <<   dwpot.GetXMAX() << " " << dwpot.GetYMAX()	<< endl;	      
		WhereCut->SetNumber(dwpot.GetXMAX()/2+1);
		WhereCut2->SetNumber(dwpot.GetXMAX()/2+1);

		dwpot.SetBoundaryConditions(ReadOutTopFlag); // set boundary conditions	

		for(int i=0;i<dwpot.GetXMAX();i++) { // fill histogram
			for(int j=0;j<dwpot.GetYMAX();j++)
			  {
			    dhist->SetBinContent(i+1,j+1,dwpot.Getdpot(j,i));
			    whist->SetBinContent(i+1,j+1,dwpot.Getwpot(j,i));
			  }
		}
		CarriersInNumberentry->SetNumber(dwpot.GetXMAX()/2);

		OnStripsButton2->SetEnabled(kFALSE);
		BetweenStripsButton2->SetEnabled(kFALSE);				
		ExButton2->SetEnabled(kFALSE);				
		EyButton2->SetEnabled(kFALSE);				
		DrawCutsUserEntry2->SetEnabled(kFALSE);
		OnStripsButton->SetEnabled(kFALSE);
		BetweenStripsButton->SetEnabled(kFALSE);		
		ExButton->SetEnabled(kFALSE);				
		EyButton->SetEnabled(kFALSE);		
		DrawCutsUserEntry->SetEnabled(kFALSE);		
		DrawAllGraph(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallSetDetType() {
  if (TypeButton[0]->IsOn())
    {
	DetType = 0;
    }
  else if (TypeButton[1]->IsOn()) 
    {
	DetType= 1;
    }
  else if (TypeButton[2]->IsOn()) 
    {
      	DetType =  2;
    }
  return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDetType() {
  return DetType;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDoping0() {	
  //set bulk doping
	if(BulkButton[0]->IsOn())  bulkdoping = NTYPE;	
	else bulkdoping = PTYPE;
	CalculateButton->SetEnabled(kFALSE);	
	//	cout << "CallSetDoping0 bulkdoping " << bulkdoping << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDJType() {	
  if (DJButton[0]->IsOn()) DJvalue = 0;
  if (DJButton[1]->IsOn()) DJvalue = 1;

  if (DJvalue == 0 ) cout << "Linear doping density for the Double Junction computation  " << endl;
  else  cout << "Step doping density for the Double Junction computation  " << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDJType() {	
  return DJvalue;
	//	cout << "CallSetDoping0 bulkdoping " << bulkdoping << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDoping0() {	
  //set bulk doping
	return bulkdoping;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetReadOut() {	
  //set bulk doping
	if(ReadOutButton[0]->IsOn())  ReadOutTopFlag = true;	
	else ReadOutTopFlag = false;
	CalculateButton->SetEnabled(kFALSE);	
	
	cout << "Needs to compute the potential again!! " << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::CallSetDoping1() {			//set strip doping
	if(StripButton[0]->IsOn()) 
	  {
	    stripdoping = NTYPE;
	  }
	else 
	  {
	    stripdoping = PTYPE;
	  }
	CalculateButton->SetEnabled(kFALSE);	
}

/////////////////////////////////////////////////////////////////////////////////////////////

int WFGUI::CallSetPart(Int_t id) {

  //Edge always false unless is edge //
  EdgeNumberentry -> SetState(kFALSE);
  NumberEntry -> SetState(kFALSE);
  EdgeLabel ->Disable(kTRUE);
  CarriersInNumberentry->SetState(kTRUE);
  CarriersAngleNumberentry->SetState(kTRUE);
  BatchRandomButton ->SetEnabled(kTRUE);
  ParticleSpecificsLabel1->Disable(kTRUE);
  ParticleSpecificsLabel2->Disable(kTRUE);
  ParticleSpecificsLabel3->Disable(kTRUE);
  ParticleSpecificsLabel4->Disable(kTRUE);
  
     if (id == 1) {
         radiobuttonstatus=MIPunif;
         ParticleSpecificsEntry->SetNumber(0);
         ParticleSpecificsEntry->SetState(kFALSE);
         NumberEntry->SetState(kTRUE);
         NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP uniform distributed" << endl;
         return 0;
     }
    
     if (id == 2) {
         radiobuttonstatus=MIPnonunif;
         ParticleSpecificsEntry->SetNumber(0);
         ParticleSpecificsEntry->SetState(kFALSE);
         NumberEntry->SetState(kTRUE);
         NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP non uniform distributed with fixed total amplitude" << endl;
         return 1;
     }
    
     if (id == 3) {
         radiobuttonstatus=MIPlandau;
         ParticleSpecificsEntry->SetNumber(0);
         ParticleSpecificsEntry->SetState(kFALSE);
         NumberEntry->SetState(kTRUE);
         NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP Landau distributed" << endl;
         return 2;
     }
    
    if (id == 4) {
        radiobuttonstatus=USR_CHARGE;
        ParticleSpecificsLabel1->Disable(kFALSE);
        ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	ParticleSpecificsEntry->SetNumber(75);
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kTRUE);
        NumberLabel->Disable(kFALSE);
        cout << "Selecting Laser (1064 nm)" << endl;
        return 3;
    }
    
    if (id == 5 || id == 6) {
        int top_or_bottom = 4;

        if (id == 5) {
            radiobuttonstatus=ALPHA_TOP;
            cout << "Selecting Alpha from top" << endl;
            top_or_bottom = 4;
        }
        if (id == 6) {
            radiobuttonstatus=ALPHA_BOTTOM;
            cout << "Selecting Alpha from bottom" << endl;
            top_or_bottom = 5;
        }

	ParticleSpecificsLabel2->Disable(kFALSE);
	 //        ParticleSpecificsLabel2->SetTextColor(gROOT->GetColor(1), kFALSE);

        ParticleSpecificsEntry->SetNumber(10);
	
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kFALSE);
        NumberLabel->Disable(kTRUE);
        return top_or_bottom;
    }
 
    if (id == 7) {
        radiobuttonstatus= CALIB;
	BatchRandomButton ->SetEnabled(kFALSE);
	CarriersInLabel ->Disable(kTRUE);
	CarriersInNumberentry->SetState(kFALSE);
	CarriersAngleNumberentry->SetState(kFALSE);
        ParticleSpecificsLabel3->Disable(kFALSE);
        ParticleSpecificsLabel3->SetTextColor(gROOT->GetColor(1), kFALSE);
        ParticleSpecificsEntry->SetNumber(4);
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kFALSE);
        NumberLabel->Disable(kTRUE);
        cout << "Selecting Electronic Pulse"<< endl;
        return 6;
    }

         if (id == 8) {
        radiobuttonstatus=EDGE;
	BatchRandomButton ->SetEnabled(kFALSE);
	CarriersInLabel ->Disable(kTRUE);
	CarriersInNumberentry->SetState(kFALSE);
	CarriersAngleNumberentry->SetState(kFALSE);
	ParticleSpecificsEntry->SetNumber(75);
        ParticleSpecificsEntry->SetState(kTRUE);
        ParticleSpecificsLabel1->Disable(kFALSE);
        ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	ParticleSpecificsEntry->SetNumber(75);	
	EdgeNumberentry -> SetState(kTRUE);
	EdgeLabel ->Disable(kFALSE);

        cout << "Selecting Edge Laser"<< endl;
        return 8;



	 }
         if (id == 9) {
        radiobuttonstatus=XRAY;
	BatchRandomButton ->SetEnabled(kTRUE);
	CarriersInNumberentry->SetState(kTRUE);
	CarriersAngleNumberentry->SetState(kTRUE);
        ParticleSpecificsLabel4->Disable(kFALSE);
	ParticleSpecificsEntry->SetNumber(10);
        ParticleSpecificsEntry->SetState(kTRUE);
	NumberEntry->SetState(kTRUE);
	NumberLabel->Disable(kFALSE);
	EdgeNumberentry -> SetState(kTRUE);
	EdgeLabel ->Disable(kFALSE);
        cout << "Selecting X-Ray"<< endl;
	cout << " For the absorption probability see: http://web-docs.gsi.de/~stoe_exp/web_programs/x_ray_absorption/index.php" << endl;
        return 9;
    }
 
	
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::FillHist()
{
	canvasp->Clear();
	canvasw->Clear();
	dhist->Reset();	//Reset histogram
	dhist->TH2F::SetBins( dwpot.GetXMAX() ,0., dwpot.GetXMAX(),dwpot.GetYMAX(), 0, dwpot.GetYMAX() );
	dhist->GetXaxis()->SetLabelColor(1);	
	dhist->GetYaxis()->SetLabelColor(1);					
	dhist->SetStats(0);
	
	whist->Reset();	//Reset histogram
	whist->TH2F::SetBins( dwpot.GetXMAX() ,0., dwpot.GetXMAX(),dwpot.GetYMAX(), 0, dwpot.GetYMAX() );
	whist->GetXaxis()->SetLabelColor(1);	
	whist->GetYaxis()->SetLabelColor(1);					
	whist->SetStats(0);
		
	for(int i=0;i<dwpot.GetXMAX();i++) {		// fill histogram
		for(int j=0;j<dwpot.GetYMAX();j++) {
			dhist->SetBinContent(i+1,j+1,dwpot.Getdpot(j,i));
		}
	}
	for(int i=0;i<dwpot.GetXMAX();i++) {		// fill histogram
		for(int j=0;j<dwpot.GetYMAX();j++) {
			whist->SetBinContent(i+1,j+1,dwpot.Getwpot(j,i));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAllGraph(int LCol = 1)
{
  //  cout << " DrawAllGraph 2DPlot " << GetLess2DPlot() << " LCol= " <<LCol << " GetLessPlot " << GetLessPlot()<< endl;


  if(GetFileNameOn()) return;
  
  wherecut = WhereCut->GetNumber(); 
  wherecut2 = WhereCut2->GetNumber(); 

  //  cout << wherecut << " wherecut" <<  " " << "LCol = " << LCol << endl;

  


  canvasp->Clear();
  canvasw->Clear();
  canvasp->Update(); 
  canvasw->Update();     
  
  
  NLine1 = new TLine();
  NLine2 = new TLine();
  NLine1->SetLineStyle(2);
  NLine2->SetLineStyle(2);
  
  int bx1 = dwpot.GetXMAX()/2-dwpot.Getpitch() - GetSWidth()/2;
  int bx2 = dwpot.GetXMAX()/2-dwpot.Getpitch() + GetSWidth()/2;
  
  int bx3 = dwpot.GetXMAX()/2- GetSWidth()/2;
  int bx4 = dwpot.GetXMAX()/2+ GetSWidth()/2;
  
  int bx5 = dwpot.GetXMAX()/2+ dwpot.Getpitch() - GetSWidth()/2;
  int bx6 = dwpot.GetXMAX()/2+ dwpot.Getpitch() + GetSWidth()/2;
  
  TBox *bb1 = new TBox( bx1 , dwpot.GetYMAX(), bx2,dwpot.GetYMAX()+3);
  TBox *bb2 = new TBox( bx3 , dwpot.GetYMAX(), bx4,dwpot.GetYMAX()+3);
  TBox *bb3 = new TBox( bx5 , dwpot.GetYMAX(), bx6,dwpot.GetYMAX()+3);
  
  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);
  
  canvasp->cd();
  dwpot.DriftPal();
  dhist->SetStats(0); // hide statistics box
  whist->SetStats(0);
  dhist->Draw("COLZ");
  bb2->Draw();
  
  if (GetXMax()>=2)
    {
      bb1->Draw();
      bb3->Draw();
    }
  canvasp->Update();
  
  
  canvasw->cd();
  dwpot.DriftPal();
  whist->Draw("COLZ");
  canvasw->Update();
  canvasp->cd();

  // memory clicking ON strips below  this line
  
  if (LCol == -1)
    {
      delete NLine1;
      delete NLine2;
      delete bb1;
      delete bb2;
      delete bb3;
      
      return;
    }


  //  cout << "Line1" << endl;
  
  NLine1->DrawLine(wherecut,0,wherecut,dwpot.GetYMAX()-2);
  if(fieldyes==true) DrawFieldHist(); // Nicolo1
  canvasp->Update(); 
  
  
  
  canvasw->cd();
  dwpot.DriftPal();
  whist->Draw("COLZ");
  
  // cout << "Line2" << endl;
  NLine2->DrawLine(wherecut2,0,wherecut2,dwpot.GetYMAX()-2);
  canvasw->Update(); 


  if (GetLessPlot() || LCol == 0 || GetFileNameOn() ) return;

  ///nicolo The comand "Divide" causes memory leaks..."
  canvaswc->Clear();
  // gPad->Divide(2,1);
   canvaswc->Divide(2,1,0.002,0.002);
  canvaswc->Update();
  canvaspc->Clear();
  // gPad->Divide(2,1);
   canvaspc->Divide(2,2,0.004,0.004);
  canvaspc->Update();
  
  
  DrawCutGraph(LCol);
  DrawDopDensGraph(LCol);
  DrawFieldsAbs(LCol);
  
  cout << "===== Finished DrawAllGraph ======" << endl;

  delete NLine1;
  delete NLine2;
  delete bb1;
  delete bb2;
  delete bb3;
 
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawCutGraph(int LCol)
{
  // return;
	wherecut=WhereCut->GetNumber();
	wherecut2=WhereCut2->GetNumber();
	cout << " " << endl;
	cout << "Plotting the field cutting in x  at " << wherecut << endl;
	double* q=0;
	q = new double[dwpot.GetYMAX()];
	for(int k=0;k<dwpot.GetYMAX();k++) q[k]=k;
	double *dpoty=0;
	float dMax = 0.;
	float dMin = 0.;
	dpoty = new double[dwpot.GetYMAX()];					
	for(int k=0;k<dwpot.GetYMAX();k++) 
	  {
	    dpoty[k] = dwpot.Getdpot(k,wherecut);
	    if (dpoty[k]> dMax) dMax = dpoty[k];
	    if (dpoty[k]< dMin) dMin = dpoty[k];
	  }
	TGraph driftcut2(dwpot.GetYMAX(),q,dpoty);
	
	driftcut= new TGraph(dwpot.GetYMAX(),q,dpoty);
	driftcut->GetXaxis()->SetTitle("y [um]");
   	driftcut->GetXaxis()->SetNdivisions(5);
	driftcut->SetTitle(" Drift Potential V [V]");
	driftcut->GetXaxis()->SetLabelColor(1);
	driftcut->GetYaxis()->SetLabelColor(1);
  	driftcut->GetYaxis()->SetLabelSize(0.05);
   	driftcut->GetYaxis()->SetLabelOffset(.01);
   	driftcut->GetXaxis()->SetLabelSize(0.05);
   	driftcut->GetXaxis()->SetLabelOffset(.01);
   	driftcut->GetXaxis()->SetTitleSize(0.05);
   	driftcut->GetYaxis()->SetRangeUser(dMin*1.3,dMax*1.3);
	
   	driftcut->SetLineWidth(3);
	driftcut->SetLineColor(LCol); // set line color to black

	canvaspc->cd(1);
	driftcut->Draw("AL");
	canvaspc->Update();

	//	cout << " Done Drift" << endl;

	double *wpoty=0;
	float wMax = 0.;
	wpoty = new double[dwpot.GetYMAX()];					
	for(int k=0;k<dwpot.GetYMAX();k++) 
	  {
	  wpoty[k] = dwpot.Getwpot(k,wherecut);
	  if (wpoty[k] > wMax) wMax = wpoty[k];
	  }

	weightcut = new TGraph(dwpot.GetYMAX(),q,wpoty);
	weightcut->GetXaxis()->SetTitle("y (um)");
   	weightcut->GetXaxis()->SetNdivisions(5);
	weightcut->SetTitle("Weighting Potential  ");	
	weightcut->GetXaxis()->SetLabelColor(1);
	weightcut->GetYaxis()->SetLabelColor(1);
   	weightcut->GetYaxis()->SetLabelSize(0.05);
   	weightcut->GetYaxis()->SetLabelOffset(.01);
   	weightcut->GetXaxis()->SetLabelSize(0.05);
   	weightcut->GetXaxis()->SetLabelOffset(.01);
   	weightcut->GetXaxis()->SetTitleSize(0.05);
   	weightcut->GetYaxis()->SetRangeUser(0.0,wMax*1.3);
   	weightcut->SetLineWidth(3);
	weightcut->SetLineColor(LCol); // set line color to black
	//	canvaswc->Divide(1,1,0.002,0.002);
	canvaswc->cd(1);
	weightcut->Draw("AL");
	canvaswc->Update();

	//	cout << " Done Weighting" << endl;

	delete[] q;
	delete[] dpoty;
	delete[] wpoty;
	//	delete driftcut;
	// delete weightcut;

	
}
////////////////////////////////////////////
void WFGUI::DrawDopDensGraph(int LCol)
{

	double* q=0;
	q = new double[dwpot.GetYMAX()];
	for(int k=0;k<dwpot.GetYMAX();k++) q[k]=k;
       	double *dpoty=0;
	float dMax = 0.;
	float dMin = 0.;
	float YMax = 0.;
	float YMin = 0.;
	dpoty = new double[dwpot.GetYMAX()];
	//dopdensity = eN/epsilon	
	for(int k=0;k<dwpot.GetYMAX();k++) 
	  {
	    
	    dpoty[k] = dwpot.Getdopdensity(k)/ECHARGE*1e-4*1e-12; // charge density in unit of N/cm3 10^12
	    //	    cout << " k = " << k << " : " << dpoty[k] << endl;
	    if (fabs(dpoty[k]) != 0 ) YMax = k;
	    if (fabs(dpoty[k]) == 0 ) YMin = k;	    
	    if (fabs(dpoty[k])> dMax) dMax = fabs(dpoty[k]);
	    if (dpoty[k]< dMin) dMin = dpoty[k];
	  }
	//	TGraph dopdens(dwpot.GetYMAX(),q,dpoty);
	
	dopdens= new TGraph(dwpot.GetYMAX()-4,q,dpoty);
	dopdens->GetXaxis()->SetTitle("y [um]");
   	dopdens->GetXaxis()->SetNdivisions(5);
	dopdens->SetTitle(" Eff. Doping e(N_{D}f_{D}-N_{A}f_{A})+rho_{Bias}  [n/cm^{3}*10^{12}]");
	dopdens->GetXaxis()->SetLabelColor(1);
	dopdens->GetYaxis()->SetLabelColor(1);
  	dopdens->GetYaxis()->SetLabelSize(0.05);
   	dopdens->GetYaxis()->SetLabelOffset(.01);
   	dopdens->GetXaxis()->SetLabelSize(0.05);
   	dopdens->GetXaxis()->SetLabelOffset(.01);
   	dopdens->GetXaxis()->SetTitleSize(0.05);
   	dopdens->GetYaxis()->SetRangeUser(-dMax*1.3,dMax*1.3);
	dopdens->GetXaxis()->SetRangeUser(2.,YMax-1.);
	
   	dopdens->SetLineWidth(3);
	dopdens->SetLineColor(LCol); // set line color to black

	canvaspc->cd(3);
	dopdens->Draw("AL");

	NLine1 = new TLine();
	NLine1->SetLineStyle(2);
	NLine1->DrawLine(0,0, YMax-2,0);

	std::stringstream ssp;
	std::stringstream ssn;
	ssn << " n-type " ;
	ssp << " p-type " ;
	TLatex ln;
	TLatex lp;
	ln.DrawLatex( 10,1,ssn.str().c_str());

	lp.DrawLatex( 10,-1,ssp.str().c_str());

	delete[] q;
	delete[] dpoty; 
	
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetCutOnStrips() {

  wherecut = dwpot.GetXMAX()/2+1;
  wherecut2 = dwpot.GetXMAX()/2+1;
  WhereCut->SetNumber(wherecut);
  WhereCut2->SetNumber(wherecut2);	
  
  DrawAllGraph(2);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAll() {

  wherecut = WhereCut->GetNumber(); 
  WhereCut->SetNumber(wherecut);
  WhereCut2->SetNumber(wherecut);	
  DrawAllGraph(1);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAllw() {

  wherecut2 = WhereCut2->GetNumber(); 
  WhereCut2->SetNumber(wherecut2);
  WhereCut->SetNumber(wherecut2);	
  DrawAllGraph(1);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetCutBetweenStrips() {
  // return;
	wherecut = dwpot.GetXMAX()/2+1+(int)dwpot.Getpitch()/2;	
	wherecut2 = dwpot.GetXMAX()/2+1+(int)dwpot.Getpitch()/2;	
	WhereCut->SetNumber(wherecut);
	WhereCut2->SetNumber(wherecut2);

	DrawAllGraph(4);
	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetBField(Bool_t on) {
	bfieldon=on;
	if(bfieldon) BfieldEntry->SetState(kTRUE);
	else BfieldEntry->SetState(kFALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetDiffusion(Bool_t on) {
	diffusionon=on;
	//	if(diffusionon) TempEntry->SetState(kTRUE);
	// else TempEntry->SetState(kFALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetChargeCloud(Bool_t on) {
	chargecloudon=on;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool WFGUI::GetChargeCloud() {
	return chargecloudon;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetOscOn(Bool_t State) {
  OscOn=State;
  if(OscOn==true) 
    {	
      OscBWEntry->SetState(kTRUE);
      CSAImpEntry->SetState(kTRUE);
      CDEntry->SetState(kTRUE);
      LDEntry->SetState(kTRUE);
      TRiseEntry->SetState(kTRUE);
      TFallEntry->SetState(kTRUE);
      ShTransEntry->SetState(kTRUE);
      ShNoiseEntry->SetState(kTRUE);
      BBVthEntry->SetState(kTRUE);
      BBNoiseEntry->SetState(kTRUE);
      BBBWEntry->SetState(kTRUE);
      BBGainEntry->SetState(kTRUE);
      BBImpEntry->SetState(kTRUE);
      CSAVthEntry->SetState(kTRUE);
      OscBWLabel->Disable(kFALSE);
      ImpLabel->Disable(kFALSE);
      CDLabel->Disable(kFALSE);
      TRiseLabel->Disable(kFALSE);
      // ShTransLabel->Disable(kFALSE);
      ShNoiseLabel->Disable(kFALSE);
      BBNoiseLabel->Disable(kFALSE);
      BBBWLabel->Disable(kFALSE);
      
    }
  else 	
    {
      CDEntry->SetState(kFALSE);
      LDEntry->SetState(kFALSE);
      OscBWEntry->SetState(kFALSE);
      CSAImpEntry->SetState(kFALSE);
      TRiseEntry->SetState(kFALSE);
      TFallEntry->SetState(kFALSE);
      ShTransEntry->SetState(kFALSE);
      ShNoiseEntry->SetState(kFALSE);
      BBVthEntry->SetState(kFALSE);
      BBNoiseEntry->SetState(kFALSE);
      BBBWEntry->SetState(kFALSE);
      BBImpEntry->SetState(kFALSE);
      BBGainEntry->SetState(kFALSE);
      CSAVthEntry->SetState(kFALSE);
      OscBWLabel->Disable(kTRUE);
      ImpLabel->Disable(kTRUE);
      CDLabel->Disable(kTRUE);
      TRiseLabel->Disable(kTRUE);
      // ShTransLabel->Disable(kTRUE);
      ShNoiseLabel->Disable(kTRUE);
      BBNoiseLabel->Disable(kTRUE);
      BBBWLabel->Disable(kTRUE);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetFileNameOn(Bool_t State) {
  SetLess2DPlot(kTRUE);
  FileNameOn=State;
	if(FileNameOn==true) 
	{	
	  FileNameEntry->SetState(kTRUE);
	  FileNameLabel->Disable(kFALSE);
	}
	else 	
	{
	    FileNameEntry->SetState(kFALSE);
	    FileNameLabel->Disable(kTRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetOscBW(double bw) {
  OscBW=bw;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetOscBW() {
  return OscBW;
  //	  OscBWEntry->GetNumber();
//	else OscBWEntry->SetState(kFALSE);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBImp(double Imp) {
  BBImp=Imp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBImp() {
  return BBImp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAImp(double Imp) {
  CSAImp=Imp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAImp() {
  return CSAImp;
  // ImpEntry->GetNumber();
//	else OscBWEntry->SetState(kFALSE);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCDet(double CD) {
	CDet=CD;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCDet() {
  return CDet;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLDet(double LD) {
	LDet=LD;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetLDet() {
  return LDet;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetTRise(double tr) {
	TRise=tr;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetTRise() {
	return TRiseEntry->GetNumber();

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetTFall(double tf) {
	TFall=tf;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetTFall() {
	return TFallEntry->GetNumber();

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAVth(double vt) {
  //time threshold
  CSAVth=vt;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAVth() {
  return CSAVth;

}


//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetShTrans() {
  return ShTrans;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShTrans(double vt) {
	ShTrans=vt;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBVth( double vt) {
	BBVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBVth() {
  return BBVth;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBGain( double vt) {
	BBGain=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBGain() {
  return BBGain;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBBW(double vt) {
	BBBW=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBBW() {
  return BBBW;

}



/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShNoise(double vt) {
	ShNoise=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetShNoise() {
  return ShNoise;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBNoise(double vt) {
	BBNoise=vt;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBNoise() {
  return BBNoise;

}

//////////////////////////////////////////////////////////////////////////////////////
// double WFGUI::GetTVth() {
//  return TVth;
// }

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetJitter(double vt) {
	CSAJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetJitter() {
  return fabs(CSAJitter);

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBJitter(double vt) {
	BBJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBJitter() {
  return fabs(BBJitter);

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSATVth(double vt) {
  CSATVth=vt; // Vth Forward time
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSATVth() {
  return CSATVth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAFTVth(double vt) {
  CSAFTVth=vt; // Vth Forward time
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAFTVth() {
  return CSAFTVth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBFTVth(double vt) {
	BBFTVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBFTVth() {
  return BBFTVth;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBTVth(double vt) {
	BBTVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBTVth() {
  return BBTVth;

}


/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetFJitter(double vt) {
	CSAFJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetFJitter() {
  return fabs(CSAFJitter);

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetSWidth(double tf) {
	SWidth= tf;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetSWidth() {
  return SWidth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetOscOn() {
	return OscOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetFileNameOn() {
	return FileNameOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBatchOn(Bool_t on) {
	BatchOn=on;
    if(BatchOn) {
        EventsEntry->SetState(kTRUE);
        EventsLabel->Disable(kFALSE);
    }
    else {
        EventsEntry->SetState(kFALSE);
        EventsLabel->Disable(kTRUE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetBatchOn() {
	return BatchOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBatchRandomOn(Bool_t on) {
	BatchRandomOn=on;
    if (BatchRandomOn==true){
        CarriersAngleNumberentry->SetState(kFALSE);
        CarriersInNumberentry->SetState(kFALSE);
        CarriersInLabel->Disable(kTRUE);
    }
    else{
        CarriersAngleNumberentry->SetState(kTRUE);
        CarriersInNumberentry->SetState(kTRUE);
        CarriersInLabel->Disable(kFALSE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetBatchRandomOn() {
	return BatchRandomOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetForceGain(Bool_t on) {
	ForceGain=on;
    }
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetForceGain() {
    return ForceGain;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetIrradiationOn() {
	return IrradiationOn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetDJOn() {
	return DJOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetIrradiationOn(Bool_t on) {
    IrradiationOn=on;
    if (IrradiationOn == true){
        IrradiationEntry ->SetState(kTRUE);
        IrradiationOnButton->SetTextColor(1, kFALSE);
        ForceGain = false;
        ForceGainButton ->SetOn(kFALSE, kFALSE);
        ForceGainButton ->SetEnabled(kFALSE);
        IrradiationLabel->Disable(kFALSE);
	SetIrradiation(IrradiationEntry->GetNumber());
    }
    else
      {
        IrradiationEntry ->SetState(kFALSE);
	if (!GetDJOn()) ForceGainButton ->SetEnabled(kTRUE);
	if (!GetDJOn()) ForceGainButton->SetTextColor(1, kFALSE);
	//        IrradiationEntry->SetNumber(0);
        IrradiationLabel->Disable(kTRUE);
    }
}
////////////////////////////////////////
void WFGUI::SetDJOn(Bool_t on) {

  DJOn=on;
  if (DJOn == true){

    DJEntry ->SetState(kTRUE);
    DJehEntry ->SetState(kTRUE);
    DJOnButton->SetTextColor(1, kFALSE);
    ForceGain = false;
    ForceGainButton ->SetOn(kFALSE, kFALSE);
    ForceGainButton ->SetEnabled(kFALSE);
    DJLabel->Disable(kFALSE);
    DJehLabel->Disable(kFALSE);
    SetDJValue(DJEntry->GetNumber());
    SetDJehValue(DJEntry->GetNumber());

  }
  else{

    DJEntry ->SetState(kFALSE);
    DJehEntry ->SetState(kFALSE);
    if (!GetIrradiationOn()) ForceGainButton ->SetEnabled(kTRUE);
    if (!GetIrradiationOn()) ForceGainButton->SetTextColor(1, kFALSE);
    DJLabel->Disable(kTRUE);
    DJehLabel->Disable(kTRUE);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getwhist() {
	return whist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getdhist() {
	return dhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::GetExhist() {
	return Exhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH1F* WFGUI::GetEnhist() {
	return Enhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getchist() {
	return chist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getchhist() {
	return chhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getctothist() {
	return ctothist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TLine* WFGUI::GetNLine1() {
	return NLine1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TEllipse* WFGUI::GetEllipse() {
	return ellipse;
}

/////////////////////////////////////////////////////////////////////////////////////////////
TLine* WFGUI::GetNLine2() {
	return NLine2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
WFGUI::~WFGUI() {
	Cleanup();
	
	
// 	for (int i = 0; i < (dwpot.GetYMAX()); i++) {
// 	   if (df[i] != NULL) delete[] df[i];
// 	}
// 	delete[] df;
// 	
// 	for (int i = 0; i < (dwpot.GetYMAX()); i++){
// 	   if (wf[i] != NULL) delete[] wf[i];
// 	}
// 
//         delete[] wf;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CloseWindow() // Got close message for this MainFrame. Terminates the application.
{
    gApplication->Terminate();
}
/////////////////////////////////////////////////////////////////////////////////////////////
TThread* WFGUI::GetPotentialThread() {
	return PotentialThread;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void* WFGUI::StartPotentialCalcTh(void* arg)
{	

	WFGUI* gui = (WFGUI*) arg;
	gui->SetAllButton(0);
	gui->CallCalculatePotentials();
	gui->ThreadstopPotential();

	
	gui->DrawAllGraph(1);
	sleep(1);
	gui->SetAllButton(1);
	//gui->DrawFieldsAbs();

	return NULL;	
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstartPotential(){

	CallBoundaryConditions();
	if(!PotentialThread){
		//stopped=false;
		WFGUI* arg = this;
		PotentialThread= new TThread("memberfunction",
			            (void(*) (void *))&StartPotentialCalcTh,
			            (void*) arg);
		PotentialThread->Run();
			
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstopPotential(){
  FillHist(); // does not increase memory


	//	if(fieldyes==true) DrawFieldHist();
	if(PotentialThread){
	  //Nicolo
	  // stopped=true;
		TThread::Delete(PotentialThread);
		//	delete PotentialThread; // dont' remove it crashes
			PotentialThread=0;
		return 0;
	}      
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void* WFGUI::StartCurrentsCalcTh(void* arg)
{	
	WFGUI* gui = (WFGUI*) arg;
	gui->CallCalculateCurrents();
	gui->ThreadstopCurrents();
	gui->stopped=false;
	return NULL;	
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstartCurrents(){
	if(!CurrentsThread){
		//stopped=false;
		WFGUI* arg = this;
		CurrentsThread= new TThread("memberfunction",
			            (void(*) (void *))&StartCurrentsCalcTh,
			            (void*) arg);
		CurrentsThread->Run();	
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstopCurrents(){
	if(CurrentsThread){
		//stopped=true;
		TThread::Delete(CurrentsThread);
		// delete CurrentsThread;
		CurrentsThread=0;
		return 0;
	}      
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::KillPotentialThread() {
		TThread::Delete(PotentialThread);
		// delete PotentialThread;
		PotentialThread=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvasp() {
	return canvasp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvaspc() {
	return canvaspc;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvasw() {
	return canvasw;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcurcanvas() {
	return curcanvas;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getosccanvas() {
	return osccanvas;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGHProgressBar* WFGUI::GetProgressBar() {
	return CurrentsProgressBar;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetT() {
	return Temp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainon(bool gain) {
	gainon=gain;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetParticleType(int ptype) {
	ParticleType=ptype;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetParticleType() {
	return ParticleType;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetNumPairs(double ptype) {
	NumPairs=ptype;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetNumPairs() {
	return NumPairs;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetGainon() {
	return gainon;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainRatio(double gain) {
	GainRatio=gain;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetGainRatio() {
	return GainRatio;

}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetYGain(double y_gain) {
	ygain=y_gain;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetYGain() {
	return ygain;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetAngle() {
	//angle = (TMath::Pi()/180)*CarriersAngleNumberentry->GetNumber();
	angle = CarriersAngleNumberentry->GetNumber(); //angle in degrees
	return angle;
}
/////////////////////////////////////////////////////////////////////////
int WFGUI::GetDimMaxCarriers(){
	return dimMaxCarriers;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAllButton(int NN) {
  if (NN == 1)
    {
      if(!GetLessPlot())
	{
	  OnStripsButton->SetEnabled(kTRUE);
	  BetweenStripsButton->SetEnabled(kTRUE);
	  ExButton->SetEnabled(kTRUE);
	  EyButton->SetEnabled(kTRUE);
	  OnStripsButton2->SetEnabled(kTRUE);
	  BetweenStripsButton2->SetEnabled(kTRUE);
	  ExButton2->SetEnabled(kTRUE);
	  EyButton2->SetEnabled(kTRUE);
	  DrawCutsUserEntry->SetEnabled(kTRUE);
	  DrawCutsUserEntry2->SetEnabled(kTRUE);
	}
	CalculateButton->SetEnabled(kTRUE);
	CalcPotButton->SetEnabled(kTRUE);
    CalcPotButton->SetTextColor(1,kFALSE);

    }
  else
    {
	OnStripsButton->SetEnabled(kFALSE);
	BetweenStripsButton->SetEnabled(kFALSE);
	ExButton->SetEnabled(kFALSE);
	EyButton->SetEnabled(kFALSE);
	//	EtotButton->SetEnabled(kFALSE);
	CalculateButton->SetEnabled(kFALSE);
	CalcPotButton->SetEnabled(kFALSE);
	OnStripsButton2->SetEnabled(kFALSE);
	BetweenStripsButton2->SetEnabled(kFALSE);
	ExButton2->SetEnabled(kFALSE);
	EyButton2->SetEnabled(kFALSE);
	DrawCutsUserEntry->SetEnabled(kFALSE);
	DrawCutsUserEntry2->SetEnabled(kFALSE);
	//	EtotButton2->SetEnabled(kFALSE);
    }

	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetFileName(string* fname) {
  SetLess2DPlot(kTRUE);
  fileName = *fname;
}


/////////////////////////////////////////////////////////////////////////////////////////////
string* WFGUI::GetFileName() {
  
  if (fileName=="" || strncmp(fileName.c_str(),"wf_",3)==0)
    {
      
      //      CallSetFileName("wf");
      CallSetFileName();  
    }
  
  //
  return &fileName;
}
/////////////////////////////////////////////////////////////////////////////////////////////

string* WFGUI::CallSetFileName() {

  
  //  TDatime *NameDate = new TDatime();
  Int_t Dif = 0;
  Float_t BW=0.;
  if(Getdiffusionon() ) Dif = 1;  
  if(OscOn==true)   BW = GetOscBW();
  string Part;
  Int_t Qc = 0;

  switch (radiobuttonstatus){
  case MIPunif:
    Part = "_MIPU";
    Qc = 75;
    break;
    
  case MIPnonunif:
    Part = "_MIPNONU";
    Qc = 0;
    break;
    
  case MIPlandau:
    Part = "_MIPL";
    Qc = 0;
    break;
    
  case ALPHA_TOP:
    Part = "_AT";
          if (ParticleSpecificsEntry->GetNumber()<=30){//putting the 30µm range limit here
          Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRangeEntry->GetNumber();
          }
          else{
              Qc = 30;
          }
    break;
    
  case ALPHA_BOTTOM:

    Part  = "_AB";
          if (ParticleSpecificsEntry->GetNumber()<=30){//putting the 30µm range limit here
              Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRangeEntry->GetNumber();
          }
          else{
              Qc = 30;
          }
    break;
    
  case USR_CHARGE:
    Part = "_USRQ";
          Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRChargeentry->GetNumber();
    break;
    
  default: break;
  }
  
  string defaultName = FileNameEntry->GetText();
  if (defaultName=="") defaultName = "wf";


  //  cout << Part << endl;
  //  ss << FileNameEntry->GetText()
  std::stringstream ss;

  ss << defaultName 
    //  ss << FileNameEntry->GetText()
    << "_BV" <<  Biasentry->GetNumber()
     << "_DV" << Depletionentry->GetNumber()
     << "_W" << dwpot.GetXMAX()
     << "_H" << dwpot.GetYMAX()
    << "_SP" << Pitchentry->GetNumber()
    << "_SW" << Widthentry->GetNumber()
    << "_G" << Gainentry->GetNumber()
    << "_GR" << GainRatioentry->GetNumber()
    << "_T" << TempEntry->GetNumber()
     << "_A" << CarriersAngleNumberentry->GetNumber()
     << "_D" << Dif  
     << "_BW" << BW
     << "_C" << GetCDet()
     << "_L" << GetLDet()    
     << Part
     << Qc;

    //    << "_" << NameDate->GetDate()
    // << "_" << NameDate->GetTime()
    //    << ".txt";
	fileName = ss.str();
	if (GetFileNameOn() ) 
	  cout <<"Creating output file: "<< fileName << endl;
	return &fileName;

}
/////////////////////////////////////////////////////////////////////////////////////////////
/*
string* WFGUI::CallSetFileName(string defaultName) {
  
  std::stringstream ss;
  ss << FileNameEntry->GetText()
    << "_BV" <<  Biasentry->GetNumber()
    << "_W" << XMAXentry->GetNumber()
    << "_H" << YMAXentry->GetNumber()
    << "_SP" << Pitchentry->GetNumber()
    << "_SW" << Widthentry->GetNumber()
    << "_G" << Gainentry->GetNumber()
    << "_T" << TempEntry->GetNumber()
    //    << "_" << NameDate->GetDate()
    // << "_" << NameDate->GetTime()
    << ".txt";
	fileName = ss.str();
	cout <<"Creating output file: "<< fileName << endl;
	return &fileName;

}
 
*/
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAlphaRange(double valrange) {
	AlphaRange=valrange;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetAlphaRange() {
	return AlphaRange;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetXRayRange(double valrange) {
	XRayRange=valrange;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetXRayRange() {
	return XRayRange;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPrecision(int valprecision) {
	Precision=valprecision;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetPrecision() {
	return Precision;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetSampling(double valprecision) {
	Sampling=valprecision;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetSampling() {
	return Sampling;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetIrradiation(double valneq) {
  if (GetIrradiationOn())   Irradiation=valneq;
  else Irradiation = 0;

}
double WFGUI::GetIrradiation() {
  return Irradiation;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetDJValue(double valneq) {
    DJ=valneq;
}
double WFGUI::GetDJValue() {
    return DJ;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetDJehValue(double valneq) {
    DJeh=valneq;
}
double WFGUI::GetDJehValue() {
    return DJeh;
}


/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetNumberP(int number) {
	NumberP=number;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetNumberP() {
	return NumberP;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetConstQFlag(bool cflag) {
	ConstQFlag=cflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetConstQFlag() {
	return ConstQFlag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCalibFlag(bool cflag) {
	CalibFlag=cflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetCalibFlag() {
	return CalibFlag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetUniformQFlag(bool uflag) {
	UniformQFlag=uflag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetUserUniformQFlag(bool uflag) {
	UserUniformQFlag=uflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetUniformQFlag() {
	return UniformQFlag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetUserUniformQFlag() {
	return UserUniformQFlag;
}
//////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetEvNumber(int ev) {
	EvNumber=ev;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetEvNumber() {
	return EvNumber;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetEvent() {
	return ThisEvent;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetEvent(int thisev) {
	ThisEvent=thisev;
	
}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::Getygainlow(){
  if(StripButton[1]->IsOn()*BulkButton[0]->IsOn() || StripButton[0]->IsOn()*BulkButton[1]->IsOn() ) return (dwpot.GetYMAX()- GAINDEPTH - GAINLENGHT);
  else return GAINDEPTH;
}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::Getygainhigh(){
   if(StripButton[1]->IsOn()*BulkButton[0]->IsOn() || StripButton[0]->IsOn()*BulkButton[1]->IsOn() ) return (dwpot.GetYMAX()- GAINDEPTH) ;
   else return GAINDEPTH+GAINLENGHT;
}

///--------------------------------------------------------------------------------------------------------------------------------------------------///

