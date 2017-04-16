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

void PipelineReg::getfrom(PipelineReg *reg){
    this->inst = reg->inst;
    this->nop = reg->nop;
    this->ALUResult = reg->ALUResult;
    this->MEMResult = reg->MEMResult;
}

void PipelineReg::getfrom(Instruction *inst){
    this->inst = inst;
    nop = inst==NULL;
    ALUResult = MEMResult = 0;
}
