#ifdef _WIN32
#include "..\core\forcore.cpp"
extern "C" {
#include "..\core\dictionary.c"
#include "..\core\iniparser.c"
}
#else
#include "../core/forcore.cpp"
extern "C" {
#include "../core/dictionary.c"
#include "../core/iniparser.c"
}
#endif
#include "fornuts.h"

void usage()
{
	cout<<"Usage:"<<endl;
	cout<<" -i interactive mode"<<endl;
	cout<<" -f [input file] [-o [output file]]"<<endl;
	cout<<" If no output file is specified it will print to the screen"<<endl;
}
int main(int argc, char **argv)
{
	vars progVars;		//create all variables 
	progVars.inputFromFile=false;
	progVars.saveInputToFile=false;
	progVars.interactiveMode=false;
	progVars.fp = &cout;
	//example of this is *fp<<"hello";
	ofstream fout;
	ifstream fin;
	if(argc == 1)
	{ 	
		usage(); 
		return 10;
	}
	for(int i = 1; i < argc; i++)
	{
		switch(argv[i][1])
		{
		case 'i':
			progVars.interactiveMode=true;
			break;
		case 'f':
			if(argv[i+1] == NULL)
			{
				usage();
				return 127;
			}
			i++;
			progVars.inputFileName=argv[i];
			progVars.inputFromFile=true;
			fin.open(argv[i]);
			if(!fin)
			{
				cout<<"Error opening infile, exiting"<<endl;
				return 0;
			}
			fin.close();
			break;
		case 'o':
			if(argv[i+1] == NULL)
			{
				usage();
				return 127;
			}
			i++;
			progVars.outputFileName=argv[i];
			progVars.saveInputToFile=true;
			fout.open(argv[i]);
			progVars.fp = &fout;
			break;
		default:
			usage();
			return 127;
			break;
		}
	}	

	if(progVars.interactiveMode && (progVars.saveInputToFile || progVars.inputFromFile))
	{
		usage();
		return 127;
	}
	else if(progVars.inputFromFile)
	{
		printIntro(progVars);
		printCredits(progVars);
		inputFromFile(progVars);
	}
	else if(progVars.interactiveMode)
	{
		printIntro(progVars);
		printCredits(progVars);
		inputManually(progVars);//call input here
	}
	else
	{
		usage();
		return 127;
	}

	zero(progVars);				//zeros out all variables
	start(progVars);

	if(progVars.saveInputToFile)
	{
		saveOutputCSV(progVars);
	}

	if(!progVars.inputFromFile)
	{
		*progVars.fp<<"Would you like to save your input from this simulation? (1=yes,0=no): "<<endl;
		bool save;
		cin>>save;
		if(save)
		{
			*progVars.fp<<"Please enter the file name you would ilke to save you input file to (ending in .ini):";
			cin>>progVars.saveInputFileName;
			saveInputToFile(progVars);
			saveOutputCSV(progVars);
			*progVars.fp<<"File saved to "<<progVars.saveInputFileName<<endl;
		}
	}

	fout.close();
	return 0;
}

