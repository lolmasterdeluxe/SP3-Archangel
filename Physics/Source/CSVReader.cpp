/**
 CCSVReader
 By: Toh Da Jun
 Added onto By: Ho Junliang
 Date: Mar 2020
 */
#include "CSVReader.h"

#include <fstream>
#include <sstream> // stringstream

#include <iostream>

/**
 @brief Constructor
 */
CCSVReader::CCSVReader()
{
}

/**
 @brief Destructor
 */
CCSVReader::~CCSVReader()
{
}

/**
 @brief Read a CSV file that contains GO spawning information
 @param filename A string containing the name of the input file
 @return A vector<pair<string, float[4]>> variable
 */
vector<pair<string, float[5]>> CCSVReader::read_csv_map(string filename)
{
	// The result of this CSV reading attempt
	bool bResult = true;

	// Create a vector of string and float[4] array to store the result
	vector<pair<string, float[5]>> vListOfValues;

	// Create an input filestream
	ifstream myFile(filename);

	// Make sure the file is open
	try {
		if (!myFile.is_open())
			throw runtime_error("Could not open file");
	}
	catch (runtime_error e)
	{
		//cout << "Runtime error: " << e.what();
		return vListOfValues;
	}

	// Helper vars
	string line;
	int val;

	// Read data, line by line
	while (getline(myFile, line))
	{
		// If this line starts with //, then it is a comment line, so skip this line
		if (line[0] == '/' && line[1] == '/')
			continue;

		// Construct a vector of string and float array
		pair<string, float[5]> gameObject;

		size_t pos = line.find(',');
		string type = line.substr(0, pos);
		line.erase(0, pos + 1);

		gameObject.first = type;

		// Create a stringstream of the remaining values
		stringstream ss(line);
		int i = 0;
		// Extract each integer
		while (ss >> val) {
			// Add the current integer to the array
			gameObject.second[i++] = val;
			//cout << val << endl;
			// If the next token is a comma, ignore it and move on
			if (ss.peek() == ',')
				ss.ignore();
		}
		vListOfValues.push_back(gameObject);
	}

	// Close file
	myFile.close();

	return vListOfValues;
}
