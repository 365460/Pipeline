#ifndef IMEMORY_H
#define IMEMORY_H

#include "Instruction.h"
#include "Error.h"
#include <cstdio>

class IMemory{
public:
    int start, end;
    Instruction *inst[300];

    IMemory();

    void loadInst(FILE *fp);
    Instruction* fetch(unsigned int pc);

    void print();
};

#endif
