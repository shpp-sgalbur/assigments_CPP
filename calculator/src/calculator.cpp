#include <iostream>
#include "console.h"
#include "tokenscanner.h"
#include "stack.h"
#include <math.h>
#include "simpio.h"
#include <cctype>


using namespace std;



/**
 * This function performs a simple operation
 * - remove the 2 operand from the stack operandStack
 * - extract the operator from the stack operatorStack
 * - execute operations respectively to the operator from the stack operatorStack
 * - the operation result stored in the  stack operandStack
 */
void simpeOperation(Stack <double> & operandStack,Stack <char> & operatorStack){
    double result;
    double rightOperand = operandStack.pop();
    double leftOperand = operandStack.pop();
    char operatorFromStack = operatorStack.pop();
    switch(operatorFromStack){
        case '+' : result = leftOperand + rightOperand; break;
        case '-' : result = leftOperand - rightOperand; break;
        case '*' : result = leftOperand * rightOperand; break;
        case '/' :
        case ':' : result = leftOperand / rightOperand; break;
        //case '%' : result = leftOperand % rightOperand; break;
        case '^' : result = pow( leftOperand, rightOperand); break;
    }
    cout << leftOperand << operatorFromStack <<rightOperand << "="<< result <<endl;
    operandStack.push( result);
}

/**
 * This function defines the execution priority of the operator transferred to it .
 * Here than the higher the priority of the operator than the greater the return value.
 * If the character passed as parameter is not a mathematical operator function will return -1
 * -----------------------------------------------------
 * @brief priorityOperator
 * @param ch
 * @return
 */
int priorityOperator(char ch){
    string operators = "-+*/:^";
    if( operators.find(ch) ==  string::npos ) return -1;
    if( ch == '-' || ch == '+') return 1;
    if( ch == '*' || ch == '/' || ch == ':') return 2;
    if( ch == '^') return 3;
    return 4;
}

/**
 * This function performs a simple sequence of operators which priority is not below the current statement
 * -------------------------
 * @brief executeSmallerOper
 * @param operatorStack
 * @param operandStack
 * @param priority
 */
void executeSmallerOper(Stack <char> & operatorStack, Stack <double> & operandStack, int priority){

    if (operatorStack.isEmpty()) return;

    if (priorityOperator(operatorStack.peek()) < priority) return;
    simpeOperation(operandStack,operatorStack);
    executeSmallerOper(operatorStack, operandStack, priority);


}

void toLower(string & str) {
   for (int i = 0; i < str.length(); i++) {
      str[i] = tolower(str[i]);
   }
}



void getFunction(string f, double & operand){

    toLower(f);

        if (f == "sin") operand = sin(operand);else
        if (f == "cos") operand = cos(operand);else
        if (f == "tg")  operand = tan(operand);else
        if (f == "ctg") operand = atan(operand);else
        if (f == "lg") operand = log10(operand);else
        if (f == "ln") operand = log(operand);else
        if (f == "sqrt") operand = sqrt(operand);
        else
        cout << "Funcrion " << f << " incorrect." << endl;

}

/**
 * @brief evaluating
 * @param scanner
 * @param operandStack
 * @param operatorStack
 * @return
 */
double evaluating(TokenScanner & scanner, Stack <double> & operandStack,Stack <char> & operatorStack ){


     //for each token
     while(scanner.hasMoreTokens()){

         //Determine by its value and type
         string carrentToken = scanner.nextToken();
         TokenType carrentTokenType = scanner.getTokenType(carrentToken);

         //if the current token is number add it to the stack of operands
         if (carrentTokenType == TokenType::NUMBER){
             operandStack.push(atof(carrentToken.c_str()));
         }

         //if the operator is belong to the class TokenScanner
         if (carrentTokenType == TokenType::OPERATOR){

             //determined by its priority
             char carrentOperator =  carrentToken[0];
             int priority = priorityOperator ( carrentOperator);

             //If it is valid  simplest  the arithmetic operator "+-/*"
             //------------------------------------------------------
             if (priority >= 0){


                 if (!operatorStack.isEmpty()){
                     //solve the problem of expressions like 2^2^2
                     if(carrentOperator == '^'){
                         if(operatorStack.peek() == '^'){
                             priority++;
                         }
                     }

                     /*perform a simple sequence of statements
                      * the priority which is lower than the current operator
                      */
                     executeSmallerOper(operatorStack, operandStack, priority);

                 }
                 //if the operand stack is empty (tracking + or - in the begin of expression)
                 if(operandStack.isEmpty()){
                     if(carrentOperator == '+') continue;
                     if(carrentOperator == '-') operandStack.push(0);
                 }



                 operatorStack.push(carrentToken[0]);
             }
             else{
                 //If this is opening bracket
                 //------------------------
                 if(carrentOperator == '('){

                     //create new stacks for the expressions in parentheses
                     Stack <double> nOperandStack;
                     Stack <char> nOperatorStack;

                     //Initiate the evaluation of an expression in parentheses
                     double result = evaluating(scanner, nOperandStack, nOperatorStack );
                     operandStack.push(result);
                 }
                 if(carrentOperator == ')') {
                     if (!operatorStack.isEmpty()){
                         break;
                     }

                 }
             }



         }
         //If this is funtion
         //------------------
         if (carrentTokenType == TokenType::WORD){

             string currentFunction = carrentToken;

             //create new stacks for the calculation of the parameter of the function
             Stack <double> nOperandStack;
             Stack <char> nOperatorStack;

             // calculate parameter function recursively
             double result = evaluating(scanner, nOperandStack, nOperatorStack );

             //calculate function with the found parameter
             getFunction(currentFunction, result);
             operandStack.push(result);

         }
     }
     //if the stack of operators has an operator, do the remaining sequence of statements of the stack
     if (!operatorStack.isEmpty())
     executeSmallerOper(operatorStack, operandStack, -1); // the function will run until it will use up all the remaining operators in the stack

     double result = operandStack.pop();
     //cout << "result " << result << endl;
     return result;

}

/**
 * this function checks the pairing of parentheses
 * -----------------------------------------------
 * @brief errBrackets
 * @param expression
 * @return
 */
bool errBrackets(string expression){
    int counterbrackets = 0;
    for ( string::iterator symbol=expression.begin(); symbol!=expression.end(); ++symbol){

        if(*symbol == '(') counterbrackets++;
        if(*symbol == ')') counterbrackets--;
        if (counterbrackets < 0) break;//first ), and not (
    }
    cout << "counterbrackets = " << counterbrackets <<endl;
    return counterbrackets;
}


int main() {
    Stack <double> operand_Stack;
    Stack <char> operator_Stack;
    Stack <char>  parenthesis_Stack;

    string line = getLine("> ");

    if(errBrackets(line)){
        cout << "errBrackets" << endl;
        return 1;
    }
    TokenScanner expression(line);
    expression.scanNumbers();
    double result = evaluating(expression, operand_Stack, operator_Stack );
    cout << "result : " << result << endl;




    return 0;
}

