#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/common.h"
#include "../include/utils.h"
#include "../include/utf8conv.h"
#include <string.h>
#include <ctype.h>

kwdToken keywordsToken[] = {

    {"var", 3, TOKEN_VAR},
    {"func", 4, TOKEN_FUNC},
    {"if", 2, TOKEN_IF},
    {"else", 4, TOKEN_ELSE},
    {"while", 5, TOKEN_WHILE},
    {"for", 3, TOKEN_FOR},
    {"break", 5, TOKEN_BREAK},
    {"continue", 8, TOKEN_CONTINUE},
    {"return", 6, TOKEN_RETURN},
    {"null", 4, TOKEN_NULL},
    {"true", 4, TOKEN_TRUE},
    {"false", 5, TOKEN_FALSE},
    {"switch", 6, TOKEN_SWITCH},
    {"case", 4, TOKEN_CASE},
    {"systemcall", 10, TOKEN_SYSTEMCALL},
    {"print", 5, TOKEN_PRINT},
    {"class", 5, TOKEN_CLASS},
    {"is", 2, TOKEN_IS},
    {"static", 6, TOKEN_STATIC},
    {"this", 4, TOKEN_THIS},
    {"super", 5, TOKEN_SUPER},
    {"import", 6, TOKEN_IMPORT},
    {NULL, 0, TOKEN_UNKNOWN}

}; //系统关键字表

static TokenType IDorKeywd(const char *start, uint32_t wordLength)
{
    uint32_t index = 0;
    while (keywordsToken[index].keyword != NULL)
    {
        if (keywordsToken[index].length == wordLength && memcmp(keywordsToken[index].keyword, start, wordLength) == 0)
        {
            return keywordsToken[index].token;
        }
        index++;
    }
    return TOKEN_ID;
}

char lookAheadChar(Parser *parser)
{
    return *parser->nextCharPointer;
}

static void getNextChar(Parser *parser)
{
    parser->currentChar = *parser->nextCharPointer++;
}

static bool matchNextChar(Parser *parser, char exceptedChar)
{
    if (lookAheadChar(parser) == exceptedChar)
    {
        getNextChar(parser);
        return true;
    }
    return false;
}

static void skipSpaces(Parser *parser)
{
    while (isspace(parser->currentChar))
    {
        if (parser->currentChar == '\n')
        {
            parser->currentToken.lineNo++;
        }
        getNextChar(parser);
    }
}
/********************************************
 * ************  识别标识符  *****************
 * *****************************************/
static void parseID(Parser *parser, TokenType tokentype)
{
    while (isalnum(parser->currentChar) || parser->currentChar == '_')
    {
        getNextChar(parser);
    }
    uint32_t length = (uint32_t)(parser->nextCharPointer - parser->currentToken.start - 1);
    if (tokentype != TOKEN_UNKNOWN)
    {
        parser->currentToken.type = tokentype;
    }
    else
    {
        parser->currentToken.type = IDorKeywd(parser->currentToken.start, length);
    }
    parser->currentToken.length = length;
}
/********************************************************************
 **************************  转换Unicode码点  ************************
 *******************  用于转换形如"/uxxxx的Unicode码"  ***************
 *******************************************************************/
static void parseUnicodeCodePnt(Parser *parser, ByteBuffer *bytebuff)
{
    uint32_t index = 0;
    int value = 0;
    uint8_t digi = 0;

    while (index++ < 4)
    {
        getNextChar(parser);
        if (parser->currentChar == '\0') //因为缓冲区存的是字符，先转换成数字再使用Unicode
        {
            LEX_ERROR(parser, "UNTERMINATED UNICODE!!!");
        }
        if (parser->currentChar >= '0' && parser->currentChar <= '9')
        {
            digi = parser->currentChar - '0';
        }
        else if (parser->currentChar >= 'a' && parser->currentChar <= 'f')
        {
            digi = parser->currentChar - 'a' + 10; //十六进制转十进制
        }
        else if (parser->currentChar >= 'A' && parser->currentChar <= 'F')
        {
            digi = parser->currentChar - 'A' + 10; //十六进制转十进制
        }
        else
        {
            LEX_ERROR(parser, "INVALID UNICODE!!!");
        }
        value = value * 16 | digi;
    }
    uint32_t byteNum = getByteNOOfEncUTF8(value);
    ASSERT(byteNum != 0, "UTF8 Encode bytes should be between 1 and 4!");
    ByteBufferFillW(parser->virtualMach, bytebuff, 0, byteNum);
    UTF8Encoder(bytebuff->datas + bytebuff->count - byteNum, value);
}

