#include "Instruction.h"
#include <map>
#include <iostream>

// int ADD(int a,int b,Env *env){
//     int c = a+b;
//     if(a<0 && b<0 && c>=0) env->err.addError(NumOverF);
//     if(a>0 && b>0 && c<=0) env->err.addError(NumOverF);
//
//     return c;
// }
int getCode(int code,int a,int b){  // get a to b
    int res = 0;
    for(int i=a, j = 0; i<=b; i++,j++) if((code>>i)&1){
        res += (1<<j);
    }
    return res;
}
Instruction::Instruction(){

}
/*                  init Instruction::Totype (for decode)                */
std::map<int,InstType> Instruction::buildMap(){
    std::map<int,InstType> Totype;
/* R-type */
    Totype[0x00] = R_Type;

/* I-type */
    Totype[0x08] = Totype[0x09] =  Totype[0x23] = Totype[0x21] = I_Type;
    Totype[0x25] = Totype[0x20] =  Totype[0x24] = Totype[0x2B] = I_Type;
    Totype[0x29] = Totype[0x28] =  Totype[0x0F] = Totype[0x0C] = I_Type;
    Totype[0x0D] = Totype[0x0E] =  Totype[0x0A] = Totype[0x04] = I_Type;
    Totype[0x05] = Totype[0x07] =  I_Type;

/* J-type*/
    Totype[0x02] = Totype[0x03] = J_Type;

    return Totype;
}
std::map<int,InstType> Instruction::Totype = Instruction::buildMap();

void Instruction::getname(){
    if(opcode==0){
        if(funct==0x20) name = "ADD";
        if(funct==0x21) name = "ADDU";
        if(funct==0x22) name = "SUB";
        if(funct==0x24) name = "AND";
        if(funct==0x25) name = "OR";
        if(funct==0x26) name = "XOR";
        if(funct==0x27) name = "NOR";
        if(funct==0x28) name = "NAND";
        if(funct==0x2A) name = "SLT";
        if(funct==0x00) name = "SLL";
        if(funct==0x02) name = "SRL";
        if(funct==0x03) name = "SRA";
        if(funct==0x08) name = "JR";
        if(funct==0x18) name = "MULT";
        if(funct==0x19) name = "MULTU";
        if(funct==0x10) name = "MFHI";
        if(funct==0x12) name = "MFLO";
    }
    else{
        if(opcode==0x08) name = "ADDI";
        if(opcode==0x09) name = "ADDIU";
        if(opcode==0x23) name = "LW";
        if(opcode==0x21) name = "LH";
        if(opcode==0x25) name = "LHU";
        if(opcode==0x20) name = "LB";
        if(opcode==0x24) name = "LBU";
        if(opcode==0x2B) name = "SW";
        if(opcode==0x29) name = "SH";
        if(opcode==0x28) name = "SB";
        if(opcode==0x0F) name = "LUI";
        if(opcode==0x0C) name = "ANDI";
        if(opcode==0x0D) name = "ORI";
        if(opcode==0x0E) name = "NORI";
        if(opcode==0x0A) name = "SLTI";
        if(opcode==0x04) name = "BEQ";
        if(opcode==0x05) name = "BNE";
        if(opcode==0x07) name = "BGTZ";

        if(opcode==0x02) name = "J";
        if(opcode==0x03) name = "JAL";
        if(opcode==0x3F) name = "HALT";

    }
}
Instruction* Instruction::decode(int code){
    Instruction* inst = new Instruction();

    inst->code    = code;
    inst->opcode  = getCode(code, 26, 31);
    inst->type    = Instruction::Totype[ inst->opcode ];
    inst->rs      = getCode(code, 21, 25);
    inst->rt      = getCode(code, 16, 20);
    inst->rd      = getCode(code, 11, 15);
    inst->C_shamt = getCode(code,  6, 10);
    inst->C       = code<<16>>16; // signed-extend
    inst->C_addr  = getCode(code, 0, 25);
    inst->funct   = getCode(code, 0, 5);

    inst->getname();

    // control signal
    if(inst->type == R_Type){
        inst->WB = true;
        inst->MEM = false;
    }
    else if(inst->type == I_Type){

        int opcode = inst->opcode;
        if(opcode==0x23 || opcode==0x21 || opcode==0x25 || opcode==0x20 ||
           opcode==0x24 || opcode==0x2B || opcode==0x29 || opcode==0x28)
           inst->MEM = true;
        else inst->MEM = false;

        if(opcode==0x2B || opcode==0x29 || opcode==0x28 || opcode==0x04 ||
            opcode==0x05 || opcode==0x07)
            inst->WB = false;
        else inst->WB = true;
    }

    return inst;
}
/*          R-type          */

