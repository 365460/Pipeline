
#include "Simulate.h"

int ADD(int a,int b,Env *env){
    int c = a+b;
    if(a<0 && b<0 && c>=0) env->err.addError(NumOverF);
    if(a>0 && b>0 && c<=0) env->err.addError(NumOverF);

    return c;
}

void WB(Env *env){
    PipelineReg *preg = env->preg[MEM_WB];
    Instruction *inst = preg->inst;
    if(preg->nop){
        env->stage[SWB]->setNop();
        return;
    }
    if(inst->RegWrite == true){
        // load
        if(inst->MEMRead) env->setReg(inst->regDst, preg->MEMResult);
        // alu
        else env->setReg(inst->regDst, preg->ALUResult);
    }
    env->stage[SWB]->set(inst->name);
}

void MEM(Env *env){
    PipelineReg *preg = env->preg[EXE_MEM];
    Instruction *inst = preg->inst;

    if(preg->nop){
        env->stage[SMEM]->setNop();
        return;
    }
    else env->stage[SMEM]->set(inst->name);

    unsigned int address = preg->ALUResult;
    int opcode = inst->opcode;
    if(inst->MEMRead){
        if(opcode==0x23){ // lw
            if(address>1020) env->err.addError(MemAddOverF);
            if(address%4!=0) env->err.addError(DataMisaligned);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            preg->MEMResult = env->memory[address/4];
        }
        else if(opcode==0x21){ // lh
            if(address>1022) env->err.addError(MemAddOverF);
            if(address%2!=0) env->err.addError(DataMisaligned);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            int value = env->memory[ address/4 ];
            if(address%4==0) value = value>>16; // front
            else value = value << 16 >> 16;
            preg->MEMResult = value;
        }
        else if(opcode==0x25){ // lhu
            if(address>1022) env->err.addError(MemAddOverF);
            if(address%2!=0) env->err.addError(DataMisaligned);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            int value = env->memory[ address/4 ];
            if(address%4==0) value = (unsigned)value>>16; // front
            else value = (unsigned)value << 16 >> 16;
            preg->MEMResult = value;
        }
        else if(opcode==0x20){ //lb
            if(address>1023) env->err.addError(MemAddOverF);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            int value = env->memory[address/4];
            if(address%4==0) value = value>>24;
            else if(address%4==1) value = value<<8>>24;
            else if(address%4==2) value = value<<16>>24;
            else value = value<<24>>24;

            preg->MEMResult = value;
        }
        else if(opcode==0x24){ //lbu
            if(address>1023) env->err.addError(MemAddOverF);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            int value = env->memory[address/4];
            if(address%4==0) value = (unsigned)value>>24;
            else if(address%4==1) value = (unsigned)value<<8>>24;
            else if(address%4==2) value = (unsigned)value<<16>>24;
            else value = (unsigned)value<<24>>24;

            preg->MEMResult = value;
        }

    }
    else if(inst->MEMWrite){
        if(opcode==0x2B){ //sw
            if(address>1020) env->err.addError(MemAddOverF);
            if(address%4!=0) env->err.addError(DataMisaligned);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            unsigned int value = env->reg[inst->rt];
            env->memory[address/4] = value;
        }
        else if(opcode==0x29){ //sh
            if(address>1022) env->err.addError(MemAddOverF);
            if(address%2!=0) env->err.addError(DataMisaligned);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            unsigned int value = env->reg[inst->rt] & 0x0000FFFF;
            unsigned int now = env->memory[address/4];
            if(address%4==0){
                now = now<<16>>16;
                now |= value<<16;
            }
            else{
                now = now>>16<<16;
                now |= value;
            }
            env->memory[address/4] = now;
        }
        else if(opcode==0x28){ //sb
            if(address>1023) env->err.addError(MemAddOverF);
            if(env->err.halt){
                env->halt = true;
                return;
            }

            unsigned int value = (unsigned) env->reg[inst->rt] & 0x000000FF;
            unsigned int now = env->memory[address/4];
            int index = address%4, start = (4-index-1)*8;
            for(int i=0; i<8; i++, start++){
                if((value>>i)&1 && !((now>>start)&1)) now |= (1<<(start));
                else if( !((value>>i)&1) && (now>>start)&1) now ^= (1<<(start));
            }
            env->memory[address/4] = now;
        }

    }
}

