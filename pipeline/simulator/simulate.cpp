#include "Env.h"
#include "Instruction.h"

int ADD(int a,int b,Env *env){
    int c = a+b;
    if(a<0 && b<0 && c>=0) env->err.addError(NumOverF);
    if(a>0 && b>0 && c<=0) env->err.addError(NumOverF);

    return c;
}
void WB(Env *env){

    PipelineReg *preg = env->preg[MEM_WB];
    Instruction *inst = preg->inst;
    if(preg->nop) env->stage[SWB]->set(NULL, NOP);
    else if(inst->RegWrite == true){
        // load
        if(inst->MEMRead) env->setReg(inst->regDst, preg->MEMResult);
        // alu
        else env->setReg(inst->regDst, preg->ALUResult);

        env->stage[SWB]->set(inst->name);
    }
}

void mem(Env *env){
    PipelineReg *preg = env->preg[EXE_MEM];
    Instruction *inst = preg->inst;

    if(preg->nop){
        env->preg[MEM_WB]->flush();
        env->stage[SMEM]->set(NULL, NOP);
    }
    else{
        if(inst->MEMRead){
            env->preg[MEM_WB]->flush();

        }
        // else if(preg->)
    }

}

/*
    Noraml
    Stall:
    forward:
    if(stall and forward)
        --> stall --> ID/EXE=>flush
    Nop:
        branch sucessfully
        -->I

*/
bool checkStalled(Env *env,int rs,int id){
    return env->preg[id]->inst->MEMRead && (env->preg[id]->inst->regDst==rs);
}

bool checkForward(Env *env,int rs, int id){
    return env->preg[id]->inst->RegWrite && env->preg[id]->inst->MEMRead==false &&
          (env->preg[id]->inst->regDst==rs);
}
void ID(Env *env){
    PipelineReg *preg = env->preg[IF_ID];
    Instruction *inst = preg->inst;

    if(preg->nop){
        env->stage[SID]->set(NULL,NOP);
        return;
    }
    bool branch = false, stall = false;
    int s, t;

// stalled
    if(inst->needRs && !checkStalled(env, inst->rs, ID_EXE)) stall = true;
    if(inst->needRt && !checkStalled(env, inst->rt, ID_EXE)) stall = true;

    // check EXE_MEM
    if(inst->opcode == 0x04 || inst->opcode == 0x05 || inst->opcode==0x07){
        if(inst->needRs && !checkStalled(env, inst->rs, EXE_MEM)) stall = true;
        if(inst->needRt && !checkStalled(env, inst->rt, EXE_MEM)) stall = true;
    }

// control
    /* get rs*/
    if(inst->opcode == 0x04 || inst->opcode == 0x05 || inst->opcode==0x07){

        if(env->preg[ID_EXE]->inst->RegWrite && env->preg[ID_EXE]->inst->regDst==inst->rs)
            stall = true;

        // check forward
         if(checkForward(env, inst->rs, EXE_MEM)){
            env->stage[SID]->setforwardRs("fwd_DM_WB_rs_$"+to_string(inst->rs));
            s = env->preg[EXE_MEM]->ALUResult;
        }
        else s = env->reg[ inst->rs ];
    }

    /* get rt*/
    if(inst->opcode == 0x04 || inst->opcode == 0x05){
        if(env->preg[ID_EXE]->inst->RegWrite && env->preg[ID_EXE]->inst->regDst==inst->rt)
            stall = true;

        if(checkForward(env, inst->rt, EXE_MEM)){
            env->stage[SID]->setforwardRs("fwd_DM_WB_rt_$"+to_string(inst->rt));
            t = env->preg[EXE_MEM]->ALUResult;
        }
        else t = env->reg[ inst->rt ];
    }

// judge result
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
            env->PC = ADD(env->PC, inst->C*4, env);
        }
        env->stage[SID]->set(inst->name);
    }
}

void IF(Env *env){
    Instruction *inst;
    try{
        inst = env->im->fetch( env->PC );
    }
    catch(Error e){
        // cout << e.illegal << endl;
        //TODO interrupt the program
        // halt = true;
    }

    char buf[23];
    sprintf(buf,"0x%08X", inst->code);
    string s(buf);
    if(env->hasFlushed) env->stage[SIF]->set(s, Flushed);
    else if(env->hasStalled) env->stage[SIF]->set(s, Stalled);
    else env->stage[SIF]->set(s);


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
    WB(env);
}