//start doing stuff
void start(vars & progVars )
{
	int tmp=0;
	while(true)
	{
		progVars.SOILN[progVars.ROTNUM] = progVars.ISOILN;

		progVars.ISAGE=progVars.AGE;
		//set initial physiol. age
		progVars.PHSAGE=double(progVars.AGE);

		*progVars.fp<<"\n\nLook over the next screen of input values\nto make sure they are what you intended."<<endl;
		if(progVars.interactiveMode)
			cin.get();		//system pause
		echo(progVars);
		if(progVars.interactiveMode)
			cin.get();		//system pause

		if(progVars.inputFromFile)
		{
			//Check ranges of input
			string temp=checkInput(progVars);
			if(temp!="")
			{
			*progVars.fp<<temp<<endl;
			*progVars.fp<<"Exiting..."<<endl;
			exit(0);
			}
			break;
		}
		else
		{
			//Check ranges of input
			string temp=checkInput(progVars);
			if(temp!="")
			{
			*progVars.fp<<temp<<endl;
			*progVars.fp<<"Do you want to re-enter the input values?\nif yes enter 1, if no enter 0: "<<endl;
			cin>>tmp;
			if (tmp == 1)
			{
				inputManually(progVars);//call input here and continue
			}
			else
			{
				exit(0);
			}
			
			}
			
			*progVars.fp<<"After looking over the inputs, do you want to re-enter the input values?\nif yes enter 1, if no enter 0: "<<endl;
			cin>>tmp;
			if (tmp == 1)
			{
				inputManually(progVars);//call input here and continue
			}
			else
			{
				break;
			}
		}
	}

	progVars.FINISH=progVars.ROTATN * (progVars.NUM - 1) + (progVars.ROTATN - progVars.ISAGE) + 1;

	//let user know its working
	for (int i=0; i<progVars.FINISH; ++i)
	{
		*progVars.fp<<"Calculating Year: " << i+1 << " now"<<endl;

		if(progVars.PBBURN != 0)
		{
			progVars.MARKER = (progVars.AGE-progVars.PBBURN)-((progVars.AGE-progVars.PBBURN)/progVars.FREQBN) * progVars.FREQBN;
			if(progVars.MARKER == 0)
			{
				pburn(progVars);
			}
		}

		if(progVars.AGE == progVars.FERAGE && progVars.FERAGE != 0)
			fert(progVars);

		availN(progVars, i);

		if(progVars.AGE != progVars.ROTATN)
		{
			litter(progVars);
			
		}
		else
		{
			harv(progVars);
			progVars.PHSAGE=0.0;
			if(progVars.SHBURN != 0)
				slashb(progVars);
		}
			

		annual(progVars);

		progVars.AGE=progVars.AGE+1;

		if(progVars.AGE == progVars.FTRIG)
			progVars.FERFAC=0.0;
		if(progVars.AGE == progVars.PBTRIG)
			progVars.PBFAC=0.0;
		//accelerate allowable growth
		progVars.PHSAGE=progVars.PHSAGE+progVars.FERFAC+progVars.PBFAC+1.0;
	}

	echo(progVars);
	if(progVars.interactiveMode)
		cin.get();
	report(progVars);
	if(progVars.interactiveMode)
		cin.get();
}

