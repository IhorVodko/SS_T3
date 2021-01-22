#pragma once
#include <algorithm>
#include <cwctype>
#include <cstring>
#include <iostream>
#include <iterator>
#include <exception>
#include <filesystem>
#include <fstream>
using namespace std;
//if a count of blank lines is negative it means that code was not written under code convention (googleStyle)
//in the way that both code and comment are in the same line
//performs better when files are approximately of the same size

//comment start or end: // or /* or */
const int SizeOfComSymbol = 2;

// C++11 strongly typed enumerator feature
enum SymbolsEnum: char {
	EnumOneLineComPart= '/',
	EnumMultiLineComPart = '*',
	EnumQuote = '\"',
	EnumSlash = '\\',
	EnumBreak = '\n',
};

//class walk trough file, analyze file's text, printOut results
class FileAnalyzer {
private:
	int blankLinesCount{};
	int	commentedLinesCount{};
	int	physicalLinesCount{};
	int	codeLinesCount{};
	filesystem::path filePath;
	std::string fileData{};
	size_t fileSize{};

	bool IsBeginningOfComment(size_t, char, char);
	bool IsEndOfMultilineComment(size_t);
	void CountCodeLines();
	void CountCommentedLines();
	void CountPhysicalLines();
	void CountTotalLines();
public:
	filesystem::path GetFilePath() const;
	int GetBlankLinesCount() const;
	int GetCommentedLinesCount() const;
	int GetPhysicalLinesCount() const;
	int GetCodeLinesCount() const;
	void SetFilePath(const filesystem::path&);
	void AnalyzeFile();
	void PrintFileAnalysis(ostream&) const;

	FileAnalyzer() = default;
	FileAnalyzer(const filesystem::path&);
};
