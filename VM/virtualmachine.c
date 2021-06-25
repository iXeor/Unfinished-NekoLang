#include <virtualmachine.h>
#include <stdlib.h>
#include <utils.h>

//初始化虚拟机
void initVirtualMach(VM *virtualMach)
{
    virtualMach->allocatedBytes = 0;
    virtualMach->currentParser = NULL;
}

VM *newVirtualMach()
{
    VM *virtualMach = (VM *)malloc(sizeof(VM));
    if (virtualMach == NULL)
    {
        MEM_ERROR("Allocate Virtual Machine failed!");
    }
    initVirtualMach(virtualMach);
    return virtualMach;
}