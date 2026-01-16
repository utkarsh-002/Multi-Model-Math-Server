#include "calculator.h"
#include <iostream>
#include <stack>
#include <string>
#include <cctype>

using namespace std;

void Calculator::operate(stack<int>& operands, stack<char>& operators) {
    if(operators.empty()) return;
    char ch = operators.top();
    operators.pop();
    
    if (operands.size() < 2) return;

    int b = operands.top();
    operands.pop();
    int a = operands.top();
    operands.pop();

    switch(ch){
        case '+': operands.push(a + b); break;
        case '-': operands.push(a - b); break;
        case '*': operands.push(a * b); break; 
        case '/': 
            if(b != 0) operands.push(a / b); 
            else operands.push(0);
            break;
    }
}

int Calculator::calculate(string s) {
    stack<int> operands;
    stack<char> operators;
    
    for(int i = 0; i < s.size(); i++){
        if(isspace(s[i])) continue; 
    
        if(isdigit(s[i])){
            long num = 0;
            while(i < s.size() && isdigit(s[i])){
                num = num * 10 + (s[i] - '0');
                i++;
            }
            operands.push(num);
            i--;
        } else if(s[i] == '('){
            operators.push('(');
        } else if(s[i] == ')'){
            while(!operators.empty() && operators.top() != '('){
                operate(operands, operators);
            }
            if(!operators.empty()) operators.pop(); 
        } else {
            while(!operators.empty() && operators.top() != '('){
                operate(operands, operators);
            }
            operators.push(s[i]);
        }
    }
    while(!operators.empty()){
        operate(operands, operators);
    }
    return operands.empty() ? 0 : operands.top();
}