void EXE(Env *env){
    PipelineReg *preg = env->preg[ID_EXE];
    Instruction *inst = preg->inst;

    if(preg->nop){
        env->stage[SEXE]->setNop();
        return;
    }
    else env->stage[SEXE]->set(inst->name);

    int opcode = inst->opcode, funct = inst->funct;
    int rs = env->reg[inst->rs], rt = env->reg[inst->rt];
    if(opcode==0x04 || opcode==0x05 || opcode==0x07 || opcode==0x02 ||
       opcode==0x03 || (opcode==0 && funct==0x08)){
        return;
    }

    // forward
    Instruction *i = env->preg[EXE_MEM]->nop? NULL:env->preg[EXE_MEM]->inst;
    Instruction *j = env->preg[MEM_WB]->nop?  NULL:env->preg[MEM_WB]->inst;
    if(inst->needRs && inst->rs!=0){
        if(env->preg[EXE_MEM]->nop==false&&i->RegWrite&&(i->regDst==inst->rs)){
           if(!i->MEMRead){
               rs = env->preg[ EXE_MEM ]->ALUResult;
               env->stage[SEXE]->setforwardRs("fwd_EX-DM_rs_$"+to_string(inst->rs));
            }
        }
        else if(env->preg[MEM_WB]->nop==false &&j->RegWrite && (j->regDst==inst->rs)){
            if(j->MEMRead) rs = env->preg[MEM_WB]->MEMResult;
            else rs = env->preg[MEM_WB]->ALUResult;
            env->stage[SEXE]->setforwardRs("fwd_DM-WB_rs_$"+to_string(inst->rs));
        }
        else rs = env->reg[inst->rs];

    }
    if(inst->needRt && inst->rt!=0){
        if( env->preg[EXE_MEM]->nop==false && i->RegWrite && (i->regDst == inst->rt)){
            if(!i->MEMRead){
                rt = env->preg[ EXE_MEM ]->ALUResult;
                env->stage[SEXE]->setforwardRt("fwd_EX-DM_rt_$"+to_string(inst->rt));
            }
        }
        else if(env->preg[MEM_WB]->nop==false && j->RegWrite && (j->regDst==inst->rt)){
            if(j->MEMRead) rt = env->preg[MEM_WB]->MEMResult;
            else rt = env->preg[MEM_WB]->ALUResult;
            env->stage[SEXE]->setforwardRt("fwd_DM-WB_rt_$"+to_string(inst->rt));
        }
        else rt = env->reg[inst->rt];
    }

    // cout << inst->name << "  rs = " << inst->rs << "  rt="<<inst->rt << " rd = " << inst->rd << endl;
// alu
    if(opcode==0){
        // add
        if(funct==0x20) preg->ALUResult = ADD(rs, rt, env);
        // addu
        if(funct==0x21) preg->ALUResult = rs + rt;
        // sub
        if(funct==0x22) preg->ALUResult = ADD(rs, -rt, env);
        // and
        if(funct==0x24) preg->ALUResult = rs & rt;
        // or
        if(funct==0x25) preg->ALUResult = rs | rt;
        // xor
        if(funct==0x26) preg->ALUResult = rs ^ rt;
        // nor
        if(funct==0x27) preg->ALUResult = ~(rs | rt);
        // nand
        if(funct==0x28) preg->ALUResult = ~(rs & rt);
        // slt
        if(funct==0x2A) preg->ALUResult = rs < rt;
        // sll
        if(funct==0x00) preg->ALUResult = rt << inst->C_R;
        // srl
        if(funct==0x02) preg->ALUResult = (unsigned)rt >> inst->C_R;
        // sra
        if(funct==0x03) preg->ALUResult = rt >> inst->C_R;
    // jr
        //if(funct==0x08) name = "JR";

        // mult
        if(funct==0x18){
            if(!env->hadgetHi)  env->err.addError(OverWriteHI);
            env->hadgetHi = false;
            long long ans = (long long)rs* (long long)rt;
            int hi = ans>>32,  lo = ans & 0x00000000FFFFFFFF;
            env->setReg(32, hi);
            env->setReg(33, lo);
        }
        // multu
        if(funct==0x19){
            if(!env->hadgetHi)  env->err.addError(OverWriteHI);
            env->hadgetHi = false;

            unsigned long long ans = (unsigned long long)(unsigned)rs *
                            (unsigned long long)(unsigned)rt;
            int hi = ans>>32,  lo = ans & 0x00000000FFFFFFFF;
            env->setReg(32, hi);
            env->setReg(33, lo);
        }
        // mfhi
        if(funct==0x10){
            env->hadgetHi = true;
            preg->ALUResult = env->reg[32];
        }
        // mflo
        if(funct==0x12){
            env->hadgetHi = true;
            preg->ALUResult = env->reg[33];
        }
    }
    else{ // I type

        int CC = (unsigned) inst->C_I<<16>>16;
        // addi
        if(opcode==0x08) preg->ALUResult = ADD(rs, inst->C_I, env);
        // addiu
        if(opcode==0x09) preg->ALUResult = rs + inst->C_I;
        // lw lh lhu lb lbu sw sh sb
        if(opcode==0x23 || opcode==0x25 || opcode==0x20 || opcode==0x21 ||
           opcode==0x24 || opcode==0x2B || opcode==0x29 || opcode==0x28)
            preg->ALUResult = ADD(rs, inst->C_I, env);
        // lui
        if(opcode==0x0F) preg->ALUResult = inst->C_I << 16;
        // andi
        if(opcode==0x0C) preg->ALUResult = rs&CC;
        // ori
        if(opcode==0x0D) preg->ALUResult = rs|CC;
        // nori
        if(opcode==0x0E) preg->ALUResult = ~(rs|CC);
        // slti
        if(opcode==0x0A) preg->ALUResult = rs < inst->C_I;
    }

}

