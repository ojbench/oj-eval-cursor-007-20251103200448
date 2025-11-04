/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        } catch (const char* msg) {
            if (std::string(msg) == "QUIT") {
                break;
            }
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string token = scanner.nextToken();
    
    if (scanner.getTokenType(token) == NUMBER) {
        int lineNumber = stringToInteger(token);
        
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(lineNumber);
            return;
        }
        
        program.addSourceLine(lineNumber, line);
        
        std::string cmd = scanner.nextToken();
        Statement *stmt = nullptr;
        
        try {
            if (cmd == "REM") {
                stmt = new RemStatement();
            } else if (cmd == "LET") {
                Expression *exp = parseExp(scanner);
                stmt = new LetStatement(exp);
            } else if (cmd == "PRINT") {
                Expression *exp = parseExp(scanner);
                stmt = new PrintStatement(exp);
            } else if (cmd == "INPUT") {
                std::string var = scanner.nextToken();
                stmt = new InputStatement(var);
            } else if (cmd == "END") {
                stmt = new EndStatement();
            } else if (cmd == "GOTO") {
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new GotoStatement(targetLine);
            } else if (cmd == "IF") {
                Expression *lhs = readE(scanner, 1);
                std::string op = scanner.nextToken();
                Expression *rhs = readE(scanner, 1);
                std::string thenToken = scanner.nextToken();
                if (thenToken != "THEN") {
                    delete lhs;
                    delete rhs;
                    error("SYNTAX ERROR");
                }
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new IfStatement(lhs, op, rhs, targetLine);
            } else {
                error("SYNTAX ERROR");
            }
            
            program.setParsedStatement(lineNumber, stmt);
        } catch (...) {
            delete stmt;
            throw;
        }
        
    } else {
        if (token == "RUN") {
            int currentLine = program.getFirstLineNumber();
            while (currentLine != -1) {
                Statement *stmt = program.getParsedStatement(currentLine);
                if (stmt != nullptr) {
                    try {
                        stmt->execute(state, program);
                        currentLine = program.getNextLineNumber(currentLine);
                    } catch (int targetLine) {
                        currentLine = targetLine;
                    } catch (std::runtime_error &e) {
                        if (std::string(e.what()) == "END") {
                            break;
                        }
                        throw;
                    }
                } else {
                    currentLine = program.getNextLineNumber(currentLine);
                }
            }
        } else if (token == "LIST") {
            int lineNumber = program.getFirstLineNumber();
            while (lineNumber != -1) {
                std::cout << program.getSourceLine(lineNumber) << std::endl;
                lineNumber = program.getNextLineNumber(lineNumber);
            }
        } else if (token == "CLEAR") {
            program.clear();
            state.Clear();
        } else if (token == "QUIT") {
            throw "QUIT";
        } else if (token == "HELP") {
            // Optional, not tested
        } else if (token == "LET") {
            Expression *exp = parseExp(scanner);
            try {
                LetStatement stmt(exp);
                stmt.execute(state, program);
            } catch (...) {
                delete exp;
                throw;
            }
        } else if (token == "PRINT") {
            Expression *exp = parseExp(scanner);
            try {
                PrintStatement stmt(exp);
                stmt.execute(state, program);
            } catch (...) {
                delete exp;
                throw;
            }
        } else if (token == "INPUT") {
            std::string var = scanner.nextToken();
            InputStatement stmt(var);
            stmt.execute(state, program);
        } else {
            error("SYNTAX ERROR");
        }
    }
}