void report(vars & progVars)
{
	progVars.NREMSL = 0, progVars.NRMSCR = 0, progVars.NREMSS = 0,
		progVars.NREMSB = 0, progVars.NPBURN = 0, progVars.NSBURN = 0, 
		progVars.RAINNS = 0, progVars.FERTNS = 0, progVars.NETDNS = 0, 
		progVars.OUTNS = 0, progVars.TLREM = 0, progVars.TBREM = 0, 
		progVars.TSREM = 0,	progVars.TCRREM = 0, progVars.NREMSI = 0;

	for(int i=0; i!=progVars.NUM; ++i)
	{
		progVars.NREMSL=progVars.NREMSL+progVars.NREML[i];
		progVars.NRMSCR=progVars.NRMSCR+progVars.NREMCR[i];
		progVars.NREMSS=progVars.NREMSS+progVars.NREMS[i];
		progVars.NREMSB=progVars.NREMSB+progVars.NREMB[i];
		progVars.NPBURN=progVars.NPBURN+progVars.NPBHAR[i];
		progVars.NSBURN=progVars.NSBURN+progVars.NSBHAR[i];
		progVars.RAINNS=progVars.RAINNS+progVars.RAINNR[i];
		progVars.FERTNS=progVars.FERTNS+progVars.FERTNR[i];
		progVars.OUTNS=progVars.OUTNS+progVars.OUTNR[i];
		progVars.TLREM=progVars.TLREM+progVars.HLBMAS[i];
		progVars.TBREM=progVars.TBREM+progVars.HBBMAS[i];
		progVars.TSREM=progVars.TSREM+progVars.HSBMAS[i];
		progVars.TCRREM=progVars.TCRREM+progVars.HCRMAS[i];
		progVars.NREMSI=progVars.NREMOV[i]+progVars.NREMSI;
	}

	progVars.TBMNRS=progVars.NREMSL+progVars.NRMSCR+progVars.NREMSS+progVars.NREMSB;
	progVars.TREM=progVars.TLREM+progVars.TBREM+progVars.TSREM+progVars.TCRREM;

	progVars.BEGIN=progVars.ISAGE;

	for(int ii=0; ii!=progVars.NUM; ++ii)
	{
		if(ii != 0)
			progVars.BEGIN=0;

		progVars.ENTRNR=progVars.RAINNR[ii]+progVars.FERTNR[ii];

		*progVars.fp<<endl<<"Summary of nitrogen budget of site and biomass yields by rotation"<<endl;
		*progVars.fp<<"-----------------------------------------------------------------"<<endl;

		progVars.SIMTIM=(ii+1)*(progVars.ROTATN)-progVars.ISAGE;
		*progVars.fp<<"Simulation year: " << progVars.SIMTIM << endl;

		*progVars.fp<<"Initial age of stand: " << progVars.BEGIN << endl;

		*progVars.fp<<"Rotation number: " << (ii+1) << endl<<endl;

		*progVars.fp<<"Inputs of nitrogen to site (kg/ha)     outputs of nitrogen from site (kg/ha)"<<endl;
		*progVars.fp<<"----------------------------------     -------------------------------------"<<endl;

		*progVars.fp<<"Fertilization: " << progVars.FERTNR[ii] << "\t\t       Slash burning:      " << progVars.NSBHAR[ii] <<endl;

		*progVars.fp<<"Precipitation: " << progVars.RAINNR[ii] << "\t\t       Prescribed burning: " << progVars.NPBHAR[ii] << endl;

		*progVars.fp<<"               ------\t\t       Leaching:           " << progVars.OUTNR[ii] << endl;

		*progVars.fp<<"Total:         " << progVars.ENTRNR << endl;
		*progVars.fp<<"\t\t\t\t       Losses from biomass removals" << endl;

		*progVars.fp<<"\t\t\t\t       Stems:        " << progVars.NREMS[ii] << endl;

		*progVars.fp<<"\t\t\t\t       Coarse roots: " << progVars.NREMCR[ii] << endl;

		*progVars.fp<<"\t\t\t\t       Foliage:      " << progVars.NREML[ii] << endl;

		*progVars.fp<<"\t\t\t\t       Branches:     " << progVars.NREMB[ii] << endl;

		*progVars.fp<<"\t\t\t\t                     -------"<<endl;
		*progVars.fp<<"\t\t\t\t       Total:        " << progVars.NREMOV[ii] << endl;

		progVars.NETDNR=progVars.ENTRNR-progVars.NREMOV[ii]-progVars.NSBHAR[ii]-progVars.NPBHAR[ii]-progVars.OUTNR[ii];
		progVars.NETDNS=progVars.NETDNS+progVars.NETDNR;

		*progVars.fp<<endl<<"Net change in total nitrogen of site (kg/ha):  " << progVars.NETDNR << endl;

		if(ii == 0)
			progVars.SOILN[0]=progVars.ISOILN+progVars.NETDNR;
		else
			progVars.SOILN[ii]=progVars.SOILN[ii-1]+progVars.NETDNR;

		*progVars.fp<<"Soil nitrogen pool at end of rotation (kg/ha): " << progVars.SOILN[ii] << endl;

		progVars.CNRATO=progVars.SOILC/progVars.SOILN[ii];
		*progVars.fp<<"Carbon:nitrogen ration at end of rotation:     " << progVars.CNRATO << endl;

		*progVars.fp<<endl<<"Biomass yields (kg/ha)"<<endl;
		*progVars.fp<<"----------------------" << endl;

		*progVars.fp<<"Stems:           " << progVars.HSBMAS[ii] << endl;

		*progVars.fp<<"Coarse roots:    " << progVars.HCRMAS[ii] << endl;

		*progVars.fp<<"Foliage:         " << progVars.HLBMAS[ii] << endl;

		*progVars.fp<<"Branches:        " << progVars.HBBMAS[ii] << endl;

		*progVars.fp<<"                 -------"<< endl;
		*progVars.fp<<"Total:           " << progVars.HTREM[ii] << endl;

		if(progVars.NREMOV[ii] != 0.0)
			progVars.NRUBR=progVars.HTREM[ii]/progVars.NREMOV[ii];
		else
			progVars.NRUBR=0.0;

		*progVars.fp<<endl<<"Biomass yield per unit of nitrogen removed: " << progVars.NRUBR << endl;

		if(progVars.interactiveMode)
			cin.get();
	}

	*progVars.fp<<endl<<"Summary of nitrogen budget and biomass yields for entire sumulation" << endl;
	*progVars.fp<<"-------------------------------------------------------------------"<<endl;

	*progVars.fp<<endl<<"Inputs of nitrogen to site (kg/ha)     outputs of nitrogen from site (kg/ha)" << endl;
	*progVars.fp<<"----------------------------------     -------------------------------------" << endl;

	*progVars.fp<<"Fertilization: " << progVars.FERTNS << "\t\t       Slash burning:      "  << progVars.NSBURN << endl;

	*progVars.fp<<"Precipitation: " << progVars.RAINNS << "\t\t       Prescribed burning: " << progVars.NPBURN << endl;

	*progVars.fp<<"               ------\t\t       Leaching:           " << progVars.OUTNS << endl;

	progVars.ENTRNR=progVars.RAINNS+progVars.FERTNS;
	*progVars.fp<<"Total:         " << progVars.ENTRNR << endl;
	*progVars.fp<<"\t\t\t\t       Losses from biomass removals" << endl;

	*progVars.fp<<"\t\t\t\t       Stems:        " << progVars.NREMSS << endl;

	*progVars.fp<<"\t\t\t\t       Coarse roots: " << progVars.NRMSCR << endl;

	*progVars.fp<<"\t\t\t\t       Foliage:      " << progVars.NREMSL << endl;

	*progVars.fp<<"\t\t\t\t       Branches:     " << progVars.NREMSB << endl;

	*progVars.fp<<"\t\t\t\t                     -------"<<endl;
	*progVars.fp<<"\t\t\t\t       Total:        " << progVars.NREMSI << endl;

	*progVars.fp<<endl<<"Net change in total nitrogen of site (kg/ha):    " << progVars.NETDNS << endl;

	*progVars.fp<<"Soil nitrogen pool at end of simulation (kg/ha): " << progVars.SOILN[progVars.NUM]<<endl;

	progVars.CNRATO=progVars.SOILC/progVars.SOILN[progVars.NUM];
	*progVars.fp<<"Carbon:nitrogen ration at end of simulation:     " << progVars.CNRATO << endl;

	*progVars.fp<<endl<<"Biomass yields (kg/ha)"<<endl<<"----------------------" << endl;

	*progVars.fp<<"Stems:        " << progVars.TSREM << endl;

	*progVars.fp<<"Coarse roots: " << progVars.TCRREM << endl;

	*progVars.fp<<"Foliage:      " << progVars.TLREM << endl;

	*progVars.fp<<"Branches:     " << progVars.TBREM << endl;

	*progVars.fp<<"              -------"<< endl;
	*progVars.fp<<"Total:        " << progVars.TREM << endl <<endl;

	if(progVars.NREMSI != 0.0)
		progVars.NRUBS=progVars.TREM/progVars.NREMSI;
	else
		progVars.NRUBS=0.0;

	*progVars.fp<<"Biomass yeild per unit of nitrogen removed: " << progVars.NRUBS << endl;
}

