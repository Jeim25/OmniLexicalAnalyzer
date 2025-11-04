#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LEXEME_LENGTH 256
#define KEYWORD_COUNT 16 // Total number of keywords; can be made dynamic if needed

//input classification functions (renamed to avoid clashing with libc)
static bool is_alpha(char input){
    return (input >= 'a' && input <= 'z' || (input >= 'A' && input <= 'Z'));
}
static bool is_digit(char input){
    return (input >= '0' && input <= '9');
}
static bool is_space(char input){
    return input == ' '  || 
           input == '\t' || 
           input == '\n'; 
}
static bool is_alphanumeric(char input){
    return (is_alpha(input) || is_digit(input));
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
    Token_Single_Line_Comment,
    Token_Block_Comment,
    Token_Arithmetic_Operator,
    Token_Boolean_Operator,
    Token_Assignment_Operator,
    Token_Arithmetic_Operator_DIV, 
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
    {"do", Token_Noise_Do},
    {"DIV", Token_Arithmetic_Operator_DIV}
};

// Function Prototypes
Token_Type getlexemeType(const char* lexeme);

//Helper Functions for getNextToken
const char *inputStream;
int streamIndex = 0;
int currentLine = 1;

char peekChar(){
    if (!inputStream) return '\0';
    char c = inputStream[streamIndex];
    if (c == '\0') return '\0';
    return c;
}

char getChar() {
    if (!inputStream) return '\0';
    char c = inputStream[streamIndex];
    if (c == '\0') return '\0';
    streamIndex++;
    if (c == '\n') {
        currentLine++;
    }
    return c;
}
Token createToken(Token_Type type, const char* lexemeStart, int len) {
    Token token;
    token.type = type;
    token.line_number = currentLine;
    strncpy(token.lexeme, lexemeStart, len);
    token.lexeme[len] = '\0'; 
    return token;
}
//Helper Functions for getNextToken

//Automaton States
typedef enum{
    STATE_START,
    STATE_IN_IDENTIFIER,
    STATE_IN_NUMBER,
    STATE_IN_CHAR,
    STATE_IN_CHAR_EXPECT_CLOSE,
    STATE_IN_CHAR_ESCAPE,
    STATE_IN_STRING,
    STATE_IN_STRING_ESCAPE,
    STATE_IN_TILDE,
    STATE_IN_SINGLE_LINE_COMMENT,
    STATE_IN_BLOCK_COMMENT,
    STATE_IN_BLOCK_COMMENT_TILDE,
    //special states for DIV operator
    STATE_IN_D_DIV,
    STATE_IN_I,
    STATE_IN_V,
    //special states for or operator
    STATE_IN_O,
    STATE_IN_R,
    //special states for and operator
    STATE_IN_A,
    STATE_IN_N,
    STATE_IN_D_AND,
    //special states for = and == 
    STATE_IN_EQUAL,
    STATE_DONE,
}AutomatonState;
//Automaton States

