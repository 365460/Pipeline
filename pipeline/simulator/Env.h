#ifndef ENV_H
#define ENV_H

#include <cstdio>
#include <vector>
#include "Error.h"

class Env
{
public:
    int memory[260];

    /* register */
    int reg[35]; // 32: HI, 33: LO
    unsigned int PC;
    bool hadgethi;

    /*  snapshot.rpt */
    FILE *fresult;

    std::vector<int> report;
    Error err;

    Env();

    void loadMem(FILE *fp);
    void setReg(int address,int code);
    void printReport(int cycle);
    void printPC();
};
#endif
