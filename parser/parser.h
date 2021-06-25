#ifndef _PARSER_PARSER_H
#define _PARSER_PARSER_H

#include "../include/common.h"
#include "../VM/virtualmachine.h"

typedef enum
{
    TOKEN_UNKNOWN, //未知Token容器

    /**************************
            数据类型枚举
    **************************/

    TOKEN_NUMBER,        //数字类型
    TOKEN_STRING,        //字符串类型
    TOKEN_ID,            //变量信息
    TOKEN_INTERPOLATION, //内嵌表达式

    /**************************
           系统保留关键字
    **************************/

    TOKEN_VAR,        //变量，var
    TOKEN_FUNC,       //方法，func
    TOKEN_IF,         //if
    TOKEN_ELSE,       //else
    TOKEN_WHILE,      //while
    TOKEN_FOR,        //for
    TOKEN_BREAK,      //break
    TOKEN_CONTINUE,   //continue
    TOKEN_RETURN,     //return
    TOKEN_NULL,       //null
    TOKEN_TRUE,       //true
    TOKEN_FALSE,      //false
    TOKEN_SWITCH,     //switch
    TOKEN_CASE,       //case
    TOKEN_SYSTEMCALL, //systemcall，唤起系统内方法
    TOKEN_PRINT,      //print，用于打印输出

    /**************************
         类，模块相关关键字
    **************************/

    TOKEN_CLASS,  //class
    TOKEN_THIS,   //this
    TOKEN_STATIC, //static
    TOKEN_IS,     //is
    TOKEN_SUPER,  //super
    TOKEN_IMPORT, //import

    /**************************
              分隔符
    **************************/

    TOKEN_COMMA,         //,
    TOKEN_COLON,         //:
    TOKEN_LEFT_PAREN,    //(
    TOKEN_RIGHT_PAREN,   //)
    TOKEN_LEFT_BRACE,    //{
    TOKEN_RIGHT_BRACE,   //}
    TOKEN_LEFT_BRACKET,  //[
    TOKEN_RIGHT_BRACKET, //]
    TOKEN_DOT,           //.
    TOKEN_DUO_DOT,       //..

    /**************************
              运算符
    **************************/

    //简单双目运算符
    TOKEN_ADD, //+
    TOKEN_SUB, //-
    TOKEN_MUL, //*
    TOKEN_DIV, ///
    TOKEN_MOD, //%
    //************

    //赋值运算符
    TOKEN_ASSIGN, //=
    //*********

    //位运算符
    TOKEN_BIT_AND,         //&
    TOKEN_BIT_OR,          //|
    TOKEN_BIT_NOT,         //~
    TOKEN_BIT_SHIFT_RIGHT, //>>
    TOKEN_BIT_SHIFT_LEFT,  //<<
    //************************

    //逻辑运算符
    TOKEN_LOGIC_AND, //&&
    TOKEN_LOGIC_OR,  //||
    TOKEN_LOGIC_NOT, //!
    //***********************

    //关系操作符
    TOKEN_EQUAL,        //==
    TOKEN_NOT_EQUAL,    //!=
    TOKEN_GREATE,       //>
    TOKEN_GREATE_EQUAL, //>=
    TOKEN_LESS,         //<
    TOKEN_LESS_EQUAL,   //<=
    //***********************

    TOKEN_QUESTION, //?

    TOKEN_EOF, //EOF
} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    uint32_t length;
    uint32_t lineNo;
} Token;

typedef struct keywordToken
{
    char *keyword;
    uint8_t length;
    TokenType token;
} kwdToken;

struct parser
{
    const char *file;
    const char *sourceCode;
    const char *nextCharPointer;
    char currentChar;
    Token currentToken;
    Token preToken;
    int interpolationExceptRightParenNum;
    VM *virtualMach;
};

#define PEEK_TOKEN(parserPointer) parserPointer->currentToken.type

char lookAheadChar(Parser *parser);
void getNextToken(Parser *parser);
bool matchToken(Parser *parser, TokenType excepted);
void consumeCurrentToken(Parser *parser, TokenType excepted, const char *errorMessage);
void consumeNextToken(Parser *parser, TokenType excepted, const char *errorMessage);
uint32_t getByteNOOfEncUTF8(int value);
uint8_t UTF8Encoder(uint8_t *buff, int value);
void initParser(VM *virtualMachine, Parser *parser, const char *file, const char *sourceCode);

#endif