static void parseString(Parser *parser)
{
    ByteBuffer stringbuffer;
    ByteBufferInit(&stringbuffer);
    while (true)
    {
        getNextChar(parser);
        if (parser->currentChar == '\0') //读取到文本结束符，不能再往下读取
        {
            LEX_ERROR(parser, "UNTERMINATED STRING!!!");
        }
        if (parser->currentChar == '"') //读取到文本起始的 ‘"’ 符号，将字段标识为字符串
        {
            parser->currentToken.type = TOKEN_STRING;
            break;
        }
        if (parser->currentChar == '%') //读取到 "%" 符号时，将该段字符标识为内嵌表达式，并检测是否缺失括号。
        {
            if (!matchNextChar(parser, '('))
            {
                LEX_ERROR(parser, " '%' should be followed by '('!");
            }
            if (parser->interpolationExceptRightParenNum > 0)
            {
                COMPILE_ERROR(parser, "Sorry,The nest interpolate expression is not supported!");
            }
            parser->interpolationExceptRightParenNum = 1;
            parser->currentToken.type = TOKEN_INTERPOLATION;
            break;
        }
        if (parser->currentChar == '\\') //读取到 "\" 符号时，将该段字符标识为转义符号，进行处理,普通字符串不处理。
        {
            getNextChar(parser);
            switch (parser->currentChar)
            {
            case '0': // "\0" 结束符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\0');
                break;
            case 'a': // "\a" 响铃符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\a');
                break;
            case 'b': // "\b" 退格符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\b');
                break;
            case 'f': // "\f" 换页符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\f');
                break;
            case 'n': // "\n" 换行符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\n');
                break;
            case 'r': // "\r" 回车符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\r');
                break;
            case 't': // "\t" 制表符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\t');
                break;
            case 'u': // "\u" Unicode符
                parseUnicodeCodePnt(parser, &stringbuffer);
                break;
            case '"': // "\"" 双引号字符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\"');
                break;
            case '\\': // "\\" 反斜杠字符
                ByteBufferAdd(parser->virtualMach, &stringbuffer, '\\');
                break;
            default: // 其他情况，不支持。
                LEX_ERROR(parser, "UNSUPPORT ESCAPE\\%c", parser->currentChar);
                break;
            }
        }
        else
        {
            ByteBufferAdd(parser->virtualMach, &stringbuffer, parser->currentChar); //针对普通字符串，直接传递字符
        }
        ByteBufferCLR(parser->virtualMach, &stringbuffer); //对虚拟机缓冲区进行回收清理
    }
}

//跳过空行的方法
static void skipAline(Parser *parser)
{
    getNextChar(parser);
    while (parser->currentChar != '\0')
    {
        if (parser->currentChar == '\n')
        {
            parser->currentToken.lineNo++;
            getNextChar(parser);
            break;
        }
        getNextChar(parser);
    }
}

//跳过注释的方法
static void skipComment(Parser *parser)
{
    char nextChar = lookAheadChar(parser);
    if (parser->currentChar == '/') //当出现 "//" 时，判定为行注释
    {
        skipAline(parser);
    }
    else //当出现成对的 "/*" 与 "*/" 时，判定为多行注释
    {
        while (nextChar != '*' && nextChar != '\0')
        {
            getNextChar(parser);
            if (parser->currentChar == '\n')
            {
                parser->currentToken.lineNo++;
            }
            nextChar = lookAheadChar(parser);
        }
        if (matchNextChar(parser, '*'))
        {
            if (!matchNextChar(parser, '/'))
            {
                LEX_ERROR(parser, "Except '/' after '*' !");
            }
            getNextChar(parser);
        }
        else
        {
            LEX_ERROR(parser, "Except '*/' before the end!");
        }
    }
    skipSpaces(parser);
}

