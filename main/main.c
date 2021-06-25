#include "main.h"
#include <stdio.h>
#include <string.h>
#include "../parser/parser.h"
#include "../VM/virtualmachine.h"
#include "../VM/core.h"
#include "../main/tokenlist"

static void runFile(const char *path)
{
    const char *lastSlash = strrchr(path, '/');
    if (lastSlash != NULL)
    {
        char *root = (char *)malloc(lastSlash - path + 2);
        memcpy(root, path, lastSlash - path + 1);
        root[lastSlash - path + 1] = '\0';
        rootDir = root;
    }
    VM *virtualMachine = newVirtualMach();
    const char *sourceCode = readFile(path);
    struct parser parser;
    initParser(virtualMachine, &parser, path, sourceCode);
    while (parser.currentToken.type != TOKEN_EOF)
    {
        getNextToken(&parser);
        printf("Line [\033[1;36m%d\033[0m] : \033[1;32m %s \033[0m[ \033[1m", parser.currentToken.lineNo, tokenArray[parser.currentToken.type]);
        uint32_t index = 0;
        while (index < parser.currentToken.length)
        {
            printf("%c", *(parser.currentToken.start + index++));
        }
        printf("\033[0m ]\n");
    }
}
int main(int argc, const char **argv)
{
    printf("\033[1;34m   /`         =@@@^ =@@@@@@@@@@@@@@^ =@@@^    /@@@@@ ,@@@@@@@@@@@@@@@             @@@@             @@@@@@@@@@@@@@@^ /`         =@@@^ /@@@@@@@@@@@@@@^\n");
    printf("\033[1;36m   @@@\\       @@@@` @@@@@@@@@@@@@@@^ @@@@  ,@@@@@/   /@@@@@@@@@@@@@@/            =@@@/            ,@@@@@@@@@@@@@@@  @@@\\       @@@@` @@@@@@@@@@@@@@@^\n");
    printf("\033[1;36m  =@@@@@@`   ,@@@@                  =@@@^/@@@@@`     @@@@       /@@@^            /@@@^            =@@@^      =@@@/ =@@@@@@`   ,@@@@ =@@@^\n");
    printf("\033[1;37m  @@@@@@@@@] =@@@^ @@@@@@@@@@@@@@@^ @@@@@@@@/`      =@@@^      ,@@@@             @@@@             @@@@       /@@@^ @@@@@@@@@] /@@@^ @@@@    =@@@@@@^\n");
    printf("\033[1;37m =@@@^,@@@@@@@@@@ =@@@@@@@@@@@@@@@ =@@@@@@@\\        @@@@       =@@@^            =@@@^            =@@@        @@@@ =@@@^,@@@@@@@@@@ =@@@/    @@@@@@@\n");
    printf("\033[1;36m @@@@`  ,\\@@@@@@^                  /@@@@@@@@@`     =@@@/       @@@@             @@@@             @@@@@@@@@@@@@@@^ @@@@`  ,\\@@@@@@^ @@@@`      =@@@^\n");
    printf("\033[1;36m,@@@@      \\@@@@ ,@@@@@@@@@@@@@@@` @@@@  \\@@@@\\    @@@@@@@@@@@@@@@^            =@@@@@@@@@@@@@@@ =@@@        @@@@ =@@@/      \\@@@@ ,@@@@@@@@@@@@@@@\n");
    printf("\033[1;34m/@@@^        ,@/ =@@@@@@@@@@@@@@/ =@@@^   ,@@@@@\\ ,@@@@@@@@@@@@@@@             @@@@@@@@@@@@@@@^ /@@@^      =@@@^ /@@@^        ,@/ =@@@@@@@@@@@@@@/\n");

    printf("\n\033[1;37m>>>NekoLang test branch v0.0.2 beta<<<\n\033[0m\n\n");
    if (argc == 1)
    {
        printf("\033[35mNOTHING TO DO (>_<;)!\033[0m\n");
    }
    else
    {
        runFile(argv[1]);
    }
    return 0;
}