#include "utils.h"
#include "virtualmachine.h"
#include "parser.h"
#include <stdlib.h>
#include <stdarg.h>

void *memoryManager(VM *virtualMach, void *pointer, uint32_t olderSize, uint32_t newerSize)
{
    //累计系统分配的总内存
    virtualMach->allocatedBytes += newerSize - olderSize;

    //避免realloc(NULL, 0)定义的新地址,此地址不能被释放
    if (newerSize == 0)
    {
        free(pointer);
        return NULL;
    }

    return realloc(pointer, newerSize);
}

uint32_t ceil2PowerOf2(uint32_t n)
{
    n += (n == 0); //修复当n等于0时结果为0的边界情况
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

DEFINE_BUFFER_METHOD(String)
DEFINE_BUFFER_METHOD(Integer)
DEFINE_BUFFER_METHOD(Char)
DEFINE_BUFFER_METHOD(Word)
DEFINE_BUFFER_METHOD(Byte)

void symTabCLR(VM *virtualMach, SymTab *buffer)
{
    uint32_t idx = 0;
    while (idx < buffer->count)
    {
        memoryManager(virtualMach, buffer->datas[idx++].str, 0, 0);
    }
    StringBufferCLR(virtualMach, buffer);
}

void errRepo(void *parser, ErrType errType, const char *format, ...)
{
    char buffer[DEFAULT_BUfFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, DEFAULT_BUfFER_SIZE, format, ap);
    va_end(ap);

    switch (errType)
    {
    case ERROR_IO:
    case ERROR_MEM:
        fprintf(stderr, "%s:%d In function %s():%s\n", __FILE__, __LINE__, __func__, buffer);
        break;
    case ERROR_LEX:
    case ERROR_COMPILE:
        ASSERT(parser != NULL, "parser is null!");
        fprintf(stderr, "%s:%d \"%s\"\n", ((Parser *)parser)->file, ((Parser *)parser)->preToken.lineNo, buffer);
        break;
    case ERROR_RUNTIME:
        fprintf(stderr, "%s\n", buffer);
        break;
    default:
        NOT_REACHED();
    }
    exit(1);
}