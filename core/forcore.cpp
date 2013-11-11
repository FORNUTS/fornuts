#ifndef FORCORE_CPP
#define FORCORE_CPP

#include "forcore.h"

void harv(vars & progVars)
{
	if(progVars.HARVT != 1)
	{
		progVars.HSBMAS[progVars.ROTNUM]=progVars.SBMAS;
		progVars.HTREM[progVars.ROTNUM]=progVars.HSBMAS[progVars.ROTNUM];
		progVars.NREMS[progVars.ROTNUM]=progVars.SBMAS*0.001;
		progVars.NREMOV[progVars.ROTNUM]=progVars.NREMS[progVars.ROTNUM];
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+progVars.LBMAS*(progVars.LEAFNF/100.0)+progVars.CRBMAS*0.001+progVars.BBMAS*0.0033+progVars.FRBMAS*(progVars.ROOTNF/100.0);
	}
	else
	{
		progVars.HLBMAS[progVars.ROTNUM]=progVars.LBMAS;
		progVars.HBBMAS[progVars.ROTNUM]=progVars.BBMAS;
		progVars.HSBMAS[progVars.ROTNUM]=progVars.SBMAS;
		progVars.HCRMAS[progVars.ROTNUM]=progVars.CRBMAS;

		progVars.HTREM[progVars.ROTNUM]=progVars.LBMAS+progVars.BBMAS+progVars.SBMAS+progVars.CRBMAS;

		progVars.NREML[progVars.ROTNUM]=progVars.LBMAS*(progVars.LEAFNF/100.0);
		progVars.NREMCR[progVars.ROTNUM]=progVars.CRBMAS*0.001;
		progVars.NREMS[progVars.ROTNUM]=progVars.SBMAS*0.001;
		progVars.NREMB[progVars.ROTNUM]=progVars.BBMAS*0.0033;

		progVars.NREMOV[progVars.ROTNUM]=progVars.NREML[progVars.ROTNUM]+progVars.NREMCR[progVars.ROTNUM]+progVars.NREMS[progVars.ROTNUM]+progVars.NREMB[progVars.ROTNUM];
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+progVars.FRBMAS*(progVars.ROOTNF/100.0);
	}

	progVars.SOILN[progVars.ROTNUM+1]=progVars.SOILN[progVars.ROTNUM];
	progVars.ROTNUM+=1;
	progVars.AGE=0;
}

void slashb(vars & progVars)
{
	//N loss, warm
	if(progVars.SBURNT == 1)
	{
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]-75.0;
		progVars.NSBHAR[progVars.ROTNUM]=progVars.NSBHAR[progVars.ROTNUM]+75.0;
	}
	//N loss, hot
	else
	{
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]-150.0;
		progVars.NSBHAR[progVars.ROTNUM]=progVars.NSBHAR[progVars.ROTNUM]+150.0;
	}
}

void litter(vars & progVars)
{
	progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+(0.50*progVars.LBMAS*(progVars.LEAFNF/100.0))*(1.0-(progVars.LTRANS/100.0))+(progVars.FRBMAS*(progVars.ROOTNF/100.0))*(1.0-(progVars.RTRANS/100.0));
}

void annual(vars & progVars)
{
	progVars.RAINNR[progVars.ROTNUM]=progVars.RAINNR[progVars.ROTNUM]+progVars.NINPUT;
	progVars.OUTNR[progVars.ROTNUM]=progVars.OUTNR[progVars.ROTNUM]+progVars.NOUTPUT;

	progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+progVars.NINPUT-progVars.NOUTPUT;
}

