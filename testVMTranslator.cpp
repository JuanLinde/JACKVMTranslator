#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <dirent.h>
#include <windows.h>
#include <iomanip>
#include <stack>
using namespace std;

/*
	Functionality: Contains all the methods neccessary for parsing a document in JACK VM code
*/
class Parser
{
private:
	ifstream vmCode;
	string currentInstruction;
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

	string getCurrentInstruction() { return currentInstruction; }
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
/*
	This class contains all the methods necessary to translate an instruction from JACK VM code
	to HACK assembly language and output the translation into an output file.
*/
class CodeWriter
{
private:
	string outputFileName;
	string fileWOExtension;
	ofstream assemblyCode;
	int writtenInstructionsSoFar;
	stack<string> functionTracker;
public:
	~CodeWriter();
	/*
		Functionality: Receives a string, c, that contains an arithmetic command in JACK VM
					   language, and outputs the translation to HACK assembly to the output file.
	*/
	void writeArithmetic(string);
	void writePushPop(string, string, int);
	/*
		What it does: Writes the needed preamble for every translated file. It sets up the stack
		              pointer to 256, calls the sys.init() function of the operating system, and 
					  writes the code for the all the comparisons in the program.
	*/
	void writeInit();
	/*
		What it does: It handles the initializing of variables and writing of the bootstrap code
		              The functionality depends on the number files that have been translated when 
					  it is called.

	    Assumptions: 1. This is only called when the driver is dealing with a folder with
			            multiple VM files.
				     2. The file counter is never less than zero

	    Inputs:      1. A string containing the input file name
			         2. An int that indicates how many files have been translated thus far.
	*/
	void initialize(string, int);
	/*
		What it does: Writes HACK assembly code that effects the "label" command.

		Assumptions:  1. Will not be called if there is no label command.
					  2. The label is error free
					  3. The stack tracking the called functions is not empty
					  4. The input refers to a label that has not been used before inside the
					     current function.

		Inputs:       1. A string, l, containing the label to be output to the assembly file.
	*/
	void writeLabel(string);
	/*
		What it does: Writes HACK assembly code that effects the JACK VM "goto" command.

		Assumptions:
		1. The label is always within some function.
		2. The passed label contains no mistakes.
		3. The compiler takes care of returning to the correct instruction after the jump.

		Inputs:
		1. A string, l, containing the label to which to jump.

	*/
	void writeGOTO(string);
	/*
		What it does: Writes HACK assembly code that effects the JACK VM if-goto command.

		Assumptions:

		  1. The label to which to jump is always inside a function.
		  2. Label has not errors.
		  3. The compiler translates where to jump if the jump is taken.

		Inputs:

		  1. A string, l, containing the instruction to which to jump if the condition is met.

	*/
	void writeIf(string);
};
/*
	What it does:
	    Assumptions:
		    1. Command argument is a folder
		Inputs:
		    1. A string containing the name of the folder
		Output:
		    1. A string with the path to the folder to traverse
*/
string getPath(string);
/*
	What it does:

	Assumptions:
	               1. There is a folder to traverse for files with VM code
	Inputs:
	               1. A string containing each item inside the folder being traversed
	Output:
	               1. True if the current file being traversed is a VM file. Otherwise, false.
*/
bool fileIsVMFile(string);

int main(int argc, char* argv[])
{

	// Main Logic for the end

	string input = argv[1];
	bool inputIsDir = (input.find(".") == string::npos);
	int VMfileCounter = 0;


	if (inputIsDir)
	{
		string path = getPath(input);
		const char* pathPointer = path.c_str();
		/*
			Creates a structure an pointer to handle
			traversal of file in folder
		*/
		struct dirent* entry = nullptr;
		DIR* dirPointer = nullptr;
		dirPointer = opendir(pathPointer);
		/*
			Iterates through each file translating it.
		*/
		if (dirPointer != nullptr)
		{
			CodeWriter writer;
			while (entry = readdir(dirPointer))
			{
				string inputFileName = entry->d_name;
				if (fileIsVMFile(inputFileName))
				{
					cout << "Now Translating: " << inputFileName << endl;
					string currPath = path + "\\" + inputFileName;
					Parser parser(currPath);
					writer.initialize(inputFileName, VMfileCounter);
					while (parser.hasMoreLines())
					{
						parser.advance();
						string commandType = parser.getCurrentCommandType();

						bool commandTypeIsNotReturn = (commandType != "C_RETURN");
						bool thereIsCommand = (commandType != "");
						if (thereIsCommand && commandTypeIsNotReturn)
						{
							bool commandTypeIsPushPop = (commandType == "C_PUSH" ||
								commandType == "C_POP");

							if (commandTypeIsPushPop)
							{
								string command = parser.getCurrentCommand();
								string modifier = parser.getCurrentModifier();
								int index = parser.getCurrentIndex();

								writer.writePushPop(command, modifier, index);
							}
							else if (commandType == "C_ARITHMETIC")
							{
								string command = parser.getCurrentCommand();
								writer.writeArithmetic(command);
							}
							else if (commandType == "C_LABEL")
							{
								string modifier = parser.getCurrentModifier();
								writer.writeLabel(modifier);
							}
							else if (commandType == "C_GOTO")
							{
								string label = parser.getCurrentModifier();
								writer.writeGOTO(label);
							}
							else if (commandType == "C_IF")
							{
								string label = parser.getCurrentModifier();
								writer.writeIf(label);
							}
						}
					}
					VMfileCounter++;
				}
			}
		}
	}
	// Input is file
	else
	{
		if (fileIsVMFile(input))
		{
			string inputFileName = input; 
			Parser parser(inputFileName);
			CodeWriter writer;
			writer.initialize(inputFileName, VMfileCounter);

			while (parser.hasMoreLines())
			{
				parser.advance();
				string commandType = parser.getCurrentCommandType();

				bool commandTypeIsNotReturn = (commandType != "C_RETURN");
				bool thereIsCommand = (commandType != "");
				if (thereIsCommand && commandTypeIsNotReturn)
				{
					bool commandTypeIsPushPop = (commandType == "C_PUSH" || 
						                         commandType == "C_POP");

					if (commandTypeIsPushPop)
					{
						string command = parser.getCurrentCommand();
						string modifier = parser.getCurrentModifier();
						int index = parser.getCurrentIndex();

						writer.writePushPop(command, modifier, index);
					}
					else if (commandType == "C_ARITHMETIC")
					{
						string command = parser.getCurrentCommand();
						writer.writeArithmetic(command);
					}
					else if (commandType == "C_LABEL")
					{
						string modifier = parser.getCurrentModifier();
						writer.writeLabel(modifier);
					}
					else if (commandType == "C_GOTO")
					{
						string label = parser.getCurrentModifier();
						writer.writeGOTO(label);
					}
					else if (commandType == "C_IF")
					{
						string label = parser.getCurrentModifier();
						writer.writeIf(label);
					}
				}
			}
		}
	}
	return 0;
}

// Main function methods
/*
	What it does:

		Assumptions:
			1. Command argument is a folder
		Inputs:
			1. A string containing the name of the folder
		Output:
			1. A string with the path to the folder to traverse

	How it does it: 

	1. Creates an array
	2. Gets the current directory and puts it into the array
	3. Iterates through the array creating a string with the path
	4. Adds the folder to search to the path
	5. Returns the path
*/
string getPath(string folderName)
{
	TCHAR buffer[MAX_PATH];
	DWORD bufferLength = GetCurrentDirectory(MAX_PATH, buffer);
	string path = "";
	for (int index = 0; index < bufferLength; index++)
	{
		char currChar = buffer[index];
		path += currChar;
	}
	string folderToSearch(folderName);
	path += "\\" + folderToSearch;
	return path;
}
/*
	What it does:

		Assumptions:
					   1. There is a folder to traverse for files with VM code
		Inputs:
					   1. A string containing each item inside the folder being traversed
		Output:
					   1. True if the current file being traversed is a VM file. Otherwise, false.
	How it does it:

		1. Find the position of the dot
		2. If there is a dot:
		3.   Get the extension of the file
		4.   If it is a vm extension return true
		5.   else return false
		6. else return false
*/
bool fileIsVMFile(string file)
{
	size_t firstDotPos = file.find(".");
	bool fileHasExtension = (firstDotPos != string::npos);

	if (fileHasExtension)
	{
		string extension = file.substr(firstDotPos + 1);
		if (extension == "vm" || extension == "txt") return true;
		else return false;
	}
	else return false;
}

// Parser class methods
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
	currentInstruction = "";
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
		currentInstruction = currentLine;
		currentCommand = extractCommandFrom(currentInstruction);
		currentCommandType = extractCommandTypeFrom(currentCommand);