//识别token的方法
void getNextToken(Parser *parser)
{
    parser->preToken = parser->currentToken;
    skipSpaces(parser);
    parser->currentToken.type = TOKEN_EOF;
    parser->currentToken.length = 0;
    parser->currentToken.start = parser->nextCharPointer - 1;
    while (parser->currentChar != '\0')
    {
        switch (parser->currentChar)
        {
        case ',':
            parser->currentToken.type = TOKEN_COMMA;
            break;
        case ':':
            parser->currentToken.type = TOKEN_COLON;
            break;
        case '(':
            if (parser->interpolationExceptRightParenNum > 0)
            {
                parser->interpolationExceptRightParenNum++;
            }
            parser->currentToken.type = TOKEN_LEFT_PAREN;
            break;
        case ')':
            if (parser->interpolationExceptRightParenNum > 0)
            {
                parser->interpolationExceptRightParenNum--;
                if (parser->interpolationExceptRightParenNum == 0)
                {
                    parseString(parser);
                    break;
                }
            }
            parser->currentToken.type = TOKEN_RIGHT_PAREN;
            break;
        case '[':
            parser->currentToken.type = TOKEN_LEFT_BRACKET;
            break;
        case ']':
            parser->currentToken.type = TOKEN_RIGHT_BRACKET;
            break;
        case '{':
            parser->currentToken.type = TOKEN_LEFT_BRACE;
            break;
        case '}':
            parser->currentToken.type = TOKEN_RIGHT_BRACE;
            break;
        case '.':
            if (matchNextChar(parser, '.'))
            {
                parser->currentToken.type = TOKEN_DUO_DOT;
            }
            else
            {
                parser->currentToken.type = TOKEN_DOT;
            }
            break;
        case '=':
            if (matchNextChar(parser, '='))
            {
                parser->currentToken.type = TOKEN_EQUAL;
            }
            else
            {
                parser->currentToken.type = TOKEN_ASSIGN;
            }
            break;
        case '+':
            parser->currentToken.type = TOKEN_ADD;
            break;
        case '-':
            parser->currentToken.type = TOKEN_SUB;
            break;
        case '*':
            parser->currentToken.type = TOKEN_MUL;
            break;
        case '/':
            if (matchNextChar(parser, '/') || matchNextChar(parser, '*'))
            {
                skipComment(parser);
                parser->currentToken.start = parser->nextCharPointer - 1;
                continue;
            }
            else
            {
                parser->currentToken.type = TOKEN_DIV;
            }
            break;
        case '%':
            parser->currentToken.type = TOKEN_MOD;
            break;
        case '&':
            if (matchNextChar(parser, '&'))
            {
                parser->currentToken.type = TOKEN_LOGIC_AND;
            }
            else
            {
                parser->currentToken.type = TOKEN_BIT_AND;
            }
            break;
        case '|':
            if (matchNextChar(parser, '|'))
            {
                parser->currentToken.type = TOKEN_LOGIC_OR;
            }
            else
            {
                parser->currentToken.type = TOKEN_BIT_OR;
            }
            break;
        case '~':
            parser->currentToken.type = TOKEN_BIT_NOT;
            break;
        case '?':
            parser->currentToken.type = TOKEN_QUESTION;
            break;
        case '>':
            if (matchNextChar(parser, '='))
            {
                parser->currentToken.type = TOKEN_GREATE_EQUAL;
            }
            else if (matchNextChar(parser, '>'))
            {
                parser->currentToken.type = TOKEN_BIT_SHIFT_RIGHT;
            }
            else
            {
                parser->currentToken.type = TOKEN_GREATE;
            }
            break;
        case '<':
            if (matchNextChar(parser, '='))
            {
                parser->currentToken.type = TOKEN_LESS_EQUAL;
            }
            else if (matchNextChar(parser, '<'))
            {
                parser->currentToken.type = TOKEN_BIT_SHIFT_LEFT;
            }
            else
            {
                parser->currentToken.type = TOKEN_LESS;
            }
            break;
        case '!':
            if (matchNextChar(parser, '='))
            {
                parser->currentToken.type = TOKEN_NOT_EQUAL;
            }
            else
            {
                parser->currentToken.type = TOKEN_LOGIC_NOT;
            }
            break;
        case '"':
            parseString(parser);
            break;
        default:
            if (isalpha(parser->currentChar) || parser->currentChar == '_')
            {
                parseID(parser, TOKEN_UNKNOWN);
            }
            else
            {
                if (parser->currentChar == '#' && matchNextChar(parser, '!'))
                {
                    skipAline(parser);
                    parser->currentToken.start = parser->nextCharPointer - 1;
                    continue;
                }
                LEX_ERROR(parser, "Unsupport Character: \'%c\',Program Quit.", parser->currentChar);
            }
            return;
        }
        parser->currentToken.length = (uint32_t)(parser->nextCharPointer - parser->currentToken.start);
        getNextChar(parser);
        return;
    }
}

bool matchToken(Parser *parser, TokenType excepted)
{
    if (parser->currentToken.type == excepted)
    {
        getNextToken(parser);
        return true;
    }
    return false;
}

void consumeCurrentToken(Parser *parser, TokenType excepted, const char *errorMessage)
{
    if (parser->currentToken.type != excepted)
    {
        COMPILE_ERROR(parser, errorMessage);
    }
    getNextToken(parser);
}

void consumeNextToken(Parser *parser, TokenType excepted, const char *errorMessage)
{
    getNextToken(parser);
    if (parser->currentToken.type != excepted)
    {
        COMPILE_ERROR(parser, errorMessage);
    }
}

void initParser(VM *virtualMachine, Parser *parser, const char *file, const char *sourceCode)
{
    parser->file = file;
    parser->sourceCode = sourceCode;
    parser->currentChar = *parser->sourceCode;
    parser->nextCharPointer = parser->sourceCode + 1;
    parser->currentToken.lineNo = 1;
    parser->currentToken.length = 0;
    parser->currentToken.type = TOKEN_UNKNOWN;
    parser->currentToken.start = NULL;
    parser->preToken = parser->currentToken;
    parser->interpolationExceptRightParenNum = 0;
    parser->virtualMach = virtualMachine;
}