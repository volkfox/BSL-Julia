/*
 *
 * This is quick and dirty spaghetti code to stitch together Fraser, Polity and
 * WorldBank databases. It does the following work:
 *
 * - unrolls Polity records from coding format
 * - matches/remaps country ISO codes between three databases
 * - cleans up country split/mergers
 * - picks up any new WB metrics added at the end
 *
 * Indicator fields:
 *
 * "Fraser Economic Freedom"  |    "Polity IV Systemic Peace"
 *     0    1   2   3    4    5        6 (66)      7 (77)      8  (88)      9
 *     A1   A2  A3  A4   A5   polity   occupation  anarchy     transition   democ
 *
 *  "Polity |V (continued) "
 *  10       11     12      13      14      15      16      17      18      19
 *  autoc    xrreg	xrcomp	xropen	xconst	parreg	parcomp	exrec	exconst	polcomp
 *
 *
 */

#include "bayes.h"
#include <algorithm>
#include "random.h"

#define _getIsoCode  { string temp = scanner.nextToken();  \
    if (newISO_CODES.containsKey(temp)) ISO_CODE=newISO_CODES[temp]; else ISO_CODE=temp;\
    }


#define _tokenScanner {  \
    scanner.addWordCharacters("_  .'-&()$");   \
    scanner.ignorePunctuation();    \
    scanner.scanStrings();   \
    scanner.scanNumbers(); \
    }

int main () {

    initialAssumptions();

    /* fill the Fraser dataset here */
    readEconomicFreedom();
    /* adjust for split/merger cases */
    fillGapsFreedom();  
    /* integrate polity data */
    readPolity();
    fillGapsPolity();
    indicatorNames+=DE1, DE2, DE3, DE4, DE5, DP1, DP2, DP3, DP4;
    /* integrate WB data */
    readWBData();
    /* add previous year fields for temporal learning */
    calculateDiffs();

    postProcess();
    //publish(1980);

    /* remove countries known for dirty data */
    killCountries();

    /* publish CSV file for learning */
    exportCSV();

    cout << "Finished. " << endl;

    return 0;
}

/* this function writes CSV comma separated output file */
void exportCSV () {

    ofstream outfile;
    outfile.open(EXPORT_FILE);   
    printHeaders(outfile);


    int count=indicatorNames.size();

    for (string ISO_CODE : countries) {

        for (int year : coveredYears) {

          double * indicatorArray;
            //cerr << "processing country" << ISO_CODE << endl;

          if (indicators.get(ISO_CODE).containsKey(year)) {

            indicatorArray = indicators.get(ISO_CODE).get(year);
            outfile << fixed << countries.get(ISO_CODE) << "," << ISO_CODE << "," << year << "," ;
            for (int i=0; i<count; i++ ) {

                    // outfile << indicatorArray[i];
                    if (indicatorArray[i]!=(PSEUDO_SPACE)) outfile << indicatorArray[i];

                    // if ( i < (count-1) ) outfile << "," ;
                    outfile << "," ;
             }
                outfile << "," << randomReal(0,1)  << endl;
           }
        }
    }

    outfile.flush();
    outfile.close();

}

/* helper with three non-indicator fields */
void printHeaders( ofstream &outfile) {

    outfile << "Country,Code,Year,";

    for (string indicator: indicatorNames) {
        outfile << indicator;
        outfile << "," ;
    }
    outfile << ", Random seed";
    outfile << endl;
}


/* remove blacklisted countries from DS */

void killCountries() {

    for (string country: excludedCountries) {
        indicators.remove(country);
        countries.remove(country);
    }
}

/* helper function: show which years have complete country info */
void findFullYears() {

    bool fullYear;
    for (int year : coveredYears) {
        fullYear=true;
        for (string ISO_CODE: countries) {

           if ( !indicators.get(ISO_CODE).containsKey(year) ) {

            fullYear=false;
            cout << "First violator in year " <<  year << ": " << countries.get(ISO_CODE) << endl;
                break;
           }
        }
        if (fullYear) cout << "full year: " << year << endl;
    }
}

