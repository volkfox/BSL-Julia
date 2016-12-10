#ifndef BAYES_H
#define BAYES_H


#include <iostream>
#include "hello.h"
#include "console.h"
#include <fstream>
#include "tokenscanner.h"
#include <iomanip>
#include <string>
#include "strlib.h"
#include "map.h"
#include <cstring>
#include "set.h"
using namespace std;

struct Inherited {
    string ISO_CODE;
    int year;
};

struct Exceptions {
    int type;
    int year;
};

/* global variables */
Map<string, Map <int, double* > >  indicators;  //  ISO_CODE ->  Map <year, array>
Vector<string> indicatorNames;
Map<string, string> countries;
Map<string, string> iso_codes;
Map <string, Inherited> inheritance;
Map <string, Exceptions> exceptions;
Set<int> coveredYears;
Set<string> excludedCountries;
Set<string> vanishedCountries;
Map<string, string> WBvariables;

Map<string,string> vanishedISOs;
Map<string, string> newISO_CODES;

/* Domain-specific piece */

const int INDICATORS=70;
const int VARIABLES_PER_LINE=10;
const double PSEUDO_SPACE=-666;

const int ECON_VARIABLES=5;
const int POLITY_VARIABLES=11;
const int POLITY_MISC_VARIABLES=4;
const int DIFF_VARIABLES=9;

const int PER_CAPITA_ADUSTMENT_VARIABLES=2;

const int ANARCHY=-66;
const int TRANSITION=-77;
const int OCCUPATION=-88;

const int INDEX_ANARCHY=6;
const int INDEX_TRANSITION=7;
const int INDEX_OCCUPATION=8;
const int EXCEPTION_DURATION=10;

const string EXPORT_FILE= "bayes.csv";

const string FRASER_FILE= "economic-freedom-of-the-world-2015-dataset.csv";
const string POLITY_IV_FILE="p4v2014d.csv";


const string FV1="Size of Govt";
const string FV2="Law & Prop.Rights";
const string FV3="Sound Money";
const string FV4="Export trade freedom";
const string FV5="Regulations";

const string PL1="polity";

const string PL2="anarchy";
const string PL3="transition";
const string PL4="occupation";

const string PL5="democ";
const string PL6="autoc";
const string PL7="xrreg";
const string PL8="xrcomp";
const string PL9="xropen";
const string PL10="xconst";
const string PL11="parreg";
const string PL12="parcomp";
const string PL13="exrec";
const string PL14="exconst";
const string PL15="polcomp";

const string DE1 = "prev Govt size";
const string DE2 = "prev Law";
const string DE3 = "prev Money";
const string DE4 = "prev Export trade";
const string DE5 = "prev Regulations";

const string DP1 = "prev polity";
const string DP2 = "prev democ";
const string DP3 = "prev autoc";
const string DP4 = "prev GDP";


/* this is a bit ugly but the only place I could think of*/