void availN(vars & progVars, int simYear)
{
	progVars.NAMT=0.0;
	//add fertilizer N maybe
	if(progVars.AGE == progVars.FERAGE && progVars.FERATE != 0)
		progVars.NAMT=progVars.FERTNR[progVars.ROTNUM];


	//determin available N
	progVars.CNRATO=progVars.SOILC/progVars.SOILN[progVars.ROTNUM];
	progVars.MINRL=progVars.SOILN[progVars.ROTNUM]*0.012*(2.7-log(0.2*progVars.CNRATO));
	progVars.NAVAIL=progVars.MINRL+progVars.NAMT;

    while(progVars.NAVAIL > 0) //this could be wrong, fixes looping forever on some inputs
	{
		//set initial biomass amts
		if(progVars.AGE <= 1)
		{
			progVars.LBN=0.0;
			progVars.BBN=0.0;
			progVars.FRBN=0.0;
			progVars.CRBN=0.0;
			progVars.SBN=0.0;
		}
		//stem biomass chapman/richards
		if(progVars.SI  == 45)
		{
			progVars.XPONNT=1-exp(-0.088*progVars.PHSAGE);
			if(progVars.XPONNT <= 0)
				progVars.XPONNT=0.0001;
			progVars.SBMAS=102000.0*(pow(progVars.XPONNT,4.7));

			if(progVars.PHSAGE > 30)
				progVars.LBMAS = 6000.0;
			else
				progVars.LBMAS=200.0*progVars.PHSAGE;
			if(progVars.PHSAGE > 60)
				progVars.BBMAS=15000.0;
			else
				progVars.BBMAS=250.0*progVars.PHSAGE;

			progVars.CRBMAS=0.2*progVars.SBMAS;
			progVars.FRBMAS=(progVars.FRFPRO/100.0)*progVars.LBMAS;
		}
		else if(progVars.SI == 55)
		{
			progVars.XPONNT=1-exp(-0.120*progVars.PHSAGE);
			if(progVars.XPONNT <= 0)
				progVars.XPONNT=0.0001;
			progVars.SBMAS=122400.0*(pow(progVars.XPONNT, 6.2));

			if(progVars.PHSAGE > 25)
				progVars.LBMAS = 7000.0;
			else
				progVars.LBMAS=280.0*progVars.PHSAGE;

			if(progVars.PHSAGE > 60)
				progVars.BBMAS=20000.0;
			else
				progVars.BBMAS=333.33*progVars.PHSAGE;

			progVars.CRBMAS=0.2*progVars.SBMAS;
			progVars.FRBMAS=(progVars.FRFPRO/100.0)*progVars.LBMAS;
		}
		else if(progVars.SI == 65)
		{
			progVars.XPONNT=1-exp(-0.1253*progVars.PHSAGE);
			if(progVars.XPONNT <= 0)
				progVars.XPONNT=0.0001;
			progVars.SBMAS=171788.0*(pow(progVars.XPONNT, 5.3));

			if(progVars.PHSAGE > 25)
				progVars.LBMAS = 8500.0;
			else
				progVars.LBMAS=320.0*progVars.PHSAGE;
			if(progVars.PHSAGE > 60)
				progVars.BBMAS=25000.0;
			else
				progVars.BBMAS=416.66*progVars.PHSAGE;

			progVars.CRBMAS=0.2*progVars.SBMAS;
			progVars.FRBMAS=(progVars.FRFPRO/100.0)*progVars.LBMAS;
		}
		else if(progVars.SI == 75)
		{
			progVars.XPONNT=1-exp(-0.114*progVars.PHSAGE);
			if(progVars.XPONNT <= 0)
				progVars.XPONNT=0.0001;
			
			progVars.SBMAS=244800.0*(pow(progVars.XPONNT, 4.3));
			
			if(progVars.PHSAGE > 20)
				progVars.LBMAS = 9000.0;
			else
				progVars.LBMAS=450.0*progVars.PHSAGE;
			if(progVars.PHSAGE > 60)
				progVars.BBMAS=30000.0;
			else
				progVars.BBMAS=500.00*progVars.PHSAGE;

			progVars.CRBMAS=0.2*progVars.SBMAS;
			progVars.FRBMAS=(progVars.FRFPRO/100.0)*progVars.LBMAS;
		}
		
		//if first year, cannot calculate NREQ
		if(progVars.ROTNUM != 1 || progVars.AGE != progVars.ISAGE)
		{
			progVars.TRANS=(progVars.LTRANS/100.0)*(progVars.LEAFNF/100.0)*progVars.LBN*0.50+(progVars.RTRANS/100.0)*(progVars.ROOTNF/100.0)*progVars.FRBN;

			progVars.NREQ=(progVars.LBMAS-progVars.LBN)*(progVars.LEAFNF/100.0)+(progVars.BBMAS-progVars.BBN)*0.0033+(progVars.FRBMAS-progVars.FRBN)*(progVars.ROOTNF/100.0)+((progVars.CRBMAS-progVars.CRBN)+(progVars.SBMAS-progVars.SBN))*0.001+0.5*progVars.LBN*(progVars.LEAFNF/100.0)+progVars.FRBN*(progVars.ROOTNF/100.0)-progVars.TRANS;
		}
		//calculate retranslocation, N needs
		else
		{
			progVars.TRANS=0.0;
			progVars.NREQ=0.0;
		}
		//see if enough N is available
        if(progVars.NAVAIL <= progVars.NREQ)
		{
			//reduce growth and try again
			progVars.PHSAGE=progVars.PHSAGE-0.20;
			if(progVars.PHSAGE<=0)
				progVars.PHSAGE=0.0;
			//go back
		}
		else
		{
			//deduct N uptake
            progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]-progVars.NREQ;
			break;
		}
	}

	//update previous biomass and return
	progVars.LBN=progVars.LBMAS;
	progVars.BBN=progVars.BBMAS;
	progVars.FRBN=progVars.FRBMAS;
	progVars.CRBN=progVars.CRBMAS;
	progVars.SBN=progVars.SBMAS;

	if(simYear==progVars.FINISH)
		availN(progVars, ++simYear);
}

