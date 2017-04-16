#include "PipelineReg.h"

PipelineReg::PipelineReg(){
    flush();
}

PipelineReg::PipelineReg(Instruction* i){
    nop = i==NULL;
    inst = i;
    ALUResult = MEMResult = 0;
}

void PipelineReg::flush(){
    nop = true;
    inst = NULL;
    ALUResult = MEMResult = 0;
}
