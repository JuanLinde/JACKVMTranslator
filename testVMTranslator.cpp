#include <string>
#include <fstream>
#include <iostream>
using namespace std;

/*
	Functionality: Contains all the methods neccessary for parsing a document in JACK VM code
*/
class Parser
{
private:
	ifstream vmCode;
	string currentCommand;
	string currentCommandType;
	string currentModifier;
	int currentIndex;
	/*
		Functionality: Determines if the current line being traversed contains an instruction. If
					   it does, returns true.
	*/
	bool HasInstruction(string);
	/*
		Functionality: Receives a string, cl, and removes all the spaces at the beginning in place.
	*/
	void removeWhitespaceFrom(string&);
	/*
		Functionality: Receives the current line being traversed, cL, and extracts and returns the
		               command in a string.
	*/
	string extractCommandFrom(string);
	/*
		Functionality: Receives a string that contains the current line being traversed, cL, and 
		               extracts and returns the current command type from it. This function assumes 
					   that whitespaces have been removed from the current line and that it 
					   contains an instruction in JACK VM.

	

		Return types:
					   C_ARITHMETIC - All arithmetic commands
					   C_PUSH
					   C_POP
					   C_LABEL
					   C_GOTO
					   C_IF
					   C_FUNCTION
					   C_RETURN
					   C_CALL
	*/
	string extractCommandTypeFrom(string);
	/*
		Functionality: Receives the command type of the current instruction, cT, and returns true
		               or false depending upon the syntax of the command type.

	*/
	bool currentCommandHasModifier();
	/*
		Functionality: Access the current command and extracts the modifier from it. It assumes the
		               program has checked if the command contains a modifier.
	*/
	string extractModifier(string);
	/*
		Functionality: Receives the current line, cL, and determines if there is an index to be
		               extracted from it. It supposes the current line contains a valid instruct.
	*/
	bool currentInstructionHasIndex();
	/*
		Functionality: Receives the current line, cL, and extracts the index from it. It assumes
		               the current line has a valid instruction and contains an index.
	*/
	int extractIndex(string); 

public:
	Parser(string fileName);
	~Parser() { vmCode.close(); }

	string getCurrentCommand() { return currentCommand; }
	string getCurrentCommandType() { return currentCommandType; }
	string getCurrentModifier() { return currentModifier; }
	int getCurrentIndex() { return currentIndex; }


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

	while (parser.hasMoreLines())
	{
		parser.advance();
		string commandType = parser.getCurrentCommandType();

		bool commandIsReturn = (commandType == "C_RETURN");
		if (!commandIsReturn)
		{
			string command = parser.getCurrentCommand();

			bool commandIsArithmetic = (commandType == "C_ARITHMETIC");
			bool commandIsPushPop = (commandType == "C_PUSH" || commandType == "C_POP");

			if (commandIsArithmetic) writer.writeArithmetic(command);
			//else if (commandIsPushPop)
			//{
			//	string command = parser.getCurrentCommand();
			//	string segment = parser.getCurrentModifier();
			//	int index = parser.getCurrentIndex();

			//	writer.writePushPop(command, segment, index);
			//}
		}

	}
	return 0;
}
/*
	Functionality: Receives the command type of the current instruction, cT, and returns true
				   or false depending upon the syntax of the command type.

*/
bool Parser::currentCommandHasModifier()
{
	bool currCommandHasModifier = (currentCommandType != "C_ARITHMETIC");
	if (currCommandHasModifier) return true;
	else return false;
}