/* prints country names with data gaps */
void findPartial() {

    for (string ISO_CODE: countries) {

        if (indicators.get(ISO_CODE).size()!=coveredYears.size()) {

            cout << "Missing data: " << countries.get(ISO_CODE) << endl;
        }
    }
}

/* this function interpolates inheritance in country names */
void fillGapsFreedom() {

    bool ready;

    do {

        ready=true;
        for (string ISO_CODE: countries) {   // for all countries

            if (inheritance.containsKey(ISO_CODE) && countries.containsKey(inheritance.get(ISO_CODE).ISO_CODE)) {   // if there was a prior state...

                string PARENT_ISO_CODE=inheritance.get(ISO_CODE).ISO_CODE;

                Map <int, double *> indicatorMap=indicators.get(ISO_CODE);
                Map <int, double *> extendedMap=indicators.get(PARENT_ISO_CODE);

                for (int year: extendedMap) {

                    if (!indicatorMap.containsKey(year))  {

                        double * temp = new double [INDICATORS];
                        for (int i=0; i<INDICATORS; i++) temp[i]=extendedMap.get(year)[i];
                        indicatorMap.add(year, temp);
                        ready=false;   // there might be changes to parent after this, so need another pass
                    }

                }

                indicators.add(ISO_CODE, indicatorMap);

            }
        }
    } while (!ready);


}

/* debug function dumping all we know about the country */
void publish(string ISO_CODE) {

    if (!indicators.containsKey(ISO_CODE)) throw "no such ISO code";
    cout << endl << "Country: " << countries[ISO_CODE] << endl << endl;

    //indicatorNames.insert(0,"Year");

    printVariableNames ();

    Map <int, double* > indicatorMap=indicators.get(ISO_CODE);


    for (int year : indicatorMap) {
        cout << year << endl;
        cout << setw(indicatorNames[0].size()+2);
        printIndicators(ISO_CODE, year);
    }

    //indicatorNames.remove(0);
    cout << endl;
}

/* debug function dumping all we know about the year */
void publish(int year) {

    cout << "Indicators for year " << year << endl << endl;
    indicatorNames.insert(0,"ISO");

    printVariableNames ();

    for (string ISO_CODE : countries) {


        if (indicators.get(ISO_CODE).containsKey(year)) {

            cout << ISO_CODE << endl;
            cout << setw(indicatorNames[0].size()+2);
            printIndicators(ISO_CODE, year);
        }

    }
    indicatorNames.remove(0);
}

/* debug function dumping all variables names we learned so far */
void printVariableNames () {

    int i=0;
    for (string variable :  indicatorNames) {
        if (i%VARIABLES_PER_LINE==0) cout << endl;
        int size = indicatorNames[i%VARIABLES_PER_LINE].size()+2;
        cout << setw(size) << variable;
        i++;
    }
    cout << endl << endl;
}

/* print all indicators per ISO code per year */
void printIndicators (string ISO_CODE, int year) {

    double * indicatorArray= indicators.get(ISO_CODE).get(year);

    for ( int i=0; i< (indicatorNames.size()); i++) {

        if (i%VARIABLES_PER_LINE==0) cout << endl;
        int size = indicatorNames[i%VARIABLES_PER_LINE].size()+2;

        cout << setw(size) << indicatorArray[i];

    }
    cout << endl;
}

/* watchdog for poisoned data entries */
bool nonNegative (double * array) {

    bool positive=true;
    for (int i=0; i<INDICATORS; i++) {
        if (array[i]<0) positive=false;
    }
    return positive;
}


