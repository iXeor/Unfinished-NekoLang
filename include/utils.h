#ifndef _INCLUDE_UTILS_H
#define _INCLUDE_UTILS_H
#include "common.h"

void *memoryManager(VM *virtualMach, void *pointer, uint32_t olderSize, uint32_t newerSize);
uint32_t ceil2PowerOf2(uint32_t n);

#define ALLOCATE(virtualMachPointer, type) (type *)memoryManager(virtualMachPointer, NULL, 0, sizeof(type))

#define ALLOCATE_EXTRA(virtualMachPointer, mainType, extraSize) (mainType *)memoryManager(virtualMachPointer, NULL, 0, sizeof(mainType) + extraSize)

#define ALLOCATE_ARRAY(virtualMachPointer, type, count) (type *)memoryManager(virtualMachPointer, NULL, 0, sizeof(type) * count)

#define DEALLOCATE_ARRAY(virtualMachPointer, arrayPtr, count) memoryManager(virtualMachPointer, arrayPtr, sizeof(arrayPtr[0]) * count, 0)

#define DEALLOCATE(virtualMachPointer, memPtr) memoryManager(virtualMachPointer, memPtr, 0, 0)

typedef struct
{
    char *str;
    uint32_t length;
} String;

typedef struct
{
    uint32_t length; //除结束'\0'之外的字符个数
    char start[0];   //柔性数组
} CharValue;         //字符串缓冲区

//声明buffer类型
#define DECLARE_BUFFER_TYPE(type)                                                                 \
    typedef struct                                                                                \
    {                                                                                             \
        type *datas;                                                                              \
        uint32_t count;                                                                           \
        uint32_t capacity;                                                                        \
    } type##Buffer;                                                                               \
    void type##BufferInit(type##Buffer *buffer);                                                  \
    void type##BufferFillW(VM *virtualMech, type##Buffer *buffer, type data, uint32_t fillCount); \
    void type##BufferAdd(VM *virtualMech, type##Buffer *buffer, type data);                       \
    void type##BufferCLR(VM *virtualMech, type##Buffer *buffer);

//定义buffer方法
#define DEFINE_BUFFER_METHOD(type)                                                                 \
    void type##BufferInit(type##Buffer *buffer)                                                    \
    {                                                                                              \
        buffer->datas = NULL;                                                                      \
        buffer->count = buffer->capacity = 0;                                                      \
    }                                                                                              \
                                                                                                   \
    void type##BufferFillW(VM *virtualMech, type##Buffer *buffer, type data, uint32_t fillCount)   \
    {                                                                                              \
        uint32_t newCounts = buffer->count + fillCount;                                            \
        if (newCounts > buffer->capacity)                                                          \
        {                                                                                          \
            size_t olderSize = buffer->capacity * sizeof(type);                                    \
            buffer->capacity = ceil2PowerOf2(newCounts);                                           \
            size_t newSize = buffer->capacity * sizeof(type);                                      \
            ASSERT(newSize > olderSize, "Memory allocated!");                                      \
            buffer->datas = (type *)memoryManager(virtualMech, buffer->datas, olderSize, newSize); \
        }                                                                                          \
        uint32_t counter = 0;                                                                      \
        while (counter < fillCount)                                                                \
        {                                                                                          \
            buffer->datas[buffer->count++] = data;                                                 \
            counter++;                                                                             \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    void type##BufferAdd(VM *virtualMech, type##Buffer *buffer, type data)                         \
    {                                                                                              \
        type##BufferFillW(virtualMech, buffer, data, 1);                                           \
    }                                                                                              \
                                                                                                   \
    void type##BufferCLR(VM *virtualMech, type##Buffer *buffer)                                    \
    {                                                                                              \
        size_t olderSize = buffer->capacity * sizeof(buffer->datas[0]);                            \
        memoryManager(virtualMech, buffer->datas, olderSize, 0);                                   \
        type##BufferInit(buffer);                                                                  \
    }

DECLARE_BUFFER_TYPE(String)
#define SymTab StringBuffer
typedef uint8_t Byte;
typedef char Char;
typedef int Integer;
typedef uint16_t Word;
DECLARE_BUFFER_TYPE(Integer)
DECLARE_BUFFER_TYPE(Char)
DECLARE_BUFFER_TYPE(Byte)
DECLARE_BUFFER_TYPE(Word)

typedef enum
{
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
    ERROR_RUNTIME
} ErrType;

void errRepo(void *parser, ErrType errType, const char *format, ...);

void symTabCLR(VM *, SymTab *buffer);

#define IO_ERROR(...) errRepo(NULL, ERROR_IO, __VA_ARGS__)

#define MEM_ERROR(...) errRepo(NULL, ERROR_MEM, __VA_ARGS__)

#define LEX_ERROR(parser, ...) errRepo(parser, ERROR_LEX, __VA_ARGS__)

#define COMPILE_ERROR(parser, ...) errRepo(parser, ERROR_COMPILE, __VA_ARGS__)

#define RUN_ERROR(...) errRepo(NULL, ERROR_RUNTIME, __VA_ARGS__)

#define DEFAULT_BUfFER_SIZE 512

#endif