		if (currentCommandHasModifier())
		{
			currentModifier = extractModifier(currentInstruction);
		}
		else currentModifier = "";

		if (currentInstructionHasIndex())
		{
			currentIndex = extractIndex(currentInstruction);
		}
		else currentIndex = -1;
	}
	else
	{
		currentInstruction = "";
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
		command == "gt" ||
		command == "lt" ||
		command == "eq" ||
		command == "neg" ||
		command == "and" ||
		command == "or" ||
		command == "not");

	bool commandIsPush = (command == "push");
	bool commandIsPop = (command == "pop");
	bool commandIsLabel = (command == "label");
	bool commandIsGOTO = (command == "goto");
	bool commandIsIFGOTO = (command == "if-goto");
	bool commandIsFunction = (command == "function");
	bool commandIsCall = (command == "call");
	bool commandIsReturn = (command == "return");

	if (commandIsArithmetic) return "C_ARITHMETIC";
	else if (commandIsPush) return "C_PUSH";
	else if (commandIsPop) return "C_POP";
	else if (commandIsLabel) return "C_LABEL";
	else if (commandIsGOTO) return "C_GOTO";
	else if (commandIsIFGOTO) return "C_IF";
	else if (commandIsFunction) return "C_FUNCTION";
	else if (commandIsCall) return "C_CALL";
	else if (commandIsReturn) return "C_RETURN";

}
/*
	Functionality: Access the current command and extracts the modifier from it. It assumes the
				   program has checked if the command contains a modifier.
*/
string Parser::extractModifier(string cL)
{
	size_t firstSpacePos = cL.find_first_of(" ");
	size_t secondSpacePos = cL.find(" ", firstSpacePos + 1);
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

// CodeWriter class methods
CodeWriter::~CodeWriter()
{
	assemblyCode.close();
}
/*
	Functionality: Receives a string, c, that contains an arithmetic command in JACK VM
				   language, and outputs the translation to HACK assembly to the output file.

*/
void CodeWriter::writeArithmetic(string c)
{
	bool commandIsComparison = (c == "eq" || c == "lt" || c == "gt");
	if (commandIsComparison)
	{

		// command in upper case
		if (c == "eq") c = "EQ";
		else if (c == "lt") c = "LT";
		else  c = "GT";

		int instructionsInCOMPCommand = 14;
		int addrOfNextInstIfEQTrue = writtenInstructionsSoFar + instructionsInCOMPCommand;

		assemblyCode << "// " << c << endl;
		assemblyCode << "// Stores the address of the next instruction" << endl;
		assemblyCode << "// to which to jump if (TRUE) is entered." << endl;
		assemblyCode << "@" << addrOfNextInstIfEQTrue << endl;
		assemblyCode << "D=A" << endl;
		assemblyCode << "@R13" << endl;
		assemblyCode << "M=D" << endl;
		assemblyCode << "// Pops both values off the stack and compares them" << endl;
		assemblyCode << "// jumps to (TRUE) if comparison is true" << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "AM=M-1" << endl;
		assemblyCode << "D=M" << endl;
		assemblyCode << "A=A-1" << endl;
		assemblyCode << "D=M-D" << endl;
		assemblyCode << "@TRUE" << endl;
		assemblyCode << "D;J" << c << endl;
		assemblyCode << "// Sets the stack to False since comparison was false." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "A=M-1" << endl;
		assemblyCode << "M=0" << endl;

		writtenInstructionsSoFar += instructionsInCOMPCommand;
	}

	else if (c == "add")
	{
		assemblyCode << "// ADD" << endl;
		assemblyCode << "// Accesses the two top elements of the stack" << endl;
		assemblyCode << "// Adds them, stores the result in stack." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "AM=M-1" << endl;
		assemblyCode << "D=M" << endl;
		assemblyCode << "A=A-1" << endl;
		assemblyCode << "M=M+D" << endl;

		writtenInstructionsSoFar += 5;
	}
	else if (c == "sub")
	{
		assemblyCode << "// SUB" << endl;
		assemblyCode << "// Accesses the two top elements of the stack" << endl;
		assemblyCode << "// subs them, stores the result in stack." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "AM=M-1" << endl;
		assemblyCode << "D=M" << endl;
		assemblyCode << "A=A-1" << endl;
		assemblyCode << "M=M-D" << endl;

		writtenInstructionsSoFar += 5;
	}
	else if (c == "neg")
	{
		assemblyCode << "// NEG" << endl;
		assemblyCode << "// Access top element stack and negates it arithmetically." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "A=M-1" << endl;
		assemblyCode << "M=-M" << endl;

		writtenInstructionsSoFar += 3;
	}
	else if (c == "and")
	{
		assemblyCode << "// AND" << endl;
		assemblyCode << "// Accesses the two top elements of the stack" << endl;
		assemblyCode << "// Ands them, stores the result in stack." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "AM=M-1" << endl;
		assemblyCode << "D=M" << endl;
		assemblyCode << "A=A-1" << endl;
		assemblyCode << "M=D&M" << endl;

		writtenInstructionsSoFar += 5;
	}
	else if (c == "or")
	{
		assemblyCode << "// OR" << endl;
		assemblyCode << "// Accesses the two top elements of the stack" << endl;
		assemblyCode << "// OR them, stores the result in stack." << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "AM=M-1" << endl;
		assemblyCode << "D=M" << endl;
		assemblyCode << "A=A-1" << endl;
		assemblyCode << "M=D|M" << endl;


		writtenInstructionsSoFar += 5;
	}
	else if (c == "not")
	{
		assemblyCode << "// NOT" << endl;
		assemblyCode << "// Accesses top element in stack and NOTs it" << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "A=M-1" << endl;
		assemblyCode << "M=!M" << endl;

		writtenInstructionsSoFar += 3;
	}
}
/*
	Functionality: Receives a command, c, a modifier of that command, m, and the index and
				   pushes or pops the index to/from the stack.
*/
void CodeWriter::writePushPop(string c, string m, int i)
{
	if (m == "constant")
	{
		assemblyCode << "// PUSH CONSTANT " << i << endl;
		assemblyCode << "// Stores value to be pushed." << endl;
		assemblyCode << "@" << i << endl;
		assemblyCode << "D=A" << endl;
		assemblyCode << "// Pushes value into the stack and updates pointers" << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "A=M" << endl;
		assemblyCode << "M=D" << endl;
		assemblyCode << "@SP" << endl;
		assemblyCode << "M=M+1" << endl;

		writtenInstructionsSoFar += 7;
	}
	else if (m == "local" || m == "argument" || m == "this" || m == "that")
	{
		string predefLabel = "";
		if (m == "local") { predefLabel = "LCL"; m = "LOCAL"; }
		else if (m == "argument") { predefLabel = "ARG", m = "ARGUMENT"; }
		else if (m == "this") { predefLabel = "THIS"; m = "THIS"; }
		else if (m == "that") { predefLabel = "THAT"; m = "THAT"; }

		if (c == "pop")
		{

			assemblyCode << "// POP  " << m << " " << i << endl;
			assemblyCode << "// Access last element in stack, stores it," << endl;
			assemblyCode << "// and update pointer." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "AM=M-1" << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "@R13" << endl;
			assemblyCode << "M=D" << endl;
			assemblyCode << "// Stores value of index, adds it to the Local base" << endl;
			assemblyCode << "// and stores it in R14 temporarily." << endl;
			assemblyCode << "@" << i << endl;
			assemblyCode << "D=A" << endl;
			assemblyCode << "@" << predefLabel << endl;
			assemblyCode << "D=M+D" << endl;
			assemblyCode << "@R14" << endl;
			assemblyCode << "M=D" << endl;
			assemblyCode << "// Accesses stores Stack element, Local segment" << endl;
			assemblyCode << "// register and stores element in it." << endl;
			assemblyCode << "@R13" << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "@R14" << endl;
			assemblyCode << "A=M" << endl;
			assemblyCode << "M=D" << endl;

			writtenInstructionsSoFar += 16;
		}
		else
		{
			assemblyCode << "// PUSH " << m << " " << i << endl;
			assemblyCode << "// Access address local + index and store it." << endl;
			assemblyCode << "@" << i << endl;
			assemblyCode << "D=A" << endl;
			assemblyCode << "@" << predefLabel << endl;
			assemblyCode << "A=M+D" << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "// Access next available stack reg and insert stored val." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "A=M" << endl;
			assemblyCode << "M=D" << endl;
			assemblyCode << "// Update the stack pointer." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "M=M+1" << endl;

			writtenInstructionsSoFar += 10;
		}
	}
	else if (m == "temp" || m == "pointer")
	{
		int realIndex = 0;    // Takes into account that segments start at R3 or R5
		if (m == "pointer") { m = "POINTER"; realIndex = 3 + i; }
		else if (m == "temp") { m = "TEMP"; realIndex = 5 + i; }

		if (c == "pop")
		{
			assemblyCode << "// POP " << m << " " << i << endl;
			assemblyCode << "// Pop element from stack." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "AM=M-1" << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "// Store it in temp register." << endl;
			assemblyCode << "@R" << realIndex << endl;
			assemblyCode << "M=D" << endl;

			writtenInstructionsSoFar += 5;

		}
		else
		{
			assemblyCode << "// POP " << m << " " << i << endl;
			assemblyCode << "// Store element in register." << endl;
			assemblyCode << "@" << realIndex << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "// Push it into the stack and update pointer." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "A=M" << endl;
			assemblyCode << "M=D" << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "M=M+1" << endl;

			writtenInstructionsSoFar += 7;
		}
	}
	else
	{
		if (c == "pop")
		{
			assemblyCode << "// POP STATIC " << i << endl;
			assemblyCode << "// Access last element in stack, stores it," << endl;
			assemblyCode << "// and update pointer." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "AM=M-1" << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "// Store element in correct static place." << endl;
			assemblyCode << "@" << fileWOExtension << "." << i << endl;
			assemblyCode << "M=D" << endl;

			writtenInstructionsSoFar += 5;
		}
		else
		{
			assemblyCode << "// PUSH STATIC " << i << endl;
			assemblyCode << "// Access static element to push and store it." << endl;
			assemblyCode << "@" << fileWOExtension << "." << i << endl;
			assemblyCode << "D=M" << endl;
			assemblyCode << "// Store in stack and update pointer." << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "A=M" << endl;
			assemblyCode << "M=D" << endl;
			assemblyCode << "@SP" << endl;
			assemblyCode << "M=M+1" << endl;

			writtenInstructionsSoFar += 7;
		}
	}
}
/*
	What it does: Writes the needed preamble for every translated file. It sets up the stack
				  pointer to 256, calls the sys.init() function of the operating system, and
				  writes the code for the all the comparisons in the program.

	How it does it: 	1. Set up the stack pointer to 256
						2. Sets up the Go to (sys.init)
						3. Sets up the jump to avoid running the (TRUE) label on the first pass
						4. Writes the (TRUE) label, which is used by all comparison instructions
*/
void CodeWriter::writeInit()
{
	assemblyCode << "// Sets up the stacK" << endl;
	assemblyCode << "@256" << endl;
	assemblyCode << "D=A" << endl;
	assemblyCode << "@SP" << endl;
	assemblyCode << "M=D" << endl;
	assemblyCode << "// Calls Sys.init() " << endl;
	assemblyCode << "@sys.init" << endl;
	assemblyCode << "0;JMP" << endl;
	assemblyCode << "// Makes sure the following is not executed on first pass." << endl;
	assemblyCode << "@14" << endl;
	assemblyCode << "0;JMP" << endl;
	assemblyCode << "// Provides all the definitions for comparison instructions." << endl;
	assemblyCode << "(TRUE)" << endl;
	assemblyCode << "// Makes sure the value is placed in righ register" << endl;
	assemblyCode << "@SP" << endl;
	assemblyCode << "A=M-1" << endl;
	assemblyCode << "M=-1" << endl;
	assemblyCode << "// Holds the value of the instruction to which to jump" << endl;
	assemblyCode << "// After label is finished running." << endl;
	assemblyCode << "@R13" << endl;
	assemblyCode << "A=M" << endl;
	assemblyCode << "0;JMP" << endl;

	writtenInstructionsSoFar += 14;
}
/*
	What it does: It handles the initializing of variables and writing of the bootstrap code
				  The functionality depends on the number files that have been translated when
				  it is called.

	Assumptions: 1. This is only called when the driver is dealing with a folder with
					multiple VM files.
				 2. The file counter is never less than zero

	Inputs:      1. A string containing the input file name
				 2. An int that indicates how many files have been translated thus far.

	How it does it:  1. If file is first to be translated:
	                 2.   Set the file without extension for static variable translation
					 3.   Set the output file name
					 4.   Open a connection to the output file
					 5.   Initialize the written instruction counters to 0
					 6.   Call the writeInit() function to write the preamble of the code
					 7. If file is not the first file to be translated:
					 8.   Set the file without extension for static variable translation
					 9.   Set the output file name for completeness
*/
void CodeWriter::initialize(string inputFileName, int filesTranslatedSoFar)
{
	if (filesTranslatedSoFar == 0)
	{
		fileWOExtension = inputFileName.substr(0, inputFileName.find("."));
		outputFileName = fileWOExtension + ".asm";
		assemblyCode.open(outputFileName);
		writtenInstructionsSoFar = 0;
		functionTracker.push("main");
		writeInit();
	}
	else
	{
		fileWOExtension = inputFileName.substr(0, inputFileName.find("."));
		outputFileName = fileWOExtension + ".asm";
	}
}
/*
	What it does: Writes HACK assembly code that effects the "label" command.

	Assumptions:  1. Will not be called if there is no label command.
				  2. The label is error free
				  3. The stack tracking the called functions is not empty
				  4. The input refers to a label that has not been used before inside the
				     current function.

	Inputs:       1. A string, l, containing the label to be output to the assembly file.

	How it works: 1. Get the name of the label´s scope from the function tracker stack.
	              2. Construct the label to be output.
				  3. Output the label to the assembly file.
				  4. Update the written instruction count.
*/
void CodeWriter::writeLabel(string l)
{
	string currFunction = functionTracker.top();
	string label = "(" + currFunction + "$" + l + ")";
	assemblyCode << "// LABEL " << l << " IN " << currFunction << endl;
	assemblyCode << label << endl;
}
/*
	What it does: Writes HACK assembly code that effects the JACK VM "goto" command.
	
	Assumptions: 
	1. The label is always within some function.
	2. The passed label contains no mistakes.
	3. The compiler takes care of returning to the correct instruction after the jump.
					
	Inputs: 
	1. A string, l, containing the label to which to jump.

	How it works: 
	1. Gets the name of the function to which the label belongs.
	2. Construct the label following language specifications.
	3. Writes the assembly instructions.
	4. Updates the written instructions counter.
*/
void CodeWriter::writeGOTO(string l)
{
	string currFunct = functionTracker.top();
	string label = currFunct + "$" + l;
	assemblyCode << "// GO TO (" << label << ")" << endl;
	assemblyCode << "@" << label << endl;
	assemblyCode << "0;JMP" << endl;
	writtenInstructionsSoFar += 2;
}
/*
	What it does: Writes HACK assembly code that effects the JACK VM if-goto command.

	Assumptions:

	  1. The label to which to jump is always inside a function.
	  2. Label has not errors.
	  3. The compiler translates where to jump if the jump is taken.

	Inputs:
	
	  1. A string, l, containing the instruction to which to jump if the condition is met.

	How it works:

	  1. Get the name of the label's function
	  2. Write the instructions to pop the stack and save the value
	  3. Construct the label according to the language specs
	  4. Write the assembly instructions to make the jump comparing the top of the stack to zero.
	  5. Updates the written instruction count
*/
void CodeWriter::writeIf(string l)
{
	string currFunct = functionTracker.top();
	assemblyCode << "// IF-GOTO " << l << " IN " << currFunct <<  endl;
	assemblyCode << "// Pops stack and saves value." << endl;
	assemblyCode << "@SP" << endl;
	assemblyCode << "AM=M-1" << endl;
	assemblyCode << "D=M" << endl;
	assemblyCode << "// Compares result to zero and jumps if not equal." << endl;
	assemblyCode << "// Continues execution if comparison is equal 0." << endl;

	string label = currFunct + "$" + l;
	assemblyCode << "@" << label << endl;
	assemblyCode << "D;JNE" << endl;

	writtenInstructionsSoFar += 5;
}