/*
 * Primary data source: Economic_freedom_of_the_world_2015.csv, Fraser Institute
 * http://www.freetheworld.com/2015/economic-freedom-of-the-world-2015-dataset.xlsx
 *
 * File is adjusted for gaps in the former USSR estimations
 * File is adjusted for removal of NAM prior to 1990
 *
 * file format: CSV free of MS Windows ^M symbols
 * first line is variable names, second line starts with a year number followed by ISO_CODE and variable of interest
 *
 * sorting order of lines can be different!
 *
 * Example from Excel:
 *
 * Year	ISO_Code	Countries	Size of Government	Legal System & Property Rights	Sound Money	Freedom to trade internationally	 Regulation
 * 2013	AGO         Angola      5.34                3.21                            6.72        5.93                                    5.67
 * 2012	AGO         Angola      4.93                3.37                            6.56        6.14                                    5.81
 *
 */

void readEconomicFreedom () {

    ifstream infile;
    string str;
    int lineCounter=1;

    // these names define consequitive variables we plan to read

    indicatorNames+=FV1,FV2,FV3,FV4,FV5;

    infile.open(FRASER_FILE);   // we assume the file is allright

    getline(infile, str);       // ignore first line with headers

    while (getline(infile, str))  {

        lineCounter++;          // track CSV line numbers for diag, starting with 2 (1st line is variable names)

        TokenScanner scanner(str);
        _tokenScanner;

        int year;
        string ISO_CODE;
        string countryName;
        // this is the 1st file to process so we prepare the main data structures here

        double * indicatorArray = new double [INDICATORS];
        memset(indicatorArray, 0, sizeof(double) * INDICATORS);
        Map <int, double*> indicatorMap;

        string token=scanner.nextToken();
        //cerr << token << endl;  // testblock: token in situ for the leading year
        try {
            year= stoi(token);
        }
        catch (const std::exception& e) {
            cerr << e.what();
            cerr << "line:" << lineCounter << " Could not convert a year" << endl;
            exit(1);
        }

        ISO_CODE=scanner.nextToken();

        countryName=scanner.nextToken();

        if (!scanDoubles(indicatorArray, indicatorNames.size(), 0, lineCounter, scanner)) continue;
        // if double variables fail, not much to do in this line anymore

        if (!countries.containsKey(ISO_CODE)) {

            countries.add(ISO_CODE, countryName);   // first year in a given country
            iso_codes.add(countryName, ISO_CODE);

        } else {

            indicatorMap=indicators.get(ISO_CODE);  // else add to existing data
        }

        indicatorMap.add(year, indicatorArray);
        indicators.add(ISO_CODE, indicatorMap);

        // how did we do? testblock

        /*               cout << year << " " << ISO_CODE << " " << countryName << " " <<
                 indicators.get(ISO_CODE).get(year)[0] << ":" << indicators.get(ISO_CODE).get(year)[1]
                 << ":" << indicators.get(ISO_CODE).get(year)[2]
                 << ":" << indicators.get(ISO_CODE).get(year)[3] << ":" << indicators.get(ISO_CODE).get(year)[4] << endl;
*/
    }

    infile.close();   // done reading file

    // now get placeholders for vanished countries

    for (string vanishedCountry: vanishedCountries) {


        Map <int, double*> indicatorMap;

        for (int year : coveredYears) {

            double * indicatorArray = new double [INDICATORS];
            memset(indicatorArray, 0, sizeof(double) * INDICATORS);

            indicatorMap.add(year, indicatorArray);
        }

        indicators.add(vanishedCountry, indicatorMap);

        countries.add(vanishedCountry,vanishedISOs[vanishedCountry]);
        iso_codes.add(vanishedISOs[vanishedCountry], vanishedCountry);
    }

}

/* scans NUM double values into indicator array starting from FROMINDEX position, file line <csvLine>, token scanner <scanner>
 * returns false if this line data is incomplete and needs to be skipped                                                             */