//getNextToken Function
Token getNextToken(){
    AutomatonState currentState = STATE_START;
    char lexemeBuffer[MAX_LEXEME_LENGTH];
    int lexemeIndex = 0;
    char currentChar;

    while(currentState != STATE_DONE){
        currentChar = peekChar();

        switch(currentState){
            case STATE_START:
                if(is_space(currentChar)){
                    getChar();
                }
                else if(strchr("()[]{},", currentChar)){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Delimeter, lexemeBuffer, lexemeIndex);
                }
                else if(strchr("+-*%/^", currentChar)){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Arithmetic_Operator, lexemeBuffer, lexemeIndex);
                }
                else if(currentChar == 'D'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_D_DIV;
                }
                else if(currentChar == 'o'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_O;
                }
                else if(currentChar == 'a'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_A;
                }
                else if(is_alpha(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else if(is_digit(currentChar)){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_NUMBER;
                }
                else if(currentChar == '\''){
                    getChar();
                    currentState = STATE_IN_CHAR;
                }
                else if(currentChar == '\"'){
                    getChar();
                    currentState = STATE_IN_STRING;
                }
                else if(currentChar == '~' ){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_TILDE;
                }
                else if(currentChar == '='){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_EQUAL;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_IDENTIFIER:
                if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                }
                else{
                    currentState = STATE_DONE;
                    // Change "Token_Identifier" with lookup table function call
                    Token_Type finalType = Token_Identifier;
                    return createToken(finalType, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_NUMBER:
                if(is_digit(currentChar)) {
                    lexemeBuffer[lexemeIndex++] = getChar();
                }
                else{
                    currentState = STATE_DONE;
                    return createToken(Token_Number, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_CHAR:
                if(currentChar == '\\'){
                    getChar();
                    currentState = STATE_IN_CHAR_ESCAPE;
                }
                else if(currentChar =='\n'|| currentChar == '\0'){ //UNCLOSED CHAR ERROR
                    currentState = STATE_DONE; 
                    return createToken(Token_Unknown, "Unclosed Char", 13);
                }
                else if(currentChar == '\''){
                    getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, "Empty char literal", 18);
                }
                else{
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_CHAR_EXPECT_CLOSE;
                }

                break;

            case STATE_IN_CHAR_EXPECT_CLOSE:
                if(currentChar == '\'') {
                    getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Character, lexemeBuffer, lexemeIndex);
                }
                else{
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, "Multi-character literal", 22);
                }
                break;

            case STATE_IN_CHAR_ESCAPE:
                switch (currentChar) {
                    case 'n': 
                        lexemeBuffer[lexemeIndex++] = '\n'; 
                        getChar(); 
                        break;
                    case 't': 
                        lexemeBuffer[lexemeIndex++] = '\t'; 
                        getChar(); 
                        break;
                    case '\'': 
                        lexemeBuffer[lexemeIndex++] = '\''; 
                        getChar(); 
                        break;
                    case '\\': 
                        lexemeBuffer[lexemeIndex++] = '\\'; 
                        getChar(); 
                        break;
                    default:
                        currentState = STATE_DONE;
                        return createToken(Token_Unknown, "Invalid escape sequence", 23);
                }
                currentState = STATE_IN_CHAR_EXPECT_CLOSE; 
                break;

            case STATE_IN_STRING:
                if(currentChar == '\\'){
                    getChar();
                    currentState = STATE_IN_STRING_ESCAPE;
                }
                else if(currentChar == '\"'){
                    getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_String, lexemeBuffer, lexemeIndex);
                }
                else if(currentChar =='\n'|| currentChar == '\0'){ //UNCLOSED STRING ERROR
                    currentState = STATE_DONE; 
                    return createToken(Token_Unknown, "Unclosed String", 15);
                }
                else{
                    lexemeBuffer[lexemeIndex++] = getChar();
                }
                break;
            
            case STATE_IN_STRING_ESCAPE:
                switch (currentChar) {
                    case 'n':
                        lexemeBuffer[lexemeIndex++] = '\n'; 
                        getChar(); 
                        break;
                    case 't':
                        lexemeBuffer[lexemeIndex++] = '\t'; 
                        getChar(); // 
                        break;
                    case '\"':
                        lexemeBuffer[lexemeIndex++] = '\"'; 
                        getChar(); // Consume the '"'
                        break;
                    case '\\':
                        lexemeBuffer[lexemeIndex++] = '\\'; 
                        getChar(); //
                        break;
                    default:
                        currentState = STATE_DONE;
                        return createToken(Token_Unknown, "Invalid escape sequence", 23);
                }
                currentState = STATE_IN_STRING;
                break;  

            case STATE_IN_D_DIV:
                if(currentChar == 'I'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_I;
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_TILDE:
                if(currentChar == '/'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_BLOCK_COMMENT;
                }
                else{
                    currentState = STATE_IN_SINGLE_LINE_COMMENT;
                }
                break;
            
            case STATE_IN_SINGLE_LINE_COMMENT:
                if (currentChar == '\n' || currentChar == '\0') {
                    currentState = STATE_DONE;
                    return createToken(Token_Single_Line_Comment, lexemeBuffer, lexemeIndex);
                }
                else {
                    lexemeBuffer[lexemeIndex++] = getChar();
                }
                break;

            case STATE_IN_BLOCK_COMMENT:
                if(currentChar == '/') {
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_BLOCK_COMMENT_TILDE;
                }
                else if (currentChar == '\0') {
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, "Unclosed block comment", 22);
                }
                else {
                    lexemeBuffer[lexemeIndex++] = getChar();
    
                }
                break;
                
            case STATE_IN_BLOCK_COMMENT_TILDE:
                if (currentChar == '~') {
                    lexemeBuffer[lexemeIndex++] = getChar(); 
                    currentState = STATE_DONE;
                    return createToken(Token_Block_Comment, lexemeBuffer, lexemeIndex);
                }
                else if (currentChar == '\0') {
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, "Unclosed block comment", 22);
                }
                else {
                    currentState = STATE_IN_BLOCK_COMMENT;
                }
                break;
                
            case STATE_IN_I:
                if(currentChar == 'V'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_V;
                    
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_V:
                if(is_space(currentChar)){ 
                    currentState = STATE_DONE;
                    return createToken(Token_Arithmetic_Operator, lexemeBuffer, lexemeIndex);
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{ 
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;
                
            case STATE_IN_O:
                if(currentChar == 'r'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_R;
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;
            
            case STATE_IN_R:
                if(is_space(currentChar)){ 
                    currentState = STATE_DONE;
                    return createToken(Token_Boolean_Operator, lexemeBuffer, lexemeIndex);
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{ 
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;

            case STATE_IN_A:
                if(currentChar == 'n'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_N;
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;

            case STATE_IN_N:
                if(currentChar == 'd'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_D_AND;
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;

            case STATE_IN_D_AND:
                if(is_space(currentChar)){
                    currentState = STATE_DONE;
                    return createToken(Token_Boolean_Operator, lexemeBuffer, lexemeIndex);
                }
                else if(is_alphanumeric(currentChar) || currentChar == '_'){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_IN_IDENTIFIER;
                }
                else{ 
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;

            case STATE_IN_EQUAL:
                if(is_space(currentChar)){
                    currentState = STATE_DONE;
                    return createToken(Token_Assignment_Operator, lexemeBuffer, lexemeIndex);
                }
                else if(currentChar == '='){
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Boolean_Operator, lexemeBuffer, lexemeIndex);
                }
                else{ 
                    //unkown
                    lexemeBuffer[lexemeIndex++] = getChar();
                    currentState = STATE_DONE;
                    return createToken(Token_Unknown, lexemeBuffer, lexemeIndex);
                }
                break;

            case STATE_DONE:
                break;

        }
    }
    return createToken(Token_CodeEnd, "EOF", 3);
}
//getNextToken Function

int main(){
    printf("Hello, world!\n");
    return 0;
}

Token getNextToken(FILE* srcFile){
    Token token;
    int state = 0; // Start state
    char ch; // Current character
    int lexemeIndex = 0; // Index for lexeme
    token.line_number = 1; // Initialize line number
    memset(token.lexeme, 0, MAX_LEXEME_LENGTH); // Clear lexeme buffer

    while((ch = fgetc(srcFile)) != EOF){
        switch(state){
            case 0: // Start state
                if(isspace(ch)){
                    if(ch == '\n') token.line_number++;
                    continue; // Ignore whitespace
                }
                else token.lexeme[lexemeIndex++] = ch; // Add character to lexeme

                if(isalpha(ch) || ch == '_') state = 1; // Identifier state
                else if(isdigit(ch)) state = 2; // Number state
                else if(ch == '~') state = 3; // Comment state
                else if(ch == '"') state = 4; // String state
                else if(ch == '\'') state = 5; // Character state
                //else if(strchr("+-*=%!<>", ch)) state = 6; // Operator state
                else if(strchr("(){}[],.;", ch)) state = 7; // Delimeter state
                else {
                    token.lexeme[lexemeIndex] = '\0';
                    token.type = getlexemeType(token.lexeme);
                    return token; // Return single character tokens
                }
                break;

            case 1: // Identifier state
                if(isalnum(ch) || ch == '_'){
                    token.lexeme[lexemeIndex++] = ch;
                } else {
                    ungetc(ch, srcFile); // Put back the non-identifier character
                    token.lexeme[lexemeIndex] = '\0';
                    token.type = getlexemeType(token.lexeme);
                    return token;
                }
                break;

            case 2: // Number state
                if(isdigit(ch) || ch == '.') token.lexeme[lexemeIndex++] = ch;
                else { // Not a number character
                    ungetc(ch, srcFile); // Put back the non-number character
                    token.lexeme[lexemeIndex] = '\0';
                    token.type = Token_Number;
                    return token;
                }
                break;
            
            case 3: // Comment state (not implemented)
                // Handle comments  here
                break;

            case 4: // String state (not implemented)
                if(ch != '"'){
                    token.lexeme[lexemeIndex++] = ch; // Add character to string
                } 
                else { // End of string
                    token.lexeme[lexemeIndex] = '\0';
                    token.type = Token_String;
                    return token;
                }
                break;

            case 5: // Character state (not implemented)
                if(ch != '\''){
                    token.lexeme[lexemeIndex++] = ch; // Add character to char
                } 
                else { // End of character
                    token.lexeme[lexemeIndex] = '\0';
                    token.type = Token_Character;
                    return token;
                }
                break;

            case 6: // Operator state (not implemented)
                // Handle multi-character operators here
                break;

            case 7: // Delimeter state (not implemented)
                // Handle delimeters here
                break;

            case 8: // End of file state
                token.type = Token_CodeEnd;
                return token;
            
            default:
                break;
        }
    }

    return token;
}