void echo(vars & progVars)
{
	*progVars.fp<<"\n\nUser-controlled simulation parameters"<<endl;
	*progVars.fp<<"--------------------------------------"<<endl;

	progVars.FINISH=progVars.ROTATN * progVars.NUM - progVars.AGE;

	*progVars.fp<<endl<<"Simulation period: " << progVars.FINISH << " Years"<<endl;

	*progVars.fp<<endl<<"Number of rotation to be simulated: " << progVars.NUM <<endl;

	*progVars.fp<<endl<<"Initial stand age: "<< progVars.AGE << " Years" << endl;

	*progVars.fp<<endl<<"Rotation length: " << progVars.ROTATN << " Years"<<endl;

	*progVars.fp<<endl<<"Site index: "<< progVars.SI <<endl;

	*progVars.fp<<endl<<"Soil carbon pool: " << progVars.SOILC << " kg/ha" <<endl;

	*progVars.fp<<endl<<"Initial soil nitrogen pool: " << progVars.ISOILN << " kg/ha" <<endl;

	*progVars.fp<<endl<<"Percent n in foliage: " << progVars.LEAFNF << " %" <<endl;

	*progVars.fp<<endl<<"Percent n in fine roots: "<< progVars.ROOTNF << " %" <<endl;

	*progVars.fp<<endl<<"Percent n reabsorbrbed\nprior to leaf abscission: " << progVars.LTRANS <<" %"<<endl;

	*progVars.fp<<endl<<"Percent n reabsorbed\nprior to seescence of fine roots: " << progVars.RTRANS << " %" <<endl;

	*progVars.fp<<endl<<"Precent of fine roots to foliage: " << progVars.FRFPRO << " %" << endl;

	*progVars.fp<<endl<<"Annual n input: " << progVars.NINPUT << " KG/HA" << endl;

	*progVars.fp<<endl<<"Annual n output: "<< progVars.NOUTPUT << " KG/HA" <<endl;

	if (progVars.FERAGE != 0)
	{
		*progVars.fp<<endl<<"N fertilization? yes"<<endl;
		*progVars.fp<<endl<<"N fertilization rate: "<< progVars.FERATE << " KG/HA" <<endl;
	}
	else
		*progVars.fp<<endl<<"N fertilization? no"<<endl;

	*progVars.fp<<endl<<"Stand age when fertilized: " << progVars.FERAGE << " years" <<endl;

	if (progVars.PBBURN == 0)
		*progVars.fp<<endl<<"Perscribed burning? no"<<endl;
	else
		*progVars.fp<<endl<<"Perscribed burning? yes"<<endl;

	*progVars.fp<<endl<<"Initial stand age when first prescribe burn: " << progVars.PBBURN << " years" <<endl;

	*progVars.fp<<endl<<"Frequency of prescribed burns: every " <<progVars.FREQBN << " years" <<endl; 
	if (progVars.BURNTP == 1)
		*progVars.fp<<endl<<"Relative temperature of prescribed burns: warm"<<endl;
	else if(progVars.BURNTP == 2)
		*progVars.fp<<endl<<"Relative temperature of prescribed burns: hot"<<endl;

	if (progVars.SHBURN == false)
		*progVars.fp<<endl<<"Slash burning? no"<<endl;
	else
		*progVars.fp<<endl<<"Slash burning? yes"<<endl;

	if (progVars.SBURNT == 1)
		*progVars.fp<<endl<<"Relative temperature of slash burns: warm"<<endl;
	else if (progVars.SBURNT == 2)
		*progVars.fp<<endl<<"Relative temperature of slash burns: hot"<<endl;

	if (progVars.HARVT == 1)
		*progVars.fp<<endl<<"Harvest method: whole tree"<<endl;
	else
		*progVars.fp<<endl<<"Harvest method: stem only"<<endl;
}


