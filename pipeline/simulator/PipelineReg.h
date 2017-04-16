#ifndef PIPELINEREG_H
#define PIPELINEREG_H

#include "Error.h"
#include "Instruction.h"

class PipelineReg{
public:
    Instruction *inst;
    int ALUResult, MEMResult;
    bool nop;

    PipelineReg(); // will set NOP
    PipelineReg(Instruction* i);

    void flush();
    void getfrom(PipelineReg *reg);
    void getfrom(Instruction *);
};


#endif /* end of include guard: PIPELINEREG_H */