bool checkStalled_lw(Env *env,int rs,int id){
    if(rs==0) return false;
    if(env->preg[id]->nop==true) return false;
    return env->preg[id]->inst->MEMRead && (env->preg[id]->inst->regDst==rs);
}

bool checkForward(Env *env,int rs, int id){
    if(rs==0) return false;
    if(env->preg[id]->nop==true) return false;
    return env->preg[id]->inst->RegWrite && (env->preg[id]->inst->MEMRead==false)
        && (env->preg[id]->inst->regDst==rs);
}

void ID(Env *env){
    PipelineReg *preg = env->preg[IF_ID];
    Instruction *inst = preg->inst;

    if(preg->nop){
        env->stage[SID]->setNop();
        return;
    }
    else if(inst->opcode==0 && inst->rt==0 && inst->rd==0 && inst->C_R==0){// sll $0, $0, 0
        env->stage[SID]->setNop();
        env->preg[IF_ID]->flush();
        return;
    }
    bool branch = false, stall = false;
    int s = env->reg[ inst->rs ], t = env->reg[ inst->rt ];

// stall
    if(inst->needRs && checkStalled_lw(env, inst->rs, ID_EXE)) stall = true;
    if(inst->needRt && checkStalled_lw(env, inst->rt, ID_EXE)) stall = true;

    // check EXE_MEM
    if(inst->opcode == 0x04 || inst->opcode == 0x05 || inst->opcode==0x07 || (inst->opcode==0 && inst->funct==0x08)){
        if(inst->needRs && checkStalled_lw(env, inst->rs, EXE_MEM)) stall = true;
        if(inst->needRt && checkStalled_lw(env, inst->rt, EXE_MEM)) stall = true;
    }

// control
    /* get rs*/
    if(inst->rs!=0 && (inst->opcode == 0x04 || inst->opcode == 0x05 || inst->opcode==0x07 ||
        (inst->opcode==0 && inst->funct==0x08))){

        // check stall
        if(env->preg[ID_EXE]->nop==false &&
           env->preg[ID_EXE]->inst->RegWrite &&
           (env->preg[ID_EXE]->inst->regDst==inst->rs))
            stall = true;

        // check forward
         if(checkForward(env, inst->rs, EXE_MEM)){
            env->stage[SID]->setforwardRs("fwd_EX-DM_rs_$"+to_string(inst->rs));
            s = env->preg[EXE_MEM]->ALUResult;
        }
        else s = env->reg[ inst->rs ];
    }

    /* get rt*/
    if(inst->rt!=0 && (inst->opcode == 0x04 || inst->opcode == 0x05) ){
        // check stall
        if(env->preg[ID_EXE]->nop==false &&
           env->preg[ID_EXE]->inst->RegWrite &&
           (env->preg[ID_EXE]->inst->regDst==inst->rt) )
            stall = true;

        if(checkForward(env, inst->rt, EXE_MEM)){
            env->stage[SID]->setforwardRs("fwd_EX-DM_rt_$"+to_string(inst->rt));
            t = env->preg[EXE_MEM]->ALUResult;
        }
        else t = env->reg[ inst->rt ];
    }

// result
    if(stall){
        env->stage[SID]->set(inst->name, Stalled);
        env->hasStalled = true;
    }
    else{
        if(inst->opcode==0x04 && s==t) branch = true;
        if(inst->opcode==0x05 && s!=t) branch = true;
        if(inst->opcode==0x07 && s>0)  branch = true;
        if(branch){
            env->hasFlushed = true;     // flush IF_ID
            env->PC_next = ADD(env->PC, inst->C_I*4,  env);
        }

        if(inst->opcode==0x02){// j
            env->hasFlushed = true;
            env->PC_next = inst->C_J*4;
        }
        if(inst->opcode==0x03){ // jal
            env->hasFlushed = true;
            env->PC_next = inst->C_J*4;
        }
        if(inst->opcode==0 && inst->funct==0x08){ // jr
            env->hasFlushed = true;
            env->PC_next = s;
        }

        // jal
        if(inst->opcode==0x03) preg->ALUResult = env->PC;

        env->stage[SID]->set(inst->name);
    }
}

