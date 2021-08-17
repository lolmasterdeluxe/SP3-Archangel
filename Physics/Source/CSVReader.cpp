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
 @brief Read a CSV file
 @param filename A string containing the name of the input file
 @param NUM_TILES_XAXIS A const int containing the number of tiles in the x-axis
 @param NUM_TILES_YAXIS A const int containing the number of tiles in the y-axis
 @return A vector<vector<int>> variable
 */
vector<vector<int>> CCSVReader::read_csv(string filename,
	const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS)
{
	// The result of this CSV reading attempt
	bool bResult = true;

	// Create a vector of vectors to store the result
	vector<vector<int>> vRowsOfValues;

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
		return vRowsOfValues;
	}

	// Helper vars
	string line, colname;
	int val;

	// Read data, line by line
	while (getline(myFile, line))
	{
		// Create a stringstream of the current line
		stringstream ss(line);

		// If this line starts with //, then it is a comment line, so skip this line
		if (line[0] == '/' && line[1] == '/')
			continue;

		// Construct a vector of int
		vector<int> vColumnsOfValues;

		// Extract each integer
		while (ss >> val) {
			// Add the current integer to the vColumnsOfValues
			vColumnsOfValues.push_back(val);

			// If the next token is a comma, ignore it and move on
			if (ss.peek() == ',')
				ss.ignore();
		}

		if (vColumnsOfValues.size() == NUM_TILES_XAXIS)
		{
			// Push back vColumnsOfValues into result
			vRowsOfValues.push_back(vColumnsOfValues);
		}
		else
		{
			vColumnsOfValues.clear();
			bResult = false;
		}
	}

	// Close file
	myFile.close();

	// Check if the number of rows is correct
	if (vRowsOfValues.size() != NUM_TILES_YAXIS)
	{
		bResult = false;
	}

	// If there was an error during the reading process, then clear all the vectors
	if (bResult == false)
	{
		// Clear each row of vectors inside result
		for (int i = 0; i < (int)vRowsOfValues.size(); i++)
		{
			vRowsOfValues[i].clear();
		}
		// Clear the result vector
		vRowsOfValues.clear();
	}

	return vRowsOfValues;
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
