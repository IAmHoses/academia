#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include "Tokenizer.hpp"
#include "Parser.hpp"

/*****************************************************
 *************INSERT DISCLOSURES HERE *********
 ******************************************************/

// splitting a string by delimiter in c++ http://stackoverflow.com/questions/236129/split-a-string-in-c/236803#236803

using namespace std;

/*****************
 DEFINE CONSTANTS
 ******************/

// You may wish to define some token codes for your tokenizer categories

const int  MAXINPUT = 40000;     /* Maximum length of an input */
const char*   PROMPT = "RuM_interpreter> ";
const char*   PROMPT2 = "--> ";
const char  COMMENTCHAR = '%';
const char  ENDOFINPUT = '$';   // use as sentinel to mark end of input


/****************
 DECLARE GLOBAL STUFF
 ******************/

char userinput[MAXINPUT];//array to store the user input
int  inputLength, pos; // indices for tokenizer

int  enterIfScope = 0; // antiquated ... holds input buffer until matching endif
string currentOutput = "Output: No statements";


/*********************************
 LOW-LEVEL HELPER METHODS FOR READING INPUT
 *********************************/
void nextchar(char& c) // pulls next char, skips over comment lines
{
    scanf("%c", &c);
    if (c == COMMENTCHAR)
    {
        while (c != '\n')
            scanf("%c", &c);
    }
}


// skips over blanks
int skipblanks(string input, int p)
{
    while (input[p] == ' ')
        ++p;
    return p;
}


int isDelim(char c) // checks char for particular delimiters
{
    return ((c == '(') || (c == ')') || (c == ' ') || (c == COMMENTCHAR) || (c == '\n'));
}


int matches(int s, int leng, char* nm) // lookahead checks input for matching string. Helps to check for keywords.
{
    int i = 0;
    
    while (i < leng)
    {
        
        if (userinput[s] != nm[i]) {
            return 0;
        }
        ++i;
        ++s;
    }
    if (!isDelim(userinput[s]))
        return 0;
    
    return 1;
}/* matches */


int matches(int s, int leng, char* nm, bool testing) // lookahead checks input for matching string. Helps to check for keywords.
{
    int i = 0;
    
    while (i < leng)
    {
        if (userinput[s] != nm[i]) {
            return 0;
        }
        ++i;
        ++s;
    }
    
    return 1;
}/* matches */


// reads input from console and stores in global userinput[]
void readInput()
{
    char  c;
    cout << PROMPT;
    pos = 0;
    do
    {
        ++pos;
        if (pos == MAXINPUT)
        {
            cout << "User input too long\n";
            exit(1);
        }
        nextchar(c);
        if (c == '\n')
            userinput[pos] = ' ';// or space
        else
            userinput[pos] = c;
        
    } while (c != '\n');
    inputLength = pos + 1;
    userinput[pos+1] = ENDOFINPUT; // sentinel
    // input buffer is now filled, reset pos to 1 (later in reader()) and begin parse
}


// Reads input until end
void reader()
{
    do
    {
        readInput();
        pos = skipblanks(userinput, 1); // reset pos for tokenizer
    } while (pos > inputLength-1); // ignore blank lines
}

/*********************************
 END OF LOW-LEVEL HELPER METHODS FOR READING INPUT
 *********************************/

void RuM(Scope * globalScope, stack<Scope *> * runtimeStack, FunctionTable * functionTable,
         ClassTable * classTable, ObjectTable * objectTable)
{
    int quittingtime = 0;
    //    lexicalError = false;
    
    while (!quittingtime)
    {
        reader(); // read input and store globally to userinput
        
        if (matches(pos, 4, "quit"))    // quit, rather than use "matches" you can replace this with a condition for the "quit" token
            quittingtime = 1;
        else
        {
            if (userinput[inputLength] == ENDOFINPUT) {
                // Do stuff
                // tokenize input
                // tokenList = tokenize();  // your implementation here will likely vary
                
                Tokenizer tokenizer = *new Tokenizer(inputLength, userinput);
                tokenizer.tokenize("", true);
                
                Parser parser = *new Parser(tokenizer.getTokens(), globalScope, runtimeStack, functionTable, classTable, objectTable);
                parser.parse();
            }
            else
            {
                cout << "Input Error: token found but end of input expected:  " << userinput[inputLength] << endl;
            }
        }
        
    }// while
    //*/
}

/************************
 ****** main **************
 **************************/

// (option 1) no commandline arg: repeatedly calls the reader to facilitate the active interpreter
// (option 2) ADD THIS:  if provided a commandline arg, should read and interpret RuM code in the file, once complete,
// then it should continue with option 1

int main(int argc, const char * argv[]) {
    Scope * globalScope = new Scope();
    stack<Scope *> * runtimeStack = new stack<Scope *>();
    FunctionTable * functionTable = new FunctionTable();
    ClassTable * classTable = new ClassTable();
    ObjectTable * objectTable = new ObjectTable();

//    globalScope->assign("foo", *new Value("3", "Integer"));
//    Value value = globalScope->retreive("foo");
//    
//    cout << value.getValue() << " " << value.getType() << endl;
//    
//    globalScope->assign("foo", *new Value("4", "Integer"));
//    value = globalScope->retreive("foo");
//    
//    cout << value.getValue() << " " << value.getType() << endl;
    
    if (argv[1]) {
        ifstream infile;
        infile.open(argv[1]);
        
        string RuM_code = "";
        string content = "";
        
        while (infile >> content) {
            RuM_code += content + ' ';
        }
        
        RuM_code += '$';
        
        Tokenizer tokenizer = * new Tokenizer(inputLength);
        tokenizer.tokenize(RuM_code, false);
        
        Parser parser = *new Parser(tokenizer.getTokens(), globalScope, runtimeStack, functionTable, classTable, objectTable);
        parser.parse();
    }
    
    RuM(globalScope, runtimeStack, functionTable, classTable, objectTable);
    
    return 0;
}

