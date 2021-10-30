#include <string>
#include <fstream>
using namespace std;

/*
	Functionality: Contains all the methods neccessary for parsing a document in JACK VM code
*/
class Parser
{
private:
	ifstream vmCode;
	string currentLine;
	string currentInstruction;
	string currentCommand;
	string currentCommandType;
	string currentSegment;
	int currentIndex;
	/*
		Functionality: Determines if the current line being traversed contains an instruction. If
					   it does, returns true.
	*/
	bool currentLineHasInstruction();
	/*
		Functionality: Receives a string, cl, and removes all the spaces at the beginning in place.
	*/
	void removeWhitespaceFrom(string&);

public:
	Parser(string fileName);
	~Parser() { vmCode.close(); }

	string getCurrentCommand() { return currentCommand; }
	string getCurrentCommandType() { return currentCommandType; }
	string getCurrentSegment() { return currentSegment; }
	int getCurrentIndex() { return currentIndex; }

	string getCurrentLine() { return currentLine; }

	/*
		Functionality: Returns true if there are more commands in the input. False otherwise.
	*/
	bool hasMoreLines();
	/*
		Functionality: Should only be called if hasMoreLines() returns true. It reads the
					   current line and stores it, extracts the current command, command type,
					   current segment (if applicable), and current index (if applicable), and
					   stores all the information.
	*/
	void advance();
};

int main(int argc, char* argv[])
{
	string inputFileName = argv[1];
	Parser parser(inputFileName);
	ofstream outputCode("outputTest.txt");
	string line;

	while (parser.hasMoreLines())
	{
		parser.advance();
		line = parser.getCurrentLine();
		outputCode << line << endl;
	}
	return 0;
}

Parser::Parser(string fileName)
{
	vmCode.open(fileName);
	currentLine = "";
	currentInstruction = "";
	currentCommand = "";
	currentCommandType = "";
	currentSegment = "";
	currentIndex = -1;
}

bool Parser::hasMoreLines()
{
	bool hasMoreLines = (vmCode.eof() == false);
	if (hasMoreLines) return true;
	return false;
}

void Parser::advance()
{
	getline(vmCode, currentLine);
	removeWhitespaceFrom(currentLine);

	if (currentLineHasInstruction())
	{
		************************************************
	}

}
/*
	Functionality: Receives a string, cl, and removes all the spaces at the beginning in place.

	General Algorithm:

	1. Check if string is empty
	2. If it is not empty:
	3.   Look for the position of the first space character
	4.   Look for the position of the first non-space character
	5.   Check if there are leading spaces
	6.   If there are:
	7.      erase leading spaces
	8.   Look for the position of the beginning of an in-line comment
	9.   Check if there is an in-line comment and if it isn't a line containing just a comment
	10.  If there is an inline comment:
	11.    remove it from the string

	Complexity: O(n) where n is the lenght of the passed string.
*/
void Parser::removeWhitespaceFrom(string& cl)
{
	bool stringIsEmpty = (cl.empty() == true);
	if (!stringIsEmpty)
	{
		size_t firstSpacePos = cl.find_first_of(" ");
		size_t firstCharPos = cl.find_first_not_of(" ");


		bool thereAreLeadingSpaces = (firstSpacePos == 0);
		if (thereAreLeadingSpaces)
		{
			cl.erase(firstSpacePos, firstCharPos);
		}

		size_t firstCommPos = cl.find_first_of("/");
		bool thereAreInlineComments = (firstCommPos != 0 && firstCommPos != string::npos);
		if (thereAreInlineComments)
		{
			cl.erase(firstCommPos);
		}
	}
}