bool scanDoubles ( double * &indicatorArray, int num, int fromIndex, int csvLine, TokenScanner &scanner){

    bool skipLine=true;

    for (int i=0; i<num ; i++) {

        string token=scanner.nextToken();

        try {

            if (token != "") {

                double variable=stod(token);
                indicatorArray[fromIndex+i]=(variable>0)?variable:0; //  remove -88/-77/-66 etc
                skipLine=false;   // there is at least some data

            }  else {

                skipLine = true;  break;     // kill partial data
                //indicatorArray[fromIndex+i]=PSEUDO_SPACE;   // preserve partial data
            }
        }
        catch (const std::exception& e) {
            cerr << e.what();
            cerr << endl << "CSV file line:" << csvLine << " token: " << token << endl;
            exit(1);
        }
    }
    return !skipLine;
}





/*
 * scode	country	polity	persist	bmonth	bday	byear	emonth	eday	eyear	democ	autoc	xrreg	xrcomp	xropen	xconst	parreg	parcomp	exrec	exconst	polcomp
 * AFG	Afghanistan	-6		1	1	1800	6	30	1935	1	7	3	1	1	1	3	3	1	1	6
 * AFG	Afghanistan	-8	10	7	1	1935	6	30	1945	0	8	3	1	1	1	3	2	1	1	3
 * AFG	Afghanistan	-10	8	7	1	1945	9	7	1953	0	10	3	1	1	1	4	1	1	1	1
 *
 * We assume the CSV file sorted by country (ISO code)
 */

void readPolity () {

    ifstream infile;
    string str;
    Set<string> unusedCountries;
    // these names define consequitive variables we plan to read

    indicatorNames+=PL1,PL2,PL3,PL4,PL5,PL6,PL7,PL8,PL9,PL10,PL11,PL12,PL13,PL14,PL15;

    infile.open(POLITY_IV_FILE);   // we assume the file is allright

    getline(infile, str);          // ignore first line with headers

    // read the whole damn thing into memory

    Map <string, Vector<string>> polity4table;

    while (getline(infile, str))  {

        TokenScanner scanner(str);
        _tokenScanner;

        string ISO_CODE;

        //string ISO_CODE=scanner.nextToken();
        _getIsoCode;  // correct junk ISO Codes

        string countryName=scanner.nextToken();


        if (countries.containsKey(ISO_CODE)) {

            Vector<string> countryStrings=polity4table[ISO_CODE];
            countryStrings.add(str);
            polity4table[ISO_CODE]=countryStrings;

            if (countryName!=countries.get(ISO_CODE)) {

                unusedCountries.add(ISO_CODE);

                //if (!unusedCountries.contains(ISO_CODE))
                // cerr << "readPolity: country name discrepancy: read "  <<  ISO_CODE << " as \"" <<  countryName << "\" while it's actually " << countries.get(ISO_CODE) << endl;
            }

        } else {  // we did not store such country in the previous phase

            if (!unusedCountries.contains(ISO_CODE))

            {
                unusedCountries.add(ISO_CODE);
                // cerr << "readPolity: skipping over country: " << ISO_CODE << ": " << countryName << endl;
            }
        }
    } // end of file reading

    infile.close();

    parsePolityTable(polity4table);
}


void countYears (string str, Vector<int> &aidYears, string AID_FILE ) {

    string yearS;
    TokenScanner scanner(str);
    _tokenScanner;

    for (int i=0; i<4; i++) yearS=scanner.nextToken(); // skip 4 columns at start

    while (scanner.hasMoreTokens()) {

        yearS = scanner.nextToken();
        // debug: cerr << " year: \" " << yearS << "\" " ;
        int yearI;

        try {
            yearI= stoi(yearS);

        }
        catch (const std::exception& e) {
            cerr << e.what();
            cerr << " Could not scan year of foreign aid: " <<  yearS << " file " << AID_FILE << endl;
            exit(1);
        }

        aidYears+=yearI;
    }
}


void readWBData () {

    int BASE_INDEX=ECON_VARIABLES+POLITY_VARIABLES+POLITY_MISC_VARIABLES+DIFF_VARIABLES;

    for (int i=0; i<WBvariables.size(); i++) {

        string AID_FILE;
        string WBName;
        WBName +="WB"+integerToString(i);
        AID_FILE=WBName+".csv";

        AID_FILE=AID_FILE.c_str();

        readAid(AID_FILE, BASE_INDEX+i, WBvariables[WBName]);
    }
}







