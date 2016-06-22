/*
 * Assuming: This application run on 64 bit system
 * Using : use insert sort and external merge sort algorithm
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <queue>
#include <regex>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

#define TEM_FILE "tem_file"

// Path file save input value of user
namespace
{
	std::string   	inputFilePath,
	  		outputFilePath;

}

//desclare function
void mergeSortTwoFile(string & pathFileOne, string & pathFileTwo, string & outputTemporaryFile);
bool fillToTemporaryFile(vector<string> & listString, string & temporaryFileName);
unsigned int firstSortHandle(fstream & inputFile, uint64_t memory_space /* use byte */);
void N_WayMergeSortHandle( unsigned int numTemporaryFile, unsigned int deepRecurse); /* number recursive, begin at 2 , because the first is qsort  had processed*/
bool compareStringsIterator(const string & , const string &);
bool endOfLine(const char);
bool isSpaceCharacter(const char);


//desclare class
class SortHandle
{
// For simple, set all attribute of this class is public
public:
	vector<string>  strings;
	SortHandle(){};
	~SortHandle(){};
	void quickSortStrings();
	int compareStrings(string&, string&);
};

void SortHandle::quickSortStrings()
{
	if (strings.size() <= 1)
		return;
	std::sort(strings.begin(), strings.end(), compareStringsIterator);
}


int SortHandle::compareStrings(string & string1, string & string2)
{
 	string::iterator itStringOne, itStringTwo;
	itStringOne = string1.begin();
	itStringTwo = string2.begin();
	int result = 0;
	// Just compare first word of two string, this is enough
	while (!isSpaceCharacter(*itStringOne) && !isSpaceCharacter(*itStringTwo) && !endOfLine(*itStringOne) && !endOfLine(*itStringTwo))
	{
		if (*itStringOne == *itStringTwo)
		{
		    itStringOne++;
		    itStringTwo++;
		    continue; // defaule value of result is zero, not need set for it
		}
		else if (*itStringOne > *itStringTwo)
		{
			result = 1;
			break;
		}
		else
		{
			result = -1;
			break;
		}
	}

	return result;
}

bool compareStringsIterator(const string & it1, const string & it2)
{
 	string::const_iterator itStringOne, itStringTwo;
	itStringOne = it1.begin();
	itStringTwo = it2.begin();

	bool result = false;
	// Just compare first word of two string, this is enough
	while (!isSpaceCharacter(*itStringOne) && !isSpaceCharacter(*itStringTwo) && !endOfLine(*itStringOne) && !endOfLine(*itStringTwo))
	{
		if (*itStringOne == *itStringTwo)
		{
		    	itStringOne++;
		    	itStringTwo++;
			continue; // defaule value of result is zero, not need set for it
		}
		else if (*itStringOne > *itStringTwo)
		{
			result = true;
			break;
		}
		else
		{
			result = false;
			break;
		}
	}

	return result;
}

bool isSpaceCharacter (const char c)
{
	return (c == ' ' || c == '\t') ? true : false;
}

bool endOfLine(const char c)
{
	return (c == '\n' || c== '\r') ? true : false;
}

bool fillToTemporaryFile(vector<string> & listString, string & temporaryFileName)
{
	fstream temporaryOutputFile;
	temporaryOutputFile.open(temporaryFileName, ios::out | ios::trunc);

	if (temporaryOutputFile.fail())
	{
		cerr << "error occured when open temporary text file" << endl;
		exit(1);
    	}

	if (listString.size() == 0) {
		temporaryOutputFile.close();
		return false;
	}

	vector<string>::iterator it = listString.begin();

	for (; it <= listString.end(); it++)
    	{
        	temporaryOutputFile << *it << endl;
    	}
	temporaryOutputFile.close();
	return true;
}

unsigned int firstSortHandle(fstream & inputFile, uint64_t memory_space /* use byte */)
{
	string line, temporaryFileName;
    	SortHandle sortHandle;
	uint64_t currentMemSpace = memory_space;
	unsigned int temFileNum = 1;

	while (getline(inputFile, line))
    	{
        	temporaryFileName = "";
		if(currentMemSpace < sizeof(char)*line.size()) {
			temporaryFileName = temporaryFileName + TEM_FILE + "_1_" + to_string(temFileNum);
			temFileNum++;

			sortHandle.quickSortStrings();
			fillToTemporaryFile(sortHandle.strings, temporaryFileName);

			sortHandle.strings.clear();
			sortHandle.strings.shrink_to_fit();
			currentMemSpace = memory_space;
		}
        	sortHandle.strings.push_back(line);
		currentMemSpace -= sizeof(char)*line.size();
    	}

	sortHandle.quickSortStrings();

	if(temFileNum == 1)
    	{
       	 	temporaryFileName = outputFilePath;
    	}
    	else 
	{
        	temporaryFileName = temporaryFileName + TEM_FILE + "_1_" +  to_string(temFileNum);
    	}

	if (!fillToTemporaryFile(sortHandle.strings, temporaryFileName))
		temFileNum--;

	return temFileNum;
}