Parser::Parser(string fileName)
{
	vmCode.open(fileName);
	currentCommand = "";
	currentCommandType = "";
	currentModifier = "";
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
	string currentLine;
	getline(vmCode, currentLine);
	removeWhitespaceFrom(currentLine);

	if (HasInstruction(currentLine))
	{
		currentCommand = extractCommandFrom(currentLine);
		currentCommandType = extractCommandTypeFrom(currentCommand);

		if (currentCommandHasModifier())
		{
			currentModifier = extractModifier(currentLine);
		}
		else currentModifier = "";

		if (currentInstructionHasIndex())
		{
			currentIndex = extractIndex(currentLine); 
		}
		else currentIndex = -1;
	}
	else
	{
		currentCommand = "";
		currentCommandType = "";
		currentModifier = "";
		currentIndex = -1;
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

		size_t lastCharOfInstructionPos = (cl.find_last_not_of(" "));
		bool thereAreTrailingWhiteSpaces = (cl.back() == ' ');
		if (thereAreTrailingWhiteSpaces)
		{
			cl.erase(lastCharOfInstructionPos + 1);
		}
	}
}
/*
	Functionality: Receives the current line being traversed, cL, and extracts and returns the
				   command in a string. This functions assumes that the current line has no
				   whitespaces and has an instruction.
*/
string Parser::extractCommandFrom(string cL)
{
	size_t firstSpacePos = cL.find_first_of(" ");
	string command = cL.substr(0, firstSpacePos);
	return command;
}
/*
	Functionality: Determines if the current line being traversed contains an instruction. If
				   it does, returns true. This should be called after whitespaces have been 
				   removed from the current line.

	Algorithm:
	
	1. If current line is empty return false
	2. Else if current line is a comment return false
	3. Else return true
*/
bool Parser::HasInstruction(string cL)
{
	bool currentLineIsEmpty = (cL.empty() == true);
	bool currentLineIsComment = (!currentLineIsEmpty && cL.at(0) == '/');
	if (currentLineIsEmpty) return false;
	else if (currentLineIsComment) return false;
	else return true;
}
/*
	Functionality: Receives a string that contains the current line being traversed, cL, and
				   extracts and returns the current command type from it. This function assumes
				   that whitespaces have been removed from the current line and that it
				   contains an instruction in JACK VM.

	Return types:
					   C_ARITHMETIC - All arithmetic commands
					   C_PUSH
					   C_POP
					   C_LABEL
					   C_GOTO
					   C_IF
					   C_FUNCTION
					   C_RETURN
					   C_CALL
*/
string Parser::extractCommandTypeFrom(string command)
{


	bool commandIsArithmetic = (command == "add" ||
								command == "sub" ||
								command == "gt"  ||
								command == "lt"  ||
								command == "eq"  ||
								command == "neg" ||
								command == "and" ||
								command == "or"  ||
								command == "not");

	bool commandIsPush = (command == "push");
	bool commandIsPop = (command == "pop");

	if (commandIsArithmetic) return "C_ARITHMETIC";
	else if (commandIsPush) return "C_PUSH";
	else if (commandIsPop) return "C_POP";
	else return "not implemented yet";

}
/*
	Functionality: Access the current command and extracts the modifier from it. It assumes the
				   program has checked if the command contains a modifier.
*/
string Parser::extractModifier(string cL)
{
	size_t firstSpacePos = cL.find_first_of(" ");
	size_t secondSpacePos = cL.find(" ", firstSpacePos+1);
	string modifier = cL.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
	return modifier;
}
/*
	Functionality: Receives the current line, cL, and determines if there is an index to be
				   extracted from it. It supposes the current line contains a valid instruct.
*/
bool Parser::currentInstructionHasIndex()
{
	bool currInstHasIndex = (currentCommandType == "C_PUSH" || currentCommandType == "C_POP" ||
							 currentCommand == "C_FUNCTION");

	if (currInstHasIndex) return true;
	else return false;
}
/*
	Functionality: Receives the current line, cL, and extracts the index from it. It assumes
				   the current line has a valid instruction and contains an index.
*/
int Parser::extractIndex(string cL)
{
	size_t firstSpacePos = cL.find(" ");
	size_t secondSpacePos = cL.find(" ", firstSpacePos + 1);
	int index = stoi(cL.substr(secondSpacePos + 1));
	return index;
}
