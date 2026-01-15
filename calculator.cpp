#include<bits/stdc++.h>

using namespace std;

class Calculator {
    unordered_map<char,int> mp;
    void operate(stack<int>& operands,stack<char>& operators){
        if(operators.empty()) return;
        char ch = operators.top();
        operators.pop();
        if ((ch == '+' || ch == '-') && operands.size() < 2) {
            return;
        }
        int a,b;
        switch(ch){
            case '+':
                b = operands.top();
                operands.pop();
                a = operands.top();
                operands.pop();
                operands.push(a+b);
                break;
            case '-':
                b = operands.top();
                operands.pop();
                a = operands.top();
                operands.pop();
                // cout<<a<<" "<<b;
                operands.push(a-b);
                break;
            case ')':
                operands.pop();
                while(operands.top()!='(')
                    operate(operands,operators);
                break;
            default:
                cout<<"";
        }
    }

public:
    int calculate(char* s) {
        stack<int> operands;
        stack<char> operators;
        
        for(int i=0; i<sizeof(s); i++){
            if(s[i] == ' ' || s[i]=='\0') continue; 
        
            if(isdigit(s[i])){
                long num = 0;
            
                while(i < sizeof(s) && isdigit(s[i])){
                    num = num * 10 + (s[i] - '0');
                    i++;
                }
                operands.push(num);
                i--;
            }
            else if(s[i] == '('){
                operators.push('(');
            }
            else if(s[i] == ')'){
                while(!operators.empty() && operators.top() != '('){
                    operate(operands, operators);
                }
                if(!operators.empty()) operators.pop(); 
            }
            else {
                while(!operators.empty() && operators.top() != '('){
                    operate(operands, operators);
                }
                operators.push(s[i]);
            }
        }
        while(!operators.empty()){
            operate(operands,operators);
        }
        return operands.top();
    }
};