void fert(vars & progVars)
{
	if(progVars.FERATE == 100)
	{
		progVars.FERTNR[progVars.ROTNUM]=progVars.FERTNR[progVars.ROTNUM]+100.0;
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+100.0;
		progVars.FERFAC=0.20;
		progVars.FTRIG=progVars.AGE+3;
	}
	else if(progVars.FERATE == 200)
	{
		progVars.FERTNR[progVars.ROTNUM]=progVars.FERTNR[progVars.ROTNUM]+200.0;
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]+200.0;
		progVars.FERFAC=0.25;
		progVars.FTRIG=progVars.AGE+4;
	}
	else
	{
		progVars.FERATE=300;
		progVars.FERTNR[progVars.ROTNUM]=progVars.FERTNR[progVars.ROTNUM]+300.0;
		progVars.SOILN[progVars.ROTNUM]=progVars.FERTNR[progVars.ROTNUM]+300.0;
		progVars.FERFAC=0.25;
		progVars.FTRIG=progVars.AGE+5;
	}
}

void pburn(vars & progVars)
{
	if(progVars.BURNTP == 1) //N loss, warm
	{
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]-20.0;
		progVars.NPBHAR[progVars.ROTNUM]=progVars.NPBHAR[progVars.ROTNUM]+20.0;
		progVars.PBTRIG=progVars.AGE+2;
		progVars.PBFAC=0.25;
	}
	else //N loss, hot
	{
		progVars.SOILN[progVars.ROTNUM]=progVars.SOILN[progVars.ROTNUM]-40.0;
		progVars.NPBHAR[progVars.ROTNUM]=progVars.NPBHAR[progVars.ROTNUM]+40.0;
		progVars.PBTRIG=progVars.AGE+3;
		progVars.PBFAC=0.25;
	}
}

void zero( vars & progVars )
{
	//6 is constant because all of the arrays are of size 6
    for(int i=0; i<progVars.arrSize; ++i)
	{
		progVars.SOILN[i] = 0.0;
		progVars.NREMOV[i] = 0.0;
		progVars.NREMB[i] = 0.0;
		progVars.NREML[i] = 0.0;
		progVars.NREMS[i] = 0.0;
		progVars.NREMCR[i] = 0.0;
		progVars.NPBHAR[i] = 0.0;
		progVars.NSBHAR[i] = 0.0;
		progVars.RAINNR[i] = 0.0;
		progVars.FERTNR[i] = 0.0;
		progVars.OUTNR[i] = 0.0;
		progVars.HLBMAS[i] = 0.0;
		progVars.HBBMAS[i] = 0.0;
		progVars.HSBMAS[i] = 0.0;
		progVars.HCRMAS[i] = 0.0;
		progVars.HTREM[i] = 0.0;
	}

	progVars.FTRIG=0;
	progVars.PBTRIG=0;
	progVars.FERFAC=0.0;
	progVars.PBFAC=0.0;
	progVars.ROTNUM=0;

}