//Prints out intro to console
void printIntro(vars & progVars)
{
	*progVars.fp<<"  FFFFFFFF  OOOOOOOO  RRRRRR    NNN   NN  UUU  UUU  TTTTTTTT  SSSSSSSS  "<<endl;
	*progVars.fp<<"  FFFFFFFF  OOOOOOOO  RRRRRRRR  NNN   NN  UUU  UUU  TTTTTTTT  SSSSSSSS  "<<endl;
	*progVars.fp<<"  FFF       OOO  OOO  RRR  RRR  NNNN  NN  UUU  UUU     TT     SSS  SSS  "<<endl;
	*progVars.fp<<"  FFF       OOO  OOO  RRR  RRR  NNNN  NN  UUU  UUU     TT     SSS       "<<endl;
	*progVars.fp<<"  FFFFFF    OOO  OOO  RRRRRRRR  NNNNN NN  UUU  UUU     TT     SSS       "<<endl;
	*progVars.fp<<"  FFFFFF    OOO  OOO  RRRRRR    NNNNNNNN  UUU  UUU     TT     SSSSSSSS  "<<endl;
	*progVars.fp<<"  FFF       OOO  OOO  RRRRRRR   NN NNNNN  UUU  UUU     TT     SSSSSSSS  "<<endl;
	*progVars.fp<<"  FFF       OOO  OOO  RRR RRRR  NN  NNNN  UUU  UUU     TT          SSS  "<<endl;
	*progVars.fp<<"  FFF       OOO  OOO  RRR  RRR  NN  NNNN  UUU  UUU     TT     SSS  SSS  "<<endl;
	*progVars.fp<<"  FFF       OOOOOOOO  RRR   RR  NN   NNN  UUUUUUUU     TT     SSSSSSSS  "<<endl;
	*progVars.fp<<"  FFF       OOOOOOOO  RRR   RR  NN   NNN  UUUUUUUU     TT     SSSSSSSS  "<<endl;
	*progVars.fp<<endl;
	*progVars.fp<<"a FORest NUTrient cycling Simulator"<<endl;
	*progVars.fp<<"  ---    ---              -"<<endl;
	*progVars.fp<<endl;
}

