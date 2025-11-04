#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEXEME_LENGTH 256
#define KEYWORD_COUNT 16 // Total number of keywords; can be made dynamic if needed

//input classification functions
static bool isalpha(char input){
    return (input >= 'a' && input <= 'z' || (input >= 'A' && input <= 'Z'));
}
static bool isdigit(char input){
    return (input >= '0' && input <= '9');
}
static bool isspace(char input){
    return input == ' '  || // Space
           input == '\t'; // Horizontal tab
}
static bool isalphanumeric(char input){
    return (isalpha(input) || isdigit(input));
}
static bool isescapesequence(char input){
    return (input == 'n' || input =='t' || input == '\\');
}

// Token Type Enumeration
typedef enum {
    Token_Identifier,
    Token_Character,
    Token_String,
    Token_Number,
    Token_Operator,
    Token_CodeEnd,
    Token_Unknown,
    Token_Delimeter,
    Token_Comment,
    Token_Arithmetic_Operator,
    Token_Boolean_Operator,
    Token_Assignment_Operator,
    Token_Builtin_Constant,
    Token_Keyword_If,
    Token_Keyword_Else,
    Token_Keyword_ElseIf,
    Token_Keyword_For,
    Token_Keyword_Int,
    Token_Keyword_Decimal,
    Token_Keyword_Char,
    Token_Keyword_String,
    Token_Keyword_Boolean,
    Token_Keyword_Read,
    Token_Keyword_Write,
    Token_Reserved_True,
    Token_Reserved_False,
    Token_Reserved_Null,
    Token_Noise_Do,
    Token_Delim_LPAR,
    Token_Delim_RPAR,
    Token_Delim_LBRAC,
    Token_Delim_RBRAC,
    Token_Delim_LBRAK,
    Token_Delim_RBRAK,
    Token_Delim_Comma,
    Token_Delim_SQuote,
    Token_Delim_DQuote,
    Token_Delim_Period
} Token_Type; 

// Token Structure
typedef struct {
    Token_Type type;
    char lexeme[MAX_LEXEME_LENGTH];
    int line_number;
} Token;

// Keyword Structure
typedef struct{
    char* word;
    Token_Type type;
} Keyword;

// Lookup Table
Keyword keywords[] = {
    {"if", Token_Keyword_If},
    {"else", Token_Keyword_Else},
    {"elseif", Token_Keyword_ElseIf},
    {"for", Token_Keyword_For},
    {"int", Token_Keyword_Int},
    {"decimal", Token_Keyword_Decimal},
    {"char", Token_Keyword_Char},
    {"string", Token_Keyword_String},
    {"boolean", Token_Keyword_Boolean},
    {"read", Token_Keyword_Read},
    {"write", Token_Keyword_Write},
    {"true", Token_Reserved_True},
    {"false", Token_Reserved_False},
    {"null", Token_Reserved_Null},
    {"do", Token_Noise_Do}
};

// Function Prototypes
Token getNextToken(FILE* srcFile);
Token_Type getlexemeType(const char* lexeme);
void outputToken(Token token);



int main(){
    printf("Hello, world!\n");
    return 0;
}