void readAid (string AID_FILE, int BASE_INDEX, string varname) {

    ifstream infile;
    string str;
    Vector <int> aidYears;

    // these names define consequitive variables we plan to read

    indicatorNames+=varname;

    infile.open(AID_FILE);   // we assume the file is allright

    if (infile.fail()) { cerr << "failed opening file " << AID_FILE << endl; exit(1); }

    getline(infile, str);          // read first line to count years of aid

    countYears(str, aidYears, AID_FILE);

    // read the whole damn thing into memory

    Map <string, string> aidTable;

    while (getline(infile, str))  {

        TokenScanner scanner(str);
        _tokenScanner;

        string countryName=trim(scanner.nextToken());
        string ISO_CODE=trim(scanner.nextToken());

        //_getIsoCode;  // assume no junk codes

        if (countries.containsKey(ISO_CODE)) {

            aidTable.add(ISO_CODE, str);

            if (countryName!=countries.get(ISO_CODE)) {

                //  cout << "Warning: country name discrepancy: read "  <<  ISO_CODE << " as \"" <<  countryName << "\" while it's actually " << countries.get(ISO_CODE) << endl;
            }

        } else {  // don't know this country

            //  cerr << "Warning: skipping over unknown country: " << ISO_CODE << ": " << countryName << endl;

        }
    } // end of file reading

    infile.close();

    parseAidTable(aidTable, aidYears, BASE_INDEX);

}

void parseAidTable(Map <string, string> &aidTable, Vector <int> &aidYears, int BASE_INDEX) {

    for (string ISO_CODE :  aidTable) {

        string str=aidTable.get(ISO_CODE);

        TokenScanner scanner(str);
        _tokenScanner;

        for (int i=0; i<4; i++) str=scanner.nextToken(); // skip 4 columns at start

        string aidS;
        double aidD=0;

        for (int i=0; i<aidYears.size(); i++) {

            int yearOfAid=aidYears[i];
            aidS=scanner.nextToken();

            if (coveredYears.contains(yearOfAid)) {

                if (aidS=="") {

                    aidD=PSEUDO_SPACE;
                    //continue;

                } else {
                    try {
                        aidD= stod(aidS);

                    }
                    catch (const std::exception& e) {
                        cerr << e.what();
                        cerr << " Could not scan foreign aid amount in year: " <<  aidYears[i] << " country " << ISO_CODE << "amount: \" " << aidS << "\" " << endl;
                        exit(1);
                    }
                }
                if (indicators.get(ISO_CODE).containsKey(aidYears[i]))
                    indicators[ISO_CODE][aidYears[i]][BASE_INDEX]=aidD;

            }
        }
    }
}




void parsePolityTable (Map <string, Vector<string>> &polity4table) {

    int countryCounter=0;

    for (string ISO_CODE: countries) {

        Vector<string> countryVector;

        countryVector=polity4table.get(ISO_CODE); // vector of raw (unparsed) strings about one country
        if (countryVector.isEmpty() ) continue;                  //  we hit a country not in the Polity Table

        int * timeline = new int[coveredYears.size()];           // parse table into all the years covered by Freedom
        memset(timeline, -1, sizeof(int) * coveredYears.size()); // -1 means this year was not covered in the Polity

        buildTimeline(countryVector, timeline);                 // timeline will hold indices of Vector

        int i=-1;

        for (int year: coveredYears) {
            i++;
            if (timeline[i]==(-1)) { // skip over gaps in polity data by year

                // cerr << "Gap in polity data: " << ISO_CODE << " in the year " << year << endl;
                continue;
            }
            string str=countryVector.get(timeline[i]);


            Map <int, double*> indicatorMap = indicators.get(ISO_CODE);

            if (indicatorMap.containsKey(year)) {  // there might be years with no data for country
                double * indicatorArray=indicatorMap.get(year);

                fillPolity(indicatorArray, str);

                indicatorMap.add(year,indicatorArray);
                indicators.add(ISO_CODE, indicatorMap);

            }
        }
        delete [] timeline;
        countryCounter++;
    }
    cout << "Processed Polity IV records for " << countryCounter << " countries" << endl;
}