//Function to enter data manually.
void inputManually(vars & progVars)
{
	//Gets Age
	*progVars.fp<<"Enter the initial stand age in years --->"<<endl;
	cin>>progVars.AGE;

	//Gets Site Index
	*progVars.fp<<"Enter the site index with a base age of 25"<<endl;
	*progVars.fp<<"(Choices are: 45, 55, 65, and 75) --->"<<endl;
	cin>>progVars.SI;

	//Gets rotation length
	do
	{
		*progVars.fp<<"Enter the desired rotation length in years --->"<<endl;
		cin>>progVars.ROTATN;

		if(progVars.ROTATN>70)
		{
			*progVars.fp<<"Warning: model not valid for rotations greater than 70"<<endl;
		}
	}while(progVars.ROTATN>70);

	//Gets number of rotations
	do
	{
		*progVars.fp<<"Enter the number of rotations (max of 5) that you"<<endl;
		*progVars.fp<<"Would like to simulate --->"<<endl;
		cin>>progVars.NUM;

		if(progVars.NUM>5)
		{
			*progVars.fp<<"You entered more than 5 rotations;"<<endl;
			*progVars.fp<<"Re-enter a number less than 6 --->"<<endl;
		}
	}while(progVars.NUM>5);

	//Gets age to fertilize
	*progVars.fp<<"Enter the stand age at which you want to"<<endl;
	*progVars.fp<<"Fertilize. if you do not want to fertilize, enter 0 --->"<<endl;
	cin>>progVars.FERAGE;

	//Gets fertilizer rate
	*progVars.fp<<"Enter the fertilizer rate: 100,200,or 300 kg/ha."<<endl;
	*progVars.fp<<"If you are not going to fertilize, enter 0 --->"<<endl;
	cin>>progVars.FERATE;

	//Gets age to burn
	*progVars.fp<<"Enter the age when you first want to prescribe burn."<<endl;
	*progVars.fp<<"If you do not want to prescribe burn, enter 0 --->"<<endl;
	cin>>progVars.PBBURN;

	//Gets frequency to burn
	*progVars.fp<<"Enter the frequency at which you want to prescribe"<<endl;
	*progVars.fp<<"Burn. if you do not want to prescribe burn enter 0 --->"<<endl;
	cin>>progVars.FREQBN;

	//Gets temp to burn
	*progVars.fp<<"How hot will your prescribed burn be? enter 2 for"<<endl;
	*progVars.fp<<"Hot, 1 for warm. if you are not going to prescribe burn, enter 0 --->"<<endl;
	cin>>progVars.BURNTP;

	//Gets type of harvest method
	*progVars.fp<<"Type of harvest method. enter 1 for whole tree,"<<endl;
	*progVars.fp<<"2 for stem only --->"<<endl;
	cin>>progVars.HARVT;

	//Gets rather or not to broadcast burn the slash after harvest
	*progVars.fp<<"Do you want to broadcast burn the slash after"<<endl;
	*progVars.fp<<"Harvest? enter 1 for yes, 0 for no --->"<<endl;
	cin>>progVars.SHBURN;

	//Gets temp to burn
	*progVars.fp<<"How hot will your broadcast burn be? enter 2 for"<<endl;
	*progVars.fp<<"Hot, 1 for warm. if not going to broadcast burn,"<<endl;
	*progVars.fp<<"Enter 0 --->"<<endl;
	cin>>progVars.SBURNT;

	//Gets % of Nitrogen reabsorbed from the leaves prior to abscission
	*progVars.fp<<"Enter the percent of nitrogen reabsorbed from the"<<endl;
	*progVars.fp<<"Leaves prior to abscission. you must include a decimal "<<endl;
	cin>>progVars.LTRANS;

	//Gets % of Nitrogen reabsorbed from the leaves from the fine roots prior to senescence
	*progVars.fp<<"Enter the percent of nitrogen reabsorbed from the"<<endl;
	*progVars.fp<<"Fine roots prior to senescence. you must include a "<<endl;
	*progVars.fp<<"Decimal point  --->"<<endl;
	cin>>progVars.RTRANS;

	//Gets wait ratio of fine roots to foliage
	*progVars.fp<<"Enter the weight ratio of fine roots to foliage,"<<endl;
	*progVars.fp<<"Expressed as a percentage. you must include a "<<endl;
	*progVars.fp<<"Decimal point  --->"<<endl;
	cin>>progVars.FRFPRO;

	//Gets total soil carbon pool
	*progVars.fp<<"Enter the total soil carbon pool in kg/ha --->"<<endl;
	cin>>progVars.SOILC;

	//Gets the total nitrogen pool
	*progVars.fp<<"Initialize the total nitrogen pool in kg/ha --->"<<endl;
	cin>>progVars.ISOILN;

	//Gets % weight content of nitrogen in leaves
	*progVars.fp<<"Enter the percent weight content of nitrogen"<<endl;
	*progVars.fp<<"In leaves. you must include a decimal point  --->"<<endl;
	cin>>progVars.LEAFNF;

	//Gets % weight content of nitrogen in fine roots
	*progVars.fp<<"Enter the percent weight content of nitrogen"<<endl;
	*progVars.fp<<"In fine roots. you must include a decimal point  --->"<<endl;
	cin>>progVars.ROOTNF;

	//Gets annual input of nitrogen from atmospheric sources and from biological n fixation
	*progVars.fp<<"Enter the annual input of nitrogen from atmospheric"<<endl;
	*progVars.fp<<"Sources and from biological n fixation (kg/ha) --->"<<endl;
	cin>>progVars.NINPUT;

	//Gets annual output of nitrogen from leaching beyond the rooting zone
	*progVars.fp<<"Enter the annual output of nitrogen from leaching"<<endl;
	*progVars.fp<<"Beyond the rooting zone (kg/ha) --->"<<endl;
	cin>>progVars.NOUTPUT;
}
void printCredits(vars & progVars)
{
	//printIntro();   is this needed???
	*progVars.fp<<"Original FORNUTS Program:"<<endl;
	*progVars.fp<<"Stephen C. Hart - Fall 1983"<<endl;
	*progVars.fp<<"John M. Pye - Winter 1985"<<endl;
	*progVars.fp<<endl;
	*progVars.fp<<"Current FORNUTS Program:"<<endl;
	*progVars.fp<<"Karl Ott"<<endl;
	*progVars.fp<<"Matt Van Veldhuizen"<<endl;
	*progVars.fp<<"Syler Clayon"<<endl;
	*progVars.fp<<endl;
	*progVars.fp<<"Logo:"<<endl;
	*progVars.fp<<"Azara Mohammadi"<<endl;
	*progVars.fp<<endl;
}
