#ifndef ENV_H
#define ENV_H

#include <cstdio>
#include <vector>
#include "Error.h"
#include "PipelineReg.h"
#include "Stage.h"
#include "IMemory.h"

enum StageName{
    SIF,
    SID,
    SEXE,
    SMEM,
    SWB
};

enum PregName{
    IF_ID,
    ID_EXE,
    EXE_MEM,
    MEM_WB
};

class Env
{
public:
    int memory[260];
    IMemory *im;

    /* register */
    int reg[35]; // 32: HI, 33: LO
    unsigned int PC;
    bool hadgetHi;


    /* Pipeline */
    PipelineReg *preg[4];
    Stage *stage[5];
    bool hasStalled;
    bool hasFlushed;

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
