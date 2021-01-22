#include <ctime>
#include <fstream>
#include "DirAnalyzer.h"
#include "FileAnalyzer.h"
using namespace std;
//if a count of blank lines is negative it means that code is not written under code convention (googleStyle)
//in the way that both code and comment are in the same line


int main()
{
	cout << "\n------------------->START<-------------------\n\n";
	ofstream analysisResults(R"(pathToSave)");
	filesystem::path directoryToAnalyze(R"(pathToAnalyze)");
	if (analysisResults.is_open()) {
		try {
			clock_t start = clock();
			DirAnalyzer analyzer(directoryToAnalyze);
			analyzer.AnalyzeDir();
			analyzer.PrintDirAnalysis(analysisResults);
			clock_t finish = clock();
			clock_t result_time = finish - start;
			analysisResults << "Velocity----------> " << result_time << " ms\n";
			std::cout << "Velocity----------> " << result_time << " ms.\n";
			analysisResults.close();
		} catch (exception& ex) {
			std::cerr << ex.what() << "\n";
		}
	} else {
		cerr << "Unable to open file to save results of analysis.\n";
	}
	cout << "\n------------------->END<-------------------\n";
	return 0;
}