void initialAssumptions() {

    inheritance["AZE"]={"RUS", 1991};
    inheritance["ARM"]={"RUS", 1991};
    inheritance["BLR"]={"RUS", 1991};
    inheritance["EST"]={"RUS", 1991};
    inheritance["GEO"]={"RUS", 1991};
    inheritance["LVA"]={"RUS", 1991};
    inheritance["LTU"]={"RUS", 1991};
    inheritance["MDA"]={"RUS", 1991};
    inheritance["TJK"]={"RUS", 1991};
    inheritance["TKM"]={"RUS", 1991};
    inheritance["KGZ"]={"RUS", 1991};
    inheritance["KAZ"]={"RUS", 1991};
    inheritance["UKR"]={"RUS", 1991};
    inheritance["UZB"]={"RUS", 1991};
    inheritance["RUS"]={"USR", 999};

    inheritance["TLS"]={"IDN", 2002};

    inheritance["BIH"]={"YGS", 1992};
    inheritance["MNE"]={"SRB", 2006};
    inheritance["SRB"]={"YGS", 2006};
    inheritance["SVN"]={"YUG", 1992};
    inheritance["CRO"]={"YUG", 1992};
    inheritance["MKD"]={"YUG", 1992};
    inheritance["YGS"]={"YUG", 1991};

    inheritance["NAM"]={"ZAF", 1990};

    exceptions["RUS"]={INDEX_TRANSITION, 1990};
    exceptions["AZE"]={INDEX_TRANSITION, 1990};
    exceptions["ARM"]={INDEX_TRANSITION, 1990};
    exceptions["BLR"]={INDEX_TRANSITION, 1990};
    exceptions["EST"]={INDEX_TRANSITION, 1990};
    exceptions["GEO"]={INDEX_TRANSITION, 1990};
    exceptions["LVA"]={INDEX_TRANSITION, 1990};
    exceptions["LTU"]={INDEX_TRANSITION, 1990};
    exceptions["MDA"]={INDEX_TRANSITION, 1990};
    exceptions["TJK"]={INDEX_TRANSITION, 1990};
    exceptions["TKM"]={INDEX_TRANSITION, 1990};
    exceptions["KGZ"]={INDEX_TRANSITION, 1990};
    exceptions["KAZ"]={INDEX_TRANSITION, 1990};
    exceptions["UKR"]={INDEX_TRANSITION, 1990};
    exceptions["UZB"]={INDEX_TRANSITION, 1990};

    exceptions["BIH"]={INDEX_ANARCHY, 1995};
    exceptions["MNE"]={INDEX_TRANSITION, 2006};
    exceptions["SRB"]={INDEX_TRANSITION, 1990};
    exceptions["SVN"]={INDEX_TRANSITION, 1990};
    exceptions["CRO"]={INDEX_TRANSITION, 1990};
    exceptions["MKD"]={INDEX_TRANSITION, 1990};
    exceptions["YGS"]={INDEX_TRANSITION, 1992};



    newISO_CODES["ALG"]="DZA";
    newISO_CODES["ANG"]="AGO";
    newISO_CODES["AUL"]="AUS";
    newISO_CODES["AUS"]="AUT";
    newISO_CODES["BAH"]="BHR";
    newISO_CODES["BFO"]="BFA";
    newISO_CODES["BHU"]="BTN";
    newISO_CODES["BNG"]="BGD";
    newISO_CODES["BOS"]="BIH";
    newISO_CODES["BOT"]="BWA";
    newISO_CODES["BUI"]="BDI";
    newISO_CODES["BUL"]="BGR";
    newISO_CODES["CAM"]="CMR";
    newISO_CODES["CAM"]="KHM";
    newISO_CODES["CAO"]="CMR";
    newISO_CODES["CAP"]="CPV";
    newISO_CODES["CEN"]="CAF";
    newISO_CODES["CHA"]="TCD";
    newISO_CODES["CMR"]="KHM";
    newISO_CODES["CON"]="COG";
    newISO_CODES["COS"]="CRI";
    newISO_CODES["CRO"]="HRV";
    newISO_CODES["CZR"]="CZE";
    newISO_CODES["DEN"]="DNK";
    newISO_CODES["ETI"]="ETH";
    newISO_CODES["ETM"]="TLS";
    newISO_CODES["FRN"]="FRA";
    newISO_CODES["GAM"]="GMB";
    newISO_CODES["GMY"]="DEU";
    newISO_CODES["GFR"]="DEU";
    newISO_CODES["GRG"]="GEO";
    newISO_CODES["GUA"]="GTM";
    newISO_CODES["GUI"]="GIN";
    newISO_CODES["HAI"]="HTI";
    newISO_CODES["HON"]="HND";
    newISO_CODES["INS"]="IDN";
    newISO_CODES["IRE"]="IRL";
    newISO_CODES["KUW"]="KWT";
    newISO_CODES["KYR"]="KGZ";
    newISO_CODES["KZK"]="KAZ";
    newISO_CODES["LAT"]="LVA";
    newISO_CODES["LEB"]="LBN";
    newISO_CODES["LES"]="LSO";
    newISO_CODES["LIB"]="LBY";
    newISO_CODES["LIT"]="LTU";
    newISO_CODES["MAA"]="MRT";
    newISO_CODES["MAC"]="MKD";
    newISO_CODES["MAG"]="MDG";
    newISO_CODES["MAL"]="MYS";
    newISO_CODES["MAS"]="MUS";
    newISO_CODES["MAW"]="MWI";
    newISO_CODES["MLD"]="MDA";
    newISO_CODES["MNT"]="MNE";
    newISO_CODES["MON"]="MNG";
    newISO_CODES["MOR"]="MAR";
    newISO_CODES["MYA"]="MMR";
    newISO_CODES["MZM"]="MOZ";
    newISO_CODES["NEP"]="NPL";
    newISO_CODES["NEW"]="NZL";
    newISO_CODES["NIG"]="NGA";
    newISO_CODES["NIR"]="NER";
    newISO_CODES["NTH"]="NLD";
    newISO_CODES["OMA"]="OMN";
    newISO_CODES["PAR"]="PRY";
    newISO_CODES["PHI"]="PHL";
    newISO_CODES["PKS"]="PAK";
    newISO_CODES["POR"]="PRT";
    newISO_CODES["ROK"]="KOR";
    newISO_CODES["RUM"]="ROU";
    newISO_CODES["SAF"]="ZAF";
    newISO_CODES["SAL"]="SLV";
    newISO_CODES["SER"]="SRB";
    newISO_CODES["SIE"]="SLE";
    newISO_CODES["SIN"]="SGP";
    newISO_CODES["SLO"]="SVK";
    newISO_CODES["SLV"]="SVN";
    newISO_CODES["SPN"]="ESP";
    newISO_CODES["SRI"]="LKA";
    newISO_CODES["SWA"]="SWZ";
    newISO_CODES["SWD"]="SWE";
    newISO_CODES["SWZ"]="CHE";
    newISO_CODES["TAJ"]="TJK";
    newISO_CODES["TAW"]="TWN";
    newISO_CODES["TAZ"]="TZA";
    newISO_CODES["THI"]="THA";
    newISO_CODES["TOG"]="TGO";
    newISO_CODES["TRI"]="TTO";
    newISO_CODES["UAE"]="ARE";
    newISO_CODES["UKG"]="GBR";
    newISO_CODES["URU"]="URY";
    newISO_CODES["VIE"]="VNM";
    newISO_CODES["YAR"]="YEM";
    newISO_CODES["ZAM"]="ZMB";
    newISO_CODES["ZAI"]="COD";
    newISO_CODES["ZIM"]="ZWE";


    coveredYears+= 1970, 1975, 1980, 1985, 1990, 1995, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
            2007, 2008, 2009, 2010, 2011, 2012, 2013;

    excludedCountries+="BIH", "USR", "SRB", "YGS", "BRD", "TWN";

    vanishedCountries+="USR", "YGS";
    vanishedISOs["USR"]="USSR";
    vanishedISOs["YGS"]="Yugoslavia";

    // high-tech
    WBvariables["WB4"]="Research and development expenditure (percent of GDP)";
    WBvariables["WB18"]="High-technology exports percent of manufactured exports";
    WBvariables["WB20"]="Energy use kg of oil equivalent per capita";

    // fintech per capita
    WBvariables["WB2"]="foreign net aid in USD per capita";
    WBvariables["WB1"]="Use of IMF credit (current USD) per capita";

    // fintech
    WBvariables["WB14"]="Lending interest rate percent";
    WBvariables["WB12"]="Total debt service as percent of exports of goods services and primary income";

    // gdp composure
    WBvariables["WB27"]="GDP growth percent";
    WBvariables["WB28"]="GDP per capita in USD";
    WBvariables["WB16"]="Agriculture percent of GDP";

    WBvariables["WB3"]="Industry value added (percent of GDP)";

    // social
    WBvariables["WB19"]="Urban population (% of total)";
    WBvariables["WB10"]="Income share held by highest ten percent";
    WBvariables["WB26"]="Employment in agriculture (percent of total employment)";
    WBvariables["WB15"]="Employment to population ratio total percent";
    WBvariables["WB0"]="Population total";
    WBvariables["WB29"]="Net secondary education enrollment";
    WBvariables["WB30"]="Gross tertiary education enrollment percent";

    // health & pop
    WBvariables["WB21"]="Life expectancy at birth total in years";
    WBvariables["WB23"]="Birth rate per 1000 people";
    WBvariables["WB24"]="Population growth annual percent";

    // regulations
    WBvariables["WB5"]="Start-up procedures to register a business (number)";
    WBvariables["WB25"]="Time required to start a business in days";
    WBvariables["WB7"]="Lead time to import median case in days";
    WBvariables["WB9"]="Lead time to export median case in days";
    WBvariables["WB8"]="Logistics performance index";

    WBvariables["WB6"]="Inflation in consumer prices (annual percent)";

    // econ
    WBvariables["WB22"]="GDP per person employed constant 1990 PPP";

    // agro
    WBvariables["WB11"]="Cereal yield kg per hectare";

    // govt
    WBvariables["WB13"]="Military expenditure percent of central government expenditure";
    WBvariables["WB17"]="Tax revenue percent of GDP";

    WBvariables["WB31"]="EU member";
    WBvariables["WB32"]="Informal payments to public officials (percent of firms)";
    WBvariables["WB33"]="CPIA Public sector management quality";
    WBvariables["WB34"]="Fuel exports (% of merchandise exports)";


}


/* functions  */


void readPolity ();
void readEconomicFreedom ();
void fillInheritance();
bool scanDoubles (double * &indicatorArray, int num, int fromIndex, int csvLine, TokenScanner &scanner);
void publish(int year);
void publish(string ISO_CODE);
bool nonNegative (double * array);
void findPartial();
void printVariableNames ();
void printIndicators (string ISO_CODE, int year);
void fillGapsFreedom();
void findFullYears();
void killCountries();
void buildTimeline (Vector<string> &countryVector, int * timeline);
void parsePolityTable (Map<string, Vector<string> > &polity4table);
void fillPolity(double * indicatorArray, string str);
void fillGapsPolity();
void exportCSV();
void printHeaders( ofstream &outfile);
bool PolityEmpty(string ISO_CODE, int year);
void updateExceptions ();
void updateException (string ISO_CODE, int year, int index_exception);
void updateExternalExceptions();
void countYears (string str, Vector<int> &aidYears , string AID_FILE);
void parseAidTable(Map <string, string> &aidTable, Vector <int> &aidYears, int BASE_INDEX);
void calculateDiffs();

void readAid (string AID_FILE, int BASE_INDEX, string varname);

void readWBData ();

void postProcess();



#endif // BAYES_H
