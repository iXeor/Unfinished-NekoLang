#ifndef _VM_VIRTUALMACHINE_H
#define _VM_VIRTUALMACHINE_H
#include "../include/common.h"

struct virtualMachine
{
    uint32_t allocatedBytes; //累计已分配的内存量
    Parser *currentParser;   //当前词法分析器
};

void initVirtualMach(VM *virtualMach);
VM *newVirtualMach(void);

#endif