void buildTimeline (Vector<string> &countryVector, int * timeline) {

    int vectorCounter=0;

    for (string str : countryVector) {

        TokenScanner scanner(str);
        _tokenScanner;

        string yearBs, yearEs;
        int yearBi, yearEi;

        string ISO_CODE;

        //string ISO_CODE=scanner.nextToken();

        _getIsoCode;   // correct for junk codes

        for (int i=1; i<7; i++) yearBs = scanner.nextToken();   // yearB = 7th position, yearE = 10h position
        for (int i=0; i<3; i++) yearEs=scanner.nextToken();

        // transform

        try {
            yearBi= stoi(yearBs);
            yearEi= stoi(yearEs);

        }
        catch (const std::exception& e) {
            cerr << e.what();
            cerr << " Could not convert country:" << ISO_CODE << ", year(s) " << yearBs << " to " << yearEs << endl;
            exit(1);
        }

        int j=0;
        for (int year : coveredYears) {   // try all covered years

            if (year >= yearBi && year <= yearEi) timeline[j]=vectorCounter;
            j++;

        }
        vectorCounter++;
    }
}

void fillPolity(double * indicatorArray, string str) {

    TokenScanner scanner(str);
    _tokenScanner;

    string ISO_CODE;
    string token;

    //string ISO_CODE=scanner.nextToken();

    _getIsoCode;   // correct for junk codes

    for (int i=0; i<2; i++) token=scanner.nextToken(); // hardcoded position: polity is in column C

    try {
        indicatorArray[ECON_VARIABLES]= stoi(token);
    }
    catch (const std::exception& e) {
        cerr << e.what();
        cerr << " Error converting polity index: " << ISO_CODE << endl;
        exit(1);
    }

    for (int i=0; i<7; i++) token=scanner.nextToken();   //  hardcoded position: democ is in column K
    //  harcoded: 4 means skip over polity, occupation, transition and anarchy
    if (!scanDoubles(indicatorArray,POLITY_VARIABLES,ECON_VARIABLES+4,0,scanner)) throw "something went wrong while scanning doubles in :"+ISO_CODE ;

}

/* this function does the following:
 *
 * (a) fills Polity data in inheritance countries
 * (b) adds transition periods to ex-Soviet block countries
 * (c) expands codes 66, 77, 88 to respective indicators
 *
 */

void fillGapsPolity() {

    bool inheritance_fixed=false;

    while (!inheritance_fixed) {

        inheritance_fixed=true;

        for (string ISO_CODE : inheritance)  {  // loop thru all countries with inheritance attribute

            if (countries.containsKey(ISO_CODE) && countries.containsKey(inheritance.get(ISO_CODE).ISO_CODE)) {

                for (int year: coveredYears) {

                    double * indicatorArray=NULL;

                    string parentCountry=inheritance.get(ISO_CODE).ISO_CODE;

                    if ( PolityEmpty(ISO_CODE, year) && !PolityEmpty(parentCountry, year) )   {

                        indicatorArray = indicators.get(ISO_CODE).get(year);
                        double * parentArray = indicators.get( inheritance.get(ISO_CODE).ISO_CODE ).get(year);

                        for (int i=ECON_VARIABLES; i<indicatorNames.size(); i++) {

                            indicatorArray[i]=parentArray[i];
                        }

                        indicators[ISO_CODE][year]=indicatorArray;
                        inheritance_fixed=false;

                    }
                }
            }
        }
    }   // done with inheritance, let's deal with  exceptions now

    updateExceptions();

    updateExternalExceptions();
}

void updateExternalExceptions(){


    for (string ISO_CODE : exceptions) {

        updateException(ISO_CODE, exceptions.get(ISO_CODE).year, exceptions.get(ISO_CODE).type);

    }
}