//Parse input.ini
void inputFromFile(vars & progVars)
{
	dictionary *ini;
	ini = iniparser_load((char *)progVars.inputFileName.c_str());

	if (ini==NULL) {
		*progVars.fp<<"Cannot parse file: "<<progVars.inputFileName.c_str()<<endl;
		return;
	}

	progVars.AGE=iniparser_getint(ini,"setting:age",0);
	progVars.SI=iniparser_getint(ini,"setting:si",0);
	progVars.ROTATN=iniparser_getint(ini,"setting:rotatn",0);
	progVars.NUM=iniparser_getint(ini,"setting:num",0);
	progVars.FERAGE=iniparser_getint(ini,"setting:ferage",0);
	progVars.FERATE=iniparser_getint(ini,"setting:ferate",0);
	progVars.PBBURN=iniparser_getint(ini,"setting:pbburn",0);
	progVars.FREQBN=iniparser_getint(ini,"setting:freqbn",0);
	progVars.BURNTP=iniparser_getint(ini,"setting:burntp",0);
	progVars.HARVT=iniparser_getint(ini,"setting:harvt",0);
	progVars.SHBURN=iniparser_getint(ini,"setting:shburn",0);
	progVars.SBURNT=iniparser_getint(ini,"setting:sburnt",0);
	progVars.LTRANS=iniparser_getdouble(ini,"setting:ltrans",0.0);
	progVars.RTRANS=iniparser_getdouble(ini,"setting:rtrans",0.0);
	progVars.FRFPRO=iniparser_getdouble(ini,"setting:frfpro",0.0);
	progVars.SOILC=iniparser_getdouble(ini,"setting:soilc",0.0);
	progVars.ISOILN=iniparser_getdouble(ini,"setting:isoiln",0.0);
	progVars.LEAFNF=iniparser_getdouble(ini,"setting:leafnf",0.0);
	progVars.ROOTNF=iniparser_getdouble(ini,"setting:rootnf",0.0);
    progVars.NINPUT=iniparser_getdouble(ini,"setting:ninput",0.0);
	progVars.NOUTPUT=iniparser_getdouble(ini,"setting:noutput",0.0);
	iniparser_freedict(ini);
}

string checkInput(vars & progVars)
{
	string temp="";
    if(progVars.AGE<0||progVars.AGE>50) temp+="0<Initial Stand Age<50\n";
    if(!(progVars.SI==45||progVars.SI==55||progVars.SI==65||progVars.SI==75)) temp+="Site Index={45,55,65,75}\n";
    if(progVars.ROTATN<20||progVars.ROTATN>75) temp+="20<Rotation Lenght<75\n";
    if(progVars.NUM<1||progVars.NUM>5) temp+="1<Number of Rotations<5\n";
    if(progVars.FERAGE<0) temp+="0<Fertilization Age\n";
    if(!(progVars.FERATE==0||progVars.FERATE==100||progVars.FERATE==200||progVars.FERATE==300)) temp+="Fertilization Rate={0,100,200,300}\n";
    if(progVars.PBBURN<10&&progVars.PBBURN!=0) temp+="10<Prescribed Burn Age or 0\n";
    if((progVars.FREQBN<4||progVars.FREQBN>20)&&progVars.FREQBN!=0) temp+="4<Prescribed Burn Frequency<20 or 0\n";
    if(progVars.BURNTP<0||progVars.BURNTP>2) temp+="0<Prescribed Burn Temperature<2\n";
    if(progVars.HARVT<1||progVars.HARVT>2) temp+="1<Harvest Method<2\n";
    if(progVars.SHBURN<0||progVars.SHBURN>1) temp+="0<Broadcast Burn?<1\n";
    if(progVars.SBURNT<0||progVars.SBURNT>2) temp+="0<Broadcast Burn Temperature<2\n";
    if(progVars.LTRANS<0.0||progVars.LTRANS>40.0) temp+="0.0<% Nitrogen Reabsorbed from Leaves<40.0\n";
    if(progVars.RTRANS<0.0||progVars.RTRANS>40.0) temp+="0.0<% Nitrogen Reabsorbed from Roots<40.0\n";
    if(progVars.FRFPRO<50.0||progVars.FRFPRO>200.0) temp+="50.0<Ratio of Roots to Foliage<200.0\n";
    if(progVars.SOILC<15000||progVars.SOILC>35000) temp+="15000<Total Soil Carbon Pool<35000\n";
    if(progVars.ISOILN<1500||progVars.ISOILN>5000) temp+="1500<Total Nitrogen Pool<5000\n";
    if(progVars.LEAFNF<0.8||progVars.LEAFNF>2.0) temp+="0.8<% Weight of Nitrogen in Leaves<2.0\n";
    if(progVars.ROOTNF<0.5||progVars.ROOTNF>1.2) temp+="0.5<% Weight of Nitrogen in Roots<1.2\n";
    if(progVars.NINPUT<3||progVars.NINPUT>20) temp+="3<Input of Nitrogen<20\n";
    if(progVars.NOUTPUT<1||progVars.NOUTPUT>5) temp+="1<Output of Nitrogen<5\n";
	
	//Special check to see if the number of rotations is greater than the prescribed burn frequency.
    if(progVars.NUM>progVars.FREQBN&&progVars.FREQBN>3) temp+="Number of Rotations < Prescribed Burn Frequency\n";

	if(temp=="")
	{
	return "";
	}
	else
	{
    return "Bad Input.\nPlease Check to Make Sure Your Values Follow These Constraints:\n"+temp;
	}
}