void IF(Env *env){
    Instruction *inst;
    inst = env->im->fetch( env->PC );

    char buf[23];
    sprintf(buf,"0x%08X", inst->code);
    string instname(buf);
    if(env->hasFlushed) env->stage[SIF]->set(instname, Flushed);
    else if(env->hasStalled) env->stage[SIF]->set(instname, Stalled);
    else{
        env->stage[SIF]->set(instname);
        env->PC_next = ADD(env->PC, 4, env);
    }

    env->PC = env->PC_next;

    // change PipelineReg
    env->preg[MEM_WB]->getfrom(env->preg[EXE_MEM]);
    env->preg[EXE_MEM]->getfrom(env->preg[ID_EXE]);

    if(env->stage[SID]->status == Stalled) env->preg[ ID_EXE ]->flush();
    else  env->preg[ID_EXE]->getfrom(env->preg[IF_ID]);

    if(env->stage[SIF]->status==Flushed) env->preg[IF_ID]->flush();
    else if(env->stage[SIF]->status!=Stalled){
        env->preg[IF_ID]->getfrom(inst);
    }
}

void run(Env *env){
    for(int i=0; i<5; i++) env->stage[i]->clearStatus();
    env->hasFlushed = env->hasStalled = false;

    // check halt
    if(env->preg[MEM_WB]->nop==false && env->preg[MEM_WB]->inst->opcode==0x3F){
        Instruction *inst;
        inst = env->im->fetch(env->PC);
        if(!inst || inst->opcode!=0x3F) throw Error("illegl halt, not all stages fiiled with halt");

        for(int i=0; i<3; i++){
            if(env->preg[i]->nop || env->preg[i]->inst->opcode!=0x3F)
                throw Error("illegl halt, not all stages fiiled with halt");
        }
        env->halt = true;
    }

    env->printReport(0);
    fprintf(env->fresult, "PC: 0x%08X\n", env->PC);
    int debug = 0, pc = env->PC;
    WB(env);
if(debug)    printf("WB: finish\n");
    MEM(env);
if(debug)    printf("MEM: finish\n");
    EXE(env);
if(debug)    printf("Exe: finish\n");
    ID(env);
if(debug)    printf("ID: finish\n");
    IF(env);

    for(int i=0; i<5; i++){
        env->stage[i]->print(env->fresult);
    }

}
