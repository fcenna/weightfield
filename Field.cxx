#include "Field.h"
///////////////////////////////////////////////////////////////////
Field::Field() {
	x=0;
	y=0;
	abs=0;
}
//Field::Field(double valx, double valy) {
//	x=valx;
//	y=valy;
//}
void Field::SetField(double valx, double valy) {
	x=valx;
	y=valy;
}
void Field::AddField(double valx, double valy) {
	x +=valx;
	y +=valy;
}

double Field::GetFieldx() {
	return x;
}
double Field::GetFieldy() {
	return y;
}
///////////////////////////////////////////////////////////////////
void CalculateFields(Potentials &pot, Field **wf, Field **df)
{
	for (int i=1; i<pot.GetXMAX(); i++)
	{
	  for (int j=1; j<pot.GetYMAX(); j++)
	  //		for (int j=1; j<pot.GetYMAX(); j++)
		{		

			df[j][i].SetField(-( pot.Getdpot(j,i) - pot.Getdpot(j,(i-1+pot.GetXMAX())%(pot.GetXMAX())) )/GRIDDIST,
					  -( pot.Getdpot(j,i) - pot.Getdpot(j-1,i) ) /GRIDDIST);
			
			//			if (i==10) std::cout <<  j << " " << pot.Getdpot(j,i) << " " << pot.Getdpot(j-1,i) << " " << pot.Getdpot(j,i) - pot.Getdpot(j-1,i) << std::endl;
			wf[j][i].SetField(-( pot.Getwpot(j,i) - pot.Getwpot(j,(i-1+pot.GetXMAX())%(pot.GetXMAX())) )/GRIDDIST,
					  -( pot.Getwpot(j,i) - pot.Getwpot(j-1,i) )/GRIDDIST);

		}

	  df[0][i].SetField(df[1][i].GetFieldx(),df[1][i].GetFieldy());
	  wf[0][i].SetField(wf[1][i].GetFieldx(),wf[1][i].GetFieldy());

	  // Set the field at y = YMAX-1 equal to y= YMAX-2
	  df[pot.GetYMAX()-1][i].SetField(df[pot.GetYMAX()-2][i].GetFieldx(),df[pot.GetYMAX()-2][i].GetFieldy());
	  wf[pot.GetYMAX()-1][i].SetField(wf[pot.GetYMAX()-2][i].GetFieldx(),wf[pot.GetYMAX()-2][i].GetFieldy());





	}
}
///////////////////////////////////////////////////////////////////
void CalculateAbsFields(Potentials &pot, Field** wf,Field** df) {
	for (int i=0; i<pot.GetXMAX(); i++)
	{
		for (int j=0; j<pot.GetYMAX(); j++)
		{
			df[j][i].Setabs(sqrt((df[j][i].GetFieldx())*(df[j][i].GetFieldx())+(df[j][i].GetFieldy())*(df[j][i].GetFieldy())));
			wf[j][i].Setabs(sqrt((wf[j][i].GetFieldx())*(wf[j][i].GetFieldx())+(wf[j][i].GetFieldy())*(wf[j][i].GetFieldy())));
		}
	}
}
///////////////////////////////////////////////////////////////////
/*
Field** RotateField(Potentials &pot, Field** df, double angle) {
	Field **newdf;									// B hinaus, winkel positiv ... B hinein, winkel negativ
	newdf = new Field*[(pot.GetYMAX())];
	for (int i = 0; i < (pot.GetYMAX()); i++) newdf[i] = new Field[(pot.GetXMAX())];

	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			newdf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	return newdf;
}
*/

void RotateField(Potentials &pot, Field** rotatedf, Field** df, double angle) {
	

	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			rotatedf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			rotatedf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////

double Field::Getabs() {
	return abs;
}

void Field::Setabs(double val) {
	abs=val;
}

Field::~Field() {

}
