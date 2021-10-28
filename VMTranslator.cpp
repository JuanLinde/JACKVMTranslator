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
	string currentCommand;

public:
	Parser(string fileName) { vmCode.open(fileName); }
	~Parser() { vmCode.close(); }
	/*
		Functionality: Returns true if there are more commands in the input. False otherwise.
	*/
	bool hasMoreCommands();
	/*
		Functionality: Reads the next command from the input and makes it the current command. 
		               Should only be called if hasMoreCommands() returns true. At the beginning,
					   there is no current command.
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

int main(int argc, char* argv[])
{

}