//Write to input.ini
void saveInputToFile(vars & progVars)
{
	ofstream fout;
	fout.open(progVars.saveInputFileName.c_str());
	fout<<"#\n"
		<<"# This is an example of ini file for fornuts\n"
		<<"#\n"
		<<"\n"
		<<"[Setting]\n"
		<<"\n"
		<<"AGE        = "<<progVars.AGE<<" ; Age\n"
		<<"SI         = "<<progVars.SI<<" ; Site Index (CHOICES ARE: 45, 55, 65, AND 75)\n"
		<<"ROTATN     = "<<progVars.ROTATN<<" ; Rotation length <=70\n"
		<<"NUM        = "<<progVars.NUM<<" ; Number of rotations <=5\n"
		<<"FERAGE     = "<<progVars.FERAGE<<" ; Age to fertilize\n"
		<<"FERATE     = "<<progVars.FERATE<<" ; Fertilizer rate (CHOICES ARE: 100,200,OR 300 KG/HA)\n"
		<<"PBBURN     = "<<progVars.PBBURN<<" ; Age to burn\n"
		<<"FREQBN     = "<<progVars.FREQBN<<" ; Frequency to burn\n"
		<<"BURNTP     = "<<progVars.BURNTP<<" ; Temp to burn\n"
		<<"HARVT      = "<<progVars.HARVT<<" ; Type of harvest method\n"
		<<"SHBURN     = "<<progVars.SHBURN<<" ; Burn the slash after harvest?\n"
		<<"SBURNT     = "<<progVars.SBURNT<<" ; Temp to burn\n"
		<<"LTRANS     = "<<progVars.LTRANS<<" ; % of Nitrogen reabsorbed from the leaves prior to abscission\n"
		<<"RTRANS     = "<<progVars.RTRANS<<" ; % of Nitrogen reabsorbed from the leaves from the fine roots prior to senescence\n"
		<<"FRFPRO     = "<<progVars.FRFPRO<<" ; Weight ratio of fine roots to foliage\n"
		<<"SOILC      = "<<progVars.SOILC<<" ; Total soil carbon pool\n"
		<<"ISOILN     = "<<progVars.ISOILN<<" ; Total nitrogen pool\n"
		<<"LEAFNF     = "<<progVars.LEAFNF<<" ; % weight content of nitrogen in leaves\n"
		<<"ROOTNF     = "<<progVars.ROOTNF<<" ; % weight content of nitrogen in fine roots\n"
		<<"NINPUT     = "<<progVars.NINPUT<<" ; Annual input of nitrogen from atmospheric sources and from biological n fixation\n"
		<<"NOUTPUT    = "<<progVars.NOUTPUT<<" ; Annual output of nitrogen from leaching beyond the rooting zone\n";
	fout.close();
}

void saveOutputCSV(vars & progVars)
{
	ofstream csvfile;
	csvfile.open((progVars.outputFileName+".csv").c_str());
	csvfile<<"SOILN"<<","<<"HTREM"<<endl;
	for (int i=0; i<progVars.NUM; ++i)
	{
		csvfile<<progVars.SOILN[i]<<","<<progVars.HTREM[i]<<endl;
	}
	csvfile.close();
}
#endif
