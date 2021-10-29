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

public:
	Parser(string fileName);
	~Parser() { vmCode.close(); }

	string getCurrentCommand() { return currentCommand; }
	string getCurrentCommandType() { return currentCommandType; }
	string getCurrentSegment() { return currentSegment; }
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

int main(int argc, char* argv[])
{
	string inputFileName = argv[1];
	Parser parser(inputFileName);
	return 0;
}