void N_WayMergeSortHandle( unsigned int numTemporaryFile, unsigned int deepRecurse = 1) /* number recursive, begin at 1 , because the first is qsort  had processed*/
{
	if (numTemporaryFile == 0)
	{
		return;
	}

	// create thread handle merge sort
	unsigned int fileInputIndex = 0;
	unsigned int fileOutputIndex = 0;

	while (true)
	{
		if ( (numTemporaryFile - fileInputIndex) == 0)
		{
			break;
		}
		if ( (numTemporaryFile - fileInputIndex) == 1)
		{
			string pathOldFile, pathNewFile;
			pathOldFile = pathOldFile + TEM_FILE + "_" + to_string(deepRecurse) + "_" + to_string(++fileInputIndex);
			pathNewFile = pathNewFile + TEM_FILE + "_" + to_string(deepRecurse+1) + "_" + to_string(++fileOutputIndex);
        		cout << "old name : " << pathOldFile << endl;
            		cout << "new name : " << pathNewFile << endl;
			if (rename(pathOldFile.c_str(), pathNewFile.c_str()))
			{
				cerr << "error occured when rename file name" << endl;
				exit(1);
			}

			break;
		}

		string pathFileOne , pathFileTwo;
 		pathFileOne = pathFileOne + TEM_FILE + "_" + to_string(deepRecurse) + "_" + to_string(++fileInputIndex);
		pathFileTwo = pathFileTwo +  TEM_FILE + "_" + to_string(deepRecurse) + "_" + to_string(++fileInputIndex);
        	cout << pathFileOne << endl;
        	cout << pathFileTwo << endl;

		/* If just have two file, process and write to output file input by user*/
		if (numTemporaryFile == 2)
		{
			mergeSortTwoFile(pathFileOne, pathFileTwo, ::outputFilePath);
			fileOutputIndex = 0;
			break;
		}
		else
		{
			string outputTemporaryFile;
		 	outputTemporaryFile = outputTemporaryFile + TEM_FILE + "_" + to_string(deepRecurse + 1) + "_" + to_string(++fileOutputIndex);
		 	cout << outputTemporaryFile << endl;
			mergeSortTwoFile(pathFileOne, pathFileTwo, outputTemporaryFile);

		}
	}

	N_WayMergeSortHandle(fileOutputIndex, deepRecurse + 1);
}

void mergeSortTwoFile(string & pathFileOne, string & pathFileTwo, string & outputTemporaryFile)
{
	fstream fileOne, fileTwo, outputFile;
	fileOne.open(pathFileOne, ios::in);
	fileTwo.open(pathFileTwo, ios::in);
	outputFile.open(outputTemporaryFile, ios::out | ios::trunc);

	SortHandle sortHandle;

	if (fileOne.fail() || fileTwo.fail() || outputFile.fail()) {
		cerr << "have error occured when process NWayMergeSort" << endl;
		exit(0);
	}

	string lineOne, lineTwo;

	getline(fileOne, lineOne);
	getline(fileTwo, lineTwo);

	while(true)
	{
		int compareResult = sortHandle.compareStrings(lineOne, lineTwo);

		// check end of file of two file input use for compare
		if (!fileOne && !fileTwo)
		{
			break;
		}
		else if(!fileOne)
		{
			outputFile << lineTwo << endl;
			getline(fileTwo, lineTwo);
			continue;
		}
		else if (!fileTwo)
		{
			outputFile << lineOne << endl;
			getline(fileOne, lineOne);
			continue;
		}

		// compare use merge sort alogthirm
		if (compareResult == 0)
		{
			outputFile << lineOne << endl;
			outputFile << lineTwo << endl;

			getline(fileOne, lineOne);
			getline(fileTwo, lineTwo);
		}
		else if (compareResult == 1)
		{
			outputFile << lineOne << endl;
			getline(fileTwo, lineTwo);
		}
		else
		{
			outputFile << lineTwo << endl;
                        getline(fileOne, lineOne);
		}
	}

	fileOne.close();
	fileTwo.close();
	outputFile.close();

	// remove two temporary file input
	remove(pathFileOne.c_str());
	remove(pathFileTwo.c_str());
}

int main (int argc, char * argv[])
{
	std::fstream 	inputFile, outputFile;

	uint64_t	space_memory_buf;

	// Open input file
	cout << "Path input file (use absolute path): ";
	cin >> inputFilePath;
	cout << endl;

	inputFile.open(inputFilePath, ios::in);
	if (inputFile.fail())
	{
		cerr << "The file path"  << inputFilePath << " could not be opened" << endl;
		exit(1);
	}

	// Open input file
	cout << "Path putput file (use absolute path ): ";
	cin >> outputFilePath;
	cout << endl;

	outputFile.open(outputFilePath, ios::out);
	if (outputFile.fail())
	{
		cerr << "The file " << outputFilePath << " could not be opened or created" << endl;
		exit(1);
	}
	outputFile.close();

	// memory space
	cout << "memory space(MB) :";
	cin >> space_memory_buf;
	cout << endl;

	// exit application if memory space input value is very large
	if (UINT64_MAX/1024/1024 <= space_memory_buf)
	{
		cerr << "Memory you require is invalue" << endl;
		exit(0);
	}
	
        cout << space_memory_buf*1024*1024 << endl;
	// Using 100% memory space for fist sort
	unsigned int numTemOutputFile = firstSortHandle (inputFile, space_memory_buf*1024*1024);
	// N-Way merge sort
	if (numTemOutputFile > 1)
	{
		N_WayMergeSortHandle(numTemOutputFile);
	}

	exit(0);
}



