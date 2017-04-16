#include "PipelineReg.h"

PipelineReg::PipelineReg(){
    flush();
}

PipelineReg::PipelineReg(Instruction* i){
    nop = i==NULL;
    inst = i;
    ALUResult = MEMResult = 0;
    WBWrite = MEMread = MEMWrite = false;
}

void PipelineReg::flush(){
    nop = true;
    WBWrite = MEMread = MEMWrite = false;
    inst = NULL;
}
