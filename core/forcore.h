#ifndef FORCORE_H
#define FORCORE_H
#include <iostream>
#include <fstream>
#include <math.h>
using std::ofstream;
using std::ostream;
using std::cout;
using std::endl;
using std::cin;
using std::ifstream;
#include <string>
using std::string;


extern "C" {
#include "dictionary.h"
#include "iniparser.h"
}

struct vars
{
	double LBMAS, BBMAS, FRBMAS, CRBMAS, SOILC, RTRANS, LTRANS, 
		NINPUT, NOUTPUT, LEAFNF, ROOTNF, FRFPRO, PHSAGE, PBFAC, 
		FERFAC, ISOILN, SBMAS, NAMT, MINRL, NAVAIL, LBN, 
        BBN, FRBN, CRBN, SBN, XPONNT, TRANS, NREQ, NREMSL, NRMSCR,
		NREMSS, NREMSB, NPBURN, NSBURN, RAINNS, FERTNS, NETDNS, 
		OUTNS, TLREM, TBREM, TSREM,	TCRREM, NREMSI, TBMNRS, TREM, 
		ENTRNR, NETDNR,	CNRATO, NRUBR, NRUBS;

	int	AGE, FERATE, FREQBN, ROTATN, FINISH, SI, PBBURN,
		BURNTP, SBURNT, HARVT, ROTNUM, ISAGE, FTRIG, PBTRIG, NUM,
		FERAGE, SHBURN, BEGIN, SIMTIM, MARKER;

    static int const arrSize = 6;

	//array of floats
	double SOILN[arrSize], NREMOV[arrSize], NREMB[arrSize], 
		NREML[arrSize],	NREMS[arrSize], NREMCR[arrSize], 
		NPBHAR[arrSize], NSBHAR[arrSize], RAINNR[arrSize], 
		FERTNR[arrSize], OUTNR[arrSize], HLBMAS[arrSize], 
		HBBMAS[arrSize], HSBMAS[arrSize], HCRMAS[arrSize], 
		HTREM[arrSize];

	string inputFileName;
	string saveInputFileName;
	string outputFileName;
	bool inputFromFile;
	bool saveInputToFile;
	bool interactiveMode;

	ostream* fp;
};

//Prototypes

void inputFromFile(vars & );
void saveInputToFile(vars & );
void pburn( vars & );
void fert( vars & );
void availN( vars &, int);
void harv(vars &); 
void slashb(vars &);
void litter(vars &);
void annual(vars &);
string checkInput(vars &);
void saveOutputCSV(vars &);
#endif