/*
void Inst_R::run(Env *env){
    if(funct==0x20){ // add
        if(rd==0) env->err.addError(WriteTo0);
        int c = ADD(env->reg[rs], env->reg[rt], env);
        env->setReg(rd, c);
    }
    else if(funct==0x21){ //addu
        if(rd==0) env->err.addError(WriteTo0);
        int a = env->reg[rs], b = env->reg[rt], c= a+b;
        env->setReg(rd, c);
    }
    else if(funct==0x22){ //sub
        if(rd==0) env->err.addError(WriteTo0);
        int c = ADD(env->reg[rs], -env->reg[rt], env);
        env->setReg(rd, c);
    }
    else if(funct==0x24){ // and
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, env->reg[rs]&env->reg[rt]);
    }
    else if(funct==0x25){ // or
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, env->reg[rs]|env->reg[rt]);
    }
    else if(funct==0x26){ // xor
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, env->reg[rs]^env->reg[rt]);
    }
    else if(funct==0x27){ // nor
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, ~(env->reg[rs] | env->reg[rt]) );
    }
    else if(funct==0x28){ // nand
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, ~(env->reg[rs] & env->reg[rt]) );
    }
    else if(funct==0x2A){ // slt
        if(rd==0) env->err.addError(WriteTo0);
        bool v = env->reg[rs] < env->reg[rt];
        env->setReg(rd, v);
    }
    else if(funct==0x00){ // sll
        if( !(rd==0 && rt==0 && C==0) && rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, env->reg[rt]<<C);
    }
    else if(funct==0x02){ // srl
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, (unsigned)env->reg[rt]>>C);
    }
    else if(funct==0x03){ // sra
        if(rd==0) env->err.addError(WriteTo0);
        env->setReg(rd, env->reg[rt]>>C);
    }
    else if(funct==0x08){ // jr
        env->PC =  env->reg[rs];
    }
    else if(funct==0x18){ // muti
        if(!env->hadgethi)  env->err.addError(OverWriteHI);
        env->hadgethi = false;

        long long ans = (long long)env->reg[rs] *
                        (long long)env->reg[rt];
        int hi = ans>>32,  lo = ans & 0x00000000FFFFFFFF;
        env->setReg(32, hi);
        env->setReg(33, lo);
    }
    else if(funct==0x19){ //multu
        if(!env->hadgethi)  env->err.addError(OverWriteHI);
        env->hadgethi = false;

        unsigned long long ans = (unsigned long long)(unsigned)env->reg[rs] *
                        (unsigned long long)(unsigned)env->reg[rt];
        int hi = ans>>32,  lo = ans & 0x00000000FFFFFFFF;
        env->setReg(32, hi);
        env->setReg(33, lo);

    }
    else if(funct==0x10){ //mfhi
        env->hadgethi = true;
        if(rd==0) env->err.addError(WriteTo0);
        else env->setReg(rd, env->reg[32]);
    }
    else if(funct==0x12){ // mflo
        env->hadgethi = true;
        if(rd==0) env->err.addError(WriteTo0);
        else env->setReg(rd, env->reg[33]);
    }
    else throw Error("Unknown instruction");
}
*/

void Instruction::print(){
    if(type==R_Type)
        printf("R_type : %d %d %d %d %d\n", rs, rt, rd, C, funct);
    else if(type==I_Type)
        printf("I_type : %d %d %d %d\n", opcode, rs, rt, C);
    else if(type==J_Type)
        printf("J_type : %d %d\n", opcode, C);
    else
        printf("S_type : %d\n", opcode);

}

/*          I-type          */

