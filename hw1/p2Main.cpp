#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "p2Table.h"

using namespace std;

int main()
{
    Table table;
    
    // TODO: read in the csv file
    string csvFile;
    cout << "Please enter the file name: ";
    cin >> csvFile;
    if (table.read(csvFile))
        cout << "File \"" << csvFile << "\" was read in successfully." << endl;
    else exit(-1); // csvFile does not exist.
    // TODO read and execute commands
    while (true) {
        string Command;
        getline( cin , Command );
        istringstream Comstream( Command );
        string FirstPart , SecondPart;
        getline( Comstream , FirstPart , ' ' );
        
        if( FirstPart == "PRINT" )
        {
            table.print();
        }
        
        else if( FirstPart == "SUM" )
        {
            getline( Comstream , SecondPart );
            table.sum( SecondPart );
        }
        
        else if( FirstPart == "AVE" )
        {
            getline( Comstream , SecondPart );
            table.ave( SecondPart );
        }
        
        else if( FirstPart == "MAX" )
        {
            getline( Comstream , SecondPart );
            table.max( SecondPart );
        }
        
        else if( FirstPart == "MIN" )
        {
            getline( Comstream , SecondPart );
            table.min( SecondPart );
        }
        
        else if( FirstPart == "COUNT" )
        {
            getline( Comstream , SecondPart );
            table.count( SecondPart );
        }
        
        else if( FirstPart == "ADD" )
        {
            getline( Comstream , SecondPart );
            table.add( SecondPart );
        }
        
        else if( FirstPart == "EXIT" )
        {
            break;
        }
    }
    
}

