/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <iostream>

/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

void InputStatement::execute(EvalState &state, Program &program) {
    while (true) {
        std::cout << " ? ";
        std::string input;
        std::getline(std::cin, input);
        
        try {
            bool valid = true;
            if (input.empty()) {
                valid = false;
            } else {
                size_t start = 0;
                if (input[0] == '-' || input[0] == '+') {
                    start = 1;
                }
                if (start >= input.length()) {
                    valid = false;
                } else {
                    for (size_t i = start; i < input.length(); i++) {
                        if (!isdigit(input[i])) {
                            valid = false;
                            break;
                        }
                    }
                }
            }
            
            if (!valid) {
                std::cout << "INVALID NUMBER" << std::endl;
                continue;
            }
            
            int value = stringToInteger(input);
            state.setValue(var, value);
            break;
        } catch (...) {
            std::cout << "INVALID NUMBER" << std::endl;
        }
    }
}