// void Inst_I::run(Env *env){
//
//     if(opcode==0x08){ // addi
//         if(rt==0) env->err.addError(WriteTo0);
//         int c = ADD(env->reg[rs], C, env);
//
//         env->setReg(rt, c);
//     }
//     else if(opcode==0x09){ // addiu
//         if(rt==0) env->err.addError(WriteTo0);
//         else env->setReg(rt, env->reg[rs] + C);
//     }
//     else if(opcode==0x23){ // lw
//         if(rt==0) env->err.addError(WriteTo0);
//
//         unsigned int address = ADD(env->reg[rs],C,env);
//         if(address>1020) env->err.addError(MemAddOverF);
//         if(address%4!=0) env->err.addError(DataMisaligned);
//         if(env->err.halt) return;
//
//         env->setReg(rt, env->memory[address/4] );
//     }
//     else if(opcode==0x21){ // lh
//         if(rt==0)  env->err.addError(WriteTo0);
//
//         unsigned int address = ADD(env->reg[rs],C,env);
//         if(address>1022) env->err.addError(MemAddOverF);
//         if(address%2!=0) env->err.addError(DataMisaligned);
//         if(env->err.halt) return;
//
//         int value = env->memory[ address/4 ];
//         if(address%4==0) value = value>>16; // front
//         else value = value << 16 >> 16;
//         env->setReg(rt, value );
//     }
//     else if(opcode==0x25){ // lhu
//         if(rt==0) env->err.addError(WriteTo0);
//
//         unsigned int address = ADD(env->reg[rs],C,env);
//         if(address>1022) env->err.addError(MemAddOverF);
//         if(address%2!=0) env->err.addError(DataMisaligned);
//         if(env->err.halt) return;
//
//         int value = env->memory[ address/4 ];
//         if(address%4==0) value = (unsigned)value>>16; // front
//         else value = (unsigned)value << 16 >> 16;
//         env->setReg(rt, value );
//     }
//     else if(opcode==0x20){ //lb
//         if(rt==0) env->err.addError(WriteTo0);
//
//         unsigned int address = ADD(env->reg[rs],  C, env);
//         if(address>1023) env->err.addError(MemAddOverF);
//         if(env->err.halt) return;
//
//         int value = env->memory[address/4];
//         if(address%4==0) value = value>>24;
//         else if(address%4==1) value = value<<8>>24;
//         else if(address%4==2) value = value<<16>>24;
//         else value = value<<24>>24;
//
//         env->setReg(rt, value);
//     }
//     else if(opcode==0x24){ //lbu
//         if(rt==0) env->err.addError(WriteTo0);
//
//         unsigned int address = ADD(env->reg[rs] , C ,env);
//         if(address>1023) env->err.addError(MemAddOverF);
//         if(env->err.halt) return;
//
//         int value = env->memory[address/4];
//         if(address%4==0) value = (unsigned)value>>24;
//         else if(address%4==1) value = (unsigned)value<<8>>24;
//         else if(address%4==2) value = (unsigned)value<<16>>24;
//         else value = (unsigned)value<<24>>24;
//
//         env->setReg(rt, value);
//     }
//     else if(opcode==0x2B){ //sw
//         unsigned int address = ADD(env->reg[rs],C,env);
//         if(address>1020) env->err.addError(MemAddOverF);
//         if(address%4!=0) env->err.addError(DataMisaligned);
//         if(env->err.halt) return;
//
//         unsigned int value = env->reg[rt];
//         env->memory[address/4] = value;
//     }
//     else if(opcode==0x29){ //sh
//         unsigned int address = ADD(env->reg[rs],C, env);
//         if(address>1022) env->err.addError(MemAddOverF);
//         if(address%2!=0) env->err.addError(DataMisaligned);
//         if(env->err.halt) return;
//
//         unsigned int value = env->reg[rt] & 0x0000FFFF;
//         unsigned int now = env->memory[address/4];
//         if(address%4==0){
//             now = now<<16>>16;
//             now |= value<<16;
//         }
//         else{
//             now = now>>16<<16;
//             now |= value;
//         }
//         env->memory[address/4] = now;
//     }
//     else if(opcode==0x28){ //sb
//         unsigned int address = ADD(env->reg[rs],C, env);
//         if(address>1023) env->err.addError(MemAddOverF);
//         if(env->err.halt) return;
//
//         unsigned int value = (unsigned) env->reg[rt] & 0x000000FF;
//         unsigned int now = env->memory[address/4];
//         int index = address%4, start = (4-index-1)*8;
//         for(int i=0; i<8; i++, start++){
//             if((value>>i)&1 && !((now>>start)&1)) now |= (1<<(start));
//             else if( !((value>>i)&1) && (now>>start)&1) now ^= (1<<(start));
//         }
//         env->memory[address/4] = now;
//     }
//     else if(opcode==0x0F){ //lui
//         if(rt==0) env->err.addError(WriteTo0);
//         int result = C<<16;
//         env->setReg(rt, result);
//     }
//     else if(opcode==0x0C){ //andi
//         if(rt==0) env->err.addError(WriteTo0);
//         int cc = (unsigned) C<<16>>16;
//         int result = env->reg[rs]&cc;
//         env->setReg(rt, result);
//     }
//     else if(opcode==0x0D){ //ori
//         if(rt==0) env->err.addError(WriteTo0);
//         int cc = (unsigned) C<<16>>16;
//         int result = env->reg[rs]|cc;
//         env->setReg(rt, result);
//     }
//     else if(opcode==0x0E){ //nori
//         if(rt==0) env->err.addError(WriteTo0);
//         int cc = (unsigned) C<<16>>16;
//         int result = ~(env->reg[rs]|cc);
//         env->setReg(rt, result);
//     }
//     else if(opcode==0x0A){ //slti
//         if(rt==0) env->err.addError(WriteTo0);
//         int result = env->reg[rs] < C;
//         env->setReg(rt, result);
//     }
//     else if(opcode==0x04){ //beq
//         if(env->reg[rs]==env->reg[rt]){
//             env->PC = ADD(env->PC, C*4, env);
//         }
//     }
//     else if(opcode==0x05){ //bne
//         if(env->reg[rs]!=env->reg[rt]){
//             env->PC = ADD(env->PC, C*4, env);
//         }
//     }
//     else if(opcode==0x07){ //bgtz
//         if(env->reg[rs]>0){
//             env->PC = ADD(env->PC, C*4, env);
//         }
//     }
//     else throw Error("Unknown instruction");
// }

/*          J-type          */

// void Inst_J::run(Env *env){
//     if(opcode==0x02){ // j
//         env->PC = C*4;
//     }
//     else if(opcode==0x03){ // jal
//         env->setReg(31, env->PC);
//         env->PC = C*4;
//     }
//     else throw Error("Unknown instruction");
// }
