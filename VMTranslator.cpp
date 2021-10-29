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
	string currentLine;
	string currentInstruction;
	string currentCommand;
	string currentCommandType;
	string currentModifier;
	int currentIndex;
	/*
		Functionality: Determines if the current line being traversed contains an instruction. If
		               it does, returns true.
	*/
	bool currentLineHasInstruction();

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
	bool hasMoreCommands();
	/*
		Functionality: Should only be called if hasMoreCommands() returns true. It reads the 
		               current line and stores it, extracts the current command, command type,
					   current segment (if applicable), and current index (if applicable), and 
					   stores all the information.
	*/
	void advance();
	/*
		Functionality: Returns the type of the current command.

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
	string commandType();
	/*
		Functionality: Returns the first argument of the current command. In the case the command
		               is C_ARITHMETIC, the command itself is returned. If current command is of 
					   type C_RETURN, this method is not called.
	*/
	string arg1();
	/*
		Functionality: Returns the second argument of the current command. Should only be called 
		               if the current command is C_PUSH, C_POP, C_FUNCTION, C_CALL
	*/
	int arg2();
};
/*
	This class contains all the methods necessary to translate an instruction from JACK VM code
	to HACK assembly language and output the translation into an output file.
*/
class CodeWriter
{
private:
	ofstream assemblyCode;
public:
	CodeWriter(string fileName) { assemblyCode.open(fileName); }
	~CodeWriter() { assemblyCode.close();}

	/*
		Functionality: Receives a string, c, that contains an arithmetic command in JACK VM 
		               language, and outputs the translation to HACK assembly to the output file.
	*/
	void writeArithmetic(string c);
	/*
		Functionality: Receives a string, c, that contains a command in JACK VM language; a string,
		s, that contains the virtual memory segment; and a an int, ind, that refers to the index
		of the virtual memory segment to be accessed. And outputs the corresponding HACK assembly
		code to the output file
	*/
	void writePushPop(string c, string s, int ind);
};
/*
	Functionality: Goes through the input file containing JACK VM code and translates it to HACK
	               assembly language. Puts the resulting code in an output file.

*/
int main(int argc, char* argv[])
{
	string inputFileName = argv[1];
	string outputFileName = "output.txt";
	Parser parser(inputFileName);
	CodeWriter writer(outputFileName);

	/*
		1. While there are lines in the input file:
		2.   Extract the command from the line if applicable
		3.   Extract the command type from the command if applicable
		4.   Extract the first argument from the command if applicable
		5.   Extract the second argument from the command if applicable
		6.   Writes the code corresponding to the command 
	*/
	while (parser.hasMoreCommands())
	{
		parser.advance();
		string commandType = parser.getCurrentCommandType();

		bool commandIsReturn = (commandType == "C_RETURN");
		if (!commandIsReturn)
		{
			string command = parser.getCurrentCommand();
			string modifier = "";

			bool thereIsModifier = (commandType == "C_PUSH"     || 
				                    commandType == "C_POP"      ||
				                    commandType == "C_FUNCTION" || 
				                    commandType == "C_CALL");
			if (thereIsModifier)
			{
				modifier = parser.getCurrentModifier();
			}

			bool commandIsArithmetic = (commandType == "C_ARITHMETIC");
			bool commandIsPushPop = (commandType == "C_PUSH" || commandType == "C_POP");

			if (commandIsArithmetic)
			{
				writer.writeArithmetic(command);
			}
			else if (commandIsPushPop)
			{
				string command = parser.getCurrentCommand();
				string segment = parser.getCurrentModifier();
				int index = parser.getCurrentIndex();

				writer.writePushPop(command, segment, index); 
				
			}
		}
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

bool Parser::hasMoreCommands()
{
	bool hasMoreCommands = (vmCode.eof() == false);
	if (hasMoreCommands) return true;
	return false;
}

void Parser::advance()
{
	getline(vmCode, currentLine);

	if (currentLineHasInstruction())
	{
		currentInstruction = extractInstructionFrom(currentLine);
		currentCommand = extractCommandFrom(currentInstruction);
		currentCommandType = extractCommandTypeFrom(currentCommand);
		
		if (currentInstructionHasSegment())
		{
			currentSegment = extractSegmentFrom(currentInstruction);

		}
		if (currentInstructionHasIndex())
		{
			currentIndex = extractIndexFrom(currentInstruction);
		}
	}
}

bool Parser::currentLineHasInstruction()
{
	bool currentLineIsEmpty = (currentLine.empty() == true);
	if (currentLineIsEmpty) return false;
	else if
}