void updateExceptions () {

    for ( string ISO_CODE:  countries)  {

        for (int year: coveredYears) {

            if (indicators.get(ISO_CODE).containsKey(year)) {

                int polity=indicators.get(ISO_CODE).get(year)[ECON_VARIABLES];

                switch (polity) {  // assume polity is #1 after ECON

                case ANARCHY:
                    updateException (ISO_CODE, year, INDEX_ANARCHY);
                    break;
                case TRANSITION:
                    updateException (ISO_CODE, year, INDEX_TRANSITION);
                    break;

                case OCCUPATION:
                    updateException (ISO_CODE, year, INDEX_OCCUPATION);
                    break;

                default: break;
                }

            }
        }
    }
}

void updateException (string ISO_CODE, int year, int index_exception) {

    for (int i=0; i<EXCEPTION_DURATION; i++) {

        if ( indicators.get(ISO_CODE).containsKey(year+i) ) indicators[ISO_CODE][year+i][index_exception]=EXCEPTION_DURATION-i;
    }
}



bool PolityEmpty(string ISO_CODE, int year) {

    bool answer=true;

    if (indicators.get(ISO_CODE).containsKey(year)) {

        double * indicatorArray = indicators.get(ISO_CODE).get(year);

        for (int i=POLITY_VARIABLES; i< indicatorNames.size(); i++) {

            if (indicatorArray[i]!=0) answer=false;
        }
    }
    return answer;
}

void postProcess () {

    int POP_INDEX=ECON_VARIABLES+POLITY_VARIABLES+POLITY_MISC_VARIABLES+DIFF_VARIABLES;

    for (int i=1; i<=PER_CAPITA_ADUSTMENT_VARIABLES; i++) {


        for (string ISO_CODE : countries) {

            for (int year : coveredYears) {

                if (indicators[ISO_CODE].containsKey(year)) {

                    int population=indicators[ISO_CODE][year][POP_INDEX];

                    if (population!=0 && indicators[ISO_CODE][year][POP_INDEX+i]!=PSEUDO_SPACE)  indicators[ISO_CODE][year][POP_INDEX+i]/=population; else {
                        if (population == 0) cerr << "Zero pop alert:" << ISO_CODE << " in year: " << year << endl;
                        indicators[ISO_CODE][year][POP_INDEX+i]=0;
                    }

                }
            }
        }
    }
}


void calculateDiffs() {

    int BASE_INDEX=ECON_VARIABLES+POLITY_VARIABLES+POLITY_MISC_VARIABLES;



    double * prevValues = new double[DIFF_VARIABLES]();

    Set<int> variableIndexesToDiff;

    variableIndexesToDiff+=0; variableIndexesToDiff+=1; variableIndexesToDiff+=2; variableIndexesToDiff+=3; variableIndexesToDiff+=4;

    variableIndexesToDiff+=5 ;  // DP1
    variableIndexesToDiff+=9;   // DP2
    variableIndexesToDiff+=10;  // DP3
    variableIndexesToDiff+=57;  // DP4


    for (string ISO_CODE : countries) {
        int prevYear=0;

        if (ISO_CODE=="BTN") {
            cout << "debug " << endl;
        }

        for (int year : coveredYears) {

            if (indicators[ISO_CODE].containsKey(year)) {

                if (prevYear==0) { prevYear=year; continue; }

                double rateDivisor=year-prevYear; // never zero!

                int i=0;
                for (int indexToDiff :  variableIndexesToDiff) {

                    //indicators[ISO_CODE][year][BASE_INDEX+i]= (indicators[ISO_CODE][year][indexToDiff]-prevValues[i])/rateDivisor;

                    prevValues[i]=indicators[ISO_CODE][prevYear][indexToDiff];

                    indicators[ISO_CODE][year][BASE_INDEX+i]= (prevValues[i]);

                    i++;
                }

                prevYear=year;
            }

        }
    }



}
