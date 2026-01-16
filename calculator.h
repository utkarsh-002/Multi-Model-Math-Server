#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <stack>

class Calculator {
public:
    
    static int calculate(std::string s);

private:
    
    static void operate(std::stack<int>& operands, std::stack<char>& operators);
};

#endif