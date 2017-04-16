#ifndef PIPELINEREG_H
#define PIPELINEREG_H

#include "Error.h"
#include "Instruction.h"

class PipelineReg{
public:
    Instruction *inst;
    int ALUResult, MEMResult;
    bool WBWrite, MEMread, MEMWrite, nop;

    PipelineReg(); // will set NOP
    PipelineReg(Instruction* i);

    void flush();
};


#endif /* end of include guard: PIPELINEREG_H */
