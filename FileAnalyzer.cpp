#include "FileAnalyzer.h"

FileAnalyzer::FileAnalyzer(const filesystem::path& paramFilePath) {
	SetFilePath(paramFilePath);
}

void FileAnalyzer::SetFilePath(const filesystem::path& paramFilePath) {
	if (paramFilePath.has_extension()) {
		filePath = paramFilePath;
	}
	else {
		throw exception((paramFilePath.string() + " does not have and extension").c_str());
	}
}

filesystem::path FileAnalyzer::GetFilePath() const {
	return filePath;
}

int FileAnalyzer::GetBlankLinesCount() const {
	return blankLinesCount;
}

int FileAnalyzer::GetCommentedLinesCount() const {
	return commentedLinesCount;
}

int FileAnalyzer::GetPhysicalLinesCount() const {
	return physicalLinesCount;
}

int FileAnalyzer::GetCodeLinesCount() const {
	return codeLinesCount;
}

void FileAnalyzer::AnalyzeFile() {
	//open mode "in" open for reading only
	ifstream file(filePath, ios_base::in);
	if (file.is_open()) {
		// typical use case: an input stream represented as a pair of iterators
	   // The default-constructed std::istreambuf_iterator is known as the end-of-stream iterator
		fileData = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		fileData.shrink_to_fit();
		fileSize = fileData.size();
		CountTotalLines();
		blankLinesCount = physicalLinesCount - codeLinesCount - commentedLinesCount;
	
		fileData.clear();
		file.close();
	}
	else {
		throw exception("Unable to open a file");
	}
}


void FileAnalyzer::PrintFileAnalysis(ostream& os) const {
	os << filePath << "\n";
	os << "Count of commented lines:                    " << commentedLinesCount << "\n";
	os << "Count of blank lines:                        " << blankLinesCount << "\n";
	os << "Count of code lines:                         " << codeLinesCount << "\n";
	os << "Count of physical lines:                     " << physicalLinesCount << "\n\n";

}

bool FileAnalyzer::IsBeginningOfComment(size_t index, char firstPart, char secondPart) {
	//iterate over a file characters to find beginning and end of a comment
	return (firstPart == fileData[index]) &&
		(secondPart == fileData[index + 1]) &&
		(index < fileSize);
}

bool FileAnalyzer::IsEndOfMultilineComment(size_t index) {
	return (EnumMultiLineComPart == fileData[index]) &&
		(EnumOneLineComPart == fileData[index + 1]) &&
		(index < fileSize);
}

void FileAnalyzer::CountPhysicalLines() {
	physicalLinesCount = count_if(begin(fileData), end(fileData),
		[](const char& ch) {return EnumBreak == ch; });
	++physicalLinesCount;
}

void FileAnalyzer::CountCodeLines() {
	size_t indexOfChar = 0;
	while (indexOfChar < fileSize) {
		//check if a char is a whitespace characters
		if (!iswspace(fileData[indexOfChar])) {
			//iterate until a new line or end of file
			while ((EnumBreak != fileData[++indexOfChar]) && (indexOfChar < fileSize)) {
			}
			++codeLinesCount;
		} else {
			++indexOfChar;
		}
	}
}

void FileAnalyzer::CountCommentedLines() {
	bool isSameLine = false;
	bool isEndOfThisComment = false;
	size_t indexOfChar = 0;
	while (indexOfChar < fileSize) {
		//do not count example: "//comment /*comment*/"
		if ((EnumSlash == fileData[indexOfChar]) && (EnumQuote == fileData[indexOfChar + 1]) && (indexOfChar < fileSize)) {
			//iterate until quotes close
			while ((EnumQuote != fileData[++indexOfChar]) && (indexOfChar < fileSize)) {
			}
			++indexOfChar;
		//count example: //comment
		}
		else if (IsBeginningOfComment(indexOfChar, EnumOneLineComPart, EnumOneLineComPart)) {
			while ((fileData[++indexOfChar] != EnumBreak) && (indexOfChar < fileSize)) {
			}
			++commentedLinesCount;
			++indexOfChar;
		//count example: /* comment */
		}
		else if (IsBeginningOfComment(indexOfChar, EnumOneLineComPart, EnumMultiLineComPart)) {
			isSameLine = true;
			isEndOfThisComment = false;
			++commentedLinesCount;
			//skip size of comment start: /*
			indexOfChar += SizeOfComSymbol;
			//iterate until new line char
			while (EnumBreak != fileData[indexOfChar]) {
				//check if the comment is multiline
				if (IsEndOfMultilineComment(indexOfChar)) {
					isEndOfThisComment = true;
					indexOfChar += SizeOfComSymbol;
				}
				else {
					++indexOfChar;
				}
			}
			//if the comment is multiline
			if (!isEndOfThisComment) {
				++indexOfChar;
				//until the end of multiline comment
				while (!IsEndOfMultilineComment(indexOfChar)) {
					//if a comment continues at a new line
					if (EnumBreak == fileData[indexOfChar]) {
						isSameLine = false;
						++commentedLinesCount;
					}
					++indexOfChar;
				}
				if (IsEndOfMultilineComment(indexOfChar)) {
					++commentedLinesCount;
				}
				indexOfChar += SizeOfComSymbol;
			}
			//if no comment detected
		}
		else {
			++indexOfChar;
		}
	}
}

void FileAnalyzer::CountTotalLines() {
	CountCommentedLines();
	CountCodeLines();
	CountPhysicalLines();
}
