#pragma once
#include <mutex>
#include <thread>
#include <vector>
#include "FileAnalyzer.h"
using namespace std;

const size_t MainThread = 1;

//class implement concurrent analysis of files (*.h, *.hpp, *.c, *.cpp) 
//which pathes are stored in the vector
class DirAnalyzer {
private:
	filesystem::path dirPath;
	vector<FileAnalyzer> vFiles;
	mutex dirAnalyzerMutex;
	int TotalBlankLinesCount{};
	int	TotalCommentedLinesCount{};
	int	TotalCodeLinesCount{};
	int	TotalPhysicalLinesCount{};
	int	TotalProcessedFilesCount{};

	bool IsCorrectExtension(const filesystem::path&);
	void PerformDirAnalysis(size_t, size_t);
public:
	int GetTotalBlankLinesCount() const;
	int GetTotalCommentedLinesCount() const;
	int GetTotalCodeLinesCount() const;
	int GetTotalPhysicalLinesCount() const;
	int GetProcessedFilesCount() const;
	void SetDirPath(const filesystem::path& dirPath);
	void AnalyzeDir();
	void PrintDirAnalysis(ostream&) const;

	DirAnalyzer() = default;
	DirAnalyzer(const filesystem::path&);
};
