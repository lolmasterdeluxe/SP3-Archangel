/**
 CCSVReader
 By: Toh Da Jun
 Added onto By: Ho Junliang
 Date: Mar 2020
 */
#ifndef CSV_READER_H
#define CSV_READER_H

#include <string>
#include <vector>


using namespace std;

class CCSVReader
{
public:
	// Constructor
	CCSVReader();
	// Destructor
	virtual ~CCSVReader();

	// Read from a CSV file which contains column names
	vector<pair<string, vector<int>>> read_csv_with_columnname(string filename);

	// Read from a CSV file
	vector<vector<int>> read_csv(string filename, const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS);
	
	// Read from a CSV file that contains GO spawning information
	vector<pair<string, float[5]>> read_csv_map(string filename);
};

#endif