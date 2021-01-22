#include "DirAnalyzer.h"

DirAnalyzer::DirAnalyzer(const filesystem::path& paramDirPath) {
	SetDirPath(paramDirPath);
}

void DirAnalyzer::SetDirPath(const filesystem::path& paramDirPath) {
	if (filesystem::is_directory(paramDirPath)) {
		dirPath = paramDirPath;
	}
	else {
		throw exception((paramDirPath.string() + " is not a directory.").c_str());
	}
}

int DirAnalyzer::GetTotalBlankLinesCount() const {
	return TotalBlankLinesCount;
}

int DirAnalyzer::GetTotalCommentedLinesCount() const {
	return TotalCommentedLinesCount;
}

int DirAnalyzer::GetTotalCodeLinesCount() const {
	return TotalCodeLinesCount;
}

int DirAnalyzer::GetTotalPhysicalLinesCount() const {
	return TotalPhysicalLinesCount;
}

int DirAnalyzer::GetProcessedFilesCount() const {
	return TotalProcessedFilesCount;
}

void DirAnalyzer::AnalyzeDir() {
	for (const auto& entry : filesystem::recursive_directory_iterator(dirPath)) {
		if (IsCorrectExtension(static_cast<filesystem::path>(entry))) {
			//keep file pathes in a vector
			vFiles.push_back(FileAnalyzer(entry.path()));
			++TotalProcessedFilesCount;
		}
	}
	//maximum number of threads on the machine, my 12
	const size_t maxThreads = thread::hardware_concurrency();
	//+1 for integer division
	const size_t filesPerThread = (TotalProcessedFilesCount / maxThreads) + 1;
	size_t Begin = 0;
	// -1 to leave one thread for main()
	vector<std::thread> pool(maxThreads - MainThread);
	for (auto& thread : pool) {
		//begin and (step+begin) args to fun PerformDirAnalysis, 1st and 2nd args are callable obj
		//it is defined in terms of invoke definition
		thread = std::thread(&DirAnalyzer::PerformDirAnalysis, this, Begin, (Begin + filesPerThread));
		Begin += filesPerThread;
	}
	//implement mutex 
	PerformDirAnalysis(Begin, TotalProcessedFilesCount);
	//bind thread calls to main
	for (auto& thread : pool) {
		thread.join();
	}
}

bool DirAnalyzer::IsCorrectExtension(const filesystem::path& file) {
	std::string extension = file.extension().string();
	return (".cpp" == extension) || (".c" == extension) ||
		(".h" == extension) || (".hpp" == extension);
}

void DirAnalyzer::PerformDirAnalysis(size_t begin, size_t end)
{   //take files to be analyzed one at a time from vector
	for (size_t fileIndex = begin; fileIndex < end; ++fileIndex) {
		vFiles[fileIndex].AnalyzeFile();
		//mechanism for owning a mutex for the duration of a scoped block
		std::lock_guard<mutex> quard(dirAnalyzerMutex);
		TotalBlankLinesCount += vFiles[fileIndex].GetBlankLinesCount();
		TotalCommentedLinesCount += vFiles[fileIndex].GetCommentedLinesCount();
		TotalCodeLinesCount += vFiles[fileIndex].GetCodeLinesCount();
		TotalPhysicalLinesCount += vFiles[fileIndex].GetPhysicalLinesCount();
	}
}

void DirAnalyzer::PrintDirAnalysis(ostream& os)const {
	os << "Total count of commented lines:              " << TotalCommentedLinesCount << "\n";
	os << "Total count of blank lines:                  " << TotalBlankLinesCount << "\n";
	os << "Total count of code lines:                   " << TotalCodeLinesCount << "\n";
	os << "Total count of physical lines:               " << TotalPhysicalLinesCount << "\n";
	os << "Count of processed files:                    " << TotalProcessedFilesCount << "\n\n\n";
	//print analysis of each file
	for (auto& file : vFiles) {
		file.PrintFileAnalysis(os);
	}
}
