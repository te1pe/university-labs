#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#define MAX_STACK_SIZE 10001 
 
typedef struct { 
    int stack[MAX_STACK_SIZE]; 
    int top; 
} Stack; 
 
int is_Valid(const char *expr) { 
    int bracket = 0;  
    int i = 0; 
    int len_expr = strlen(expr); 
    if (len_expr == 0) {
        return 0;
    } 
    if (expr[len_expr - 1] != ')' && !isdigit(expr[len_expr - 1])) {
        return 0;
    } 
    while (expr[i] != '\0') { 
        if (expr[i] == '(') bracket++; 
        else if (expr[i] == ')') { 
            bracket--;
            if (i > 0 && expr[i - 1] == '(') { return 0; }
            if (i > 1 && expr[i - 2] == '(' && strchr("+-*/", expr[i - 1])) {
                return 0;
            } 
            if (bracket < 0) return 0; 
        } 
        if (expr[i] == ' '){
            return 0;
        }
        if (!isdigit(expr[i]) && !strchr("+-*/() ", expr[i])){
            return 0;
        }  
        if (i > 0 && strchr("+-*/", expr[i - 1]) && strchr("+-*/", expr[i])){
            return 0;
        } 
        i++; 
    } 
    return bracket == 0;  
} 
 
int Push(Stack *s, int num) { 
    if (s->top == MAX_STACK_SIZE - 1) { 
        printf("Stack overflow"); 
        return 0; 
    } 
    s->stack[++s->top] = num; 
    return 1; 
} 

int Pop(Stack *s) { 
    if (s->top == -1) { 
        printf("Stack underflow"); 
        return 0; 
    } 
    return s->stack[s->top--]; 
} 
 
int Priority(char op) { 
    if (op == '+' || op == '-') return 1; 
    if (op == '*' || op == '/') return 2; 
    return 0; 
} 
 
void InfixToPostfix(const char *infix, char *postfix, Stack *s) { 
    int j = 0; 
    for (int i = 0; infix[i] != '\0'; i++) { 
        if (isspace(infix[i])){
            continue;
        }
        if (isdigit(infix[i])) {
            while (isdigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' ';
            i--;
        }
        else if (infix[i] == '(') { 
            Push(s, infix[i]); 
        } else if (infix[i] == ')') { 
            while (s->top != -1 && s->stack[s->top] != '(') { 
                postfix[j++] = Pop(s); 
                postfix[j++]=' ';
            } 
            if (s->top != -1 && s->stack[s->top] == '(') { 
                Pop(s);  
            } 
        } else { 
            while (s->top != -1 && Priority(s->stack[s->top]) >= Priority(infix[i])) { 
                postfix[j++] = Pop(s); 
                postfix[j++]=' ';
            } 
            Push(s, infix[i]); 
            postfix[j++]=' ';
        } 
    } 
    while (s->top != -1) { 
        postfix[j++] = Pop(s);
        postfix[j++]=' ';
    } 
    postfix[j-1] = '\0'; 
} 
 
int Calculate(const char *postfix, Stack *s) {
    for (int i = 0; postfix[i] != '\0'; i++) {
        if (isdigit(postfix[i])) {
            int num = 0;
            while (isdigit(postfix[i])) {
                num = num * 10 + (postfix[i++] - '0');
            }
            Push(s, num);
        } else if (strchr("+-*/", postfix[i])) {
            int b = Pop(s);
            int a = Pop(s);
            switch (postfix[i]) {
                case '+': Push(s, a + b); break;
                case '-': Push(s, a - b); break;
                case '*': Push(s, a * b); break;
                case '/':
                    if (b == 0) {
                        printf("division by zero");
                        return 0;
                    }
                    Push(s, a / b);
                    break;
            }
        }
    }
    return Pop(s);
}
 
int main(void) { 
    char infix[MAX_STACK_SIZE]; 
    if (!fgets(infix, sizeof(infix), stdin)) { 
        return 0; 
    } 
 
    size_t len = strlen(infix); 
    if (len > 0 && infix[len - 1] == '\n') { 
        infix[len - 1] = '\0'; 
    } 
 
    if (!is_Valid(infix)) { 
        printf("syntax error"); 
        return 0; 
    } 
 
    char postfix[MAX_STACK_SIZE]; 
    Stack s = { .top = -1 }; 
    InfixToPostfix(infix, postfix, &s); 
    s.top = -1;  
    int result = Calculate(postfix, &s); 
    printf("%d", result); 
    return 0; 
}

