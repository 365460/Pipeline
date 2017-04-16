#ifndef INSTRUCTION_h
#define INSTRUCTION_h

#include <cstdio>
#include <iostream>
#include <map>
using namespace std;

enum InstType{
    R_Type = 1,
    I_Type,
    J_Type,
    S_type
};

class Instruction{
public:
    static std::map<int,InstType> Totype;
    static std::map<int,InstType> buildMap();

    InstType type;
    string name;
    int code, opcode, rs, rt, rd, funct, C_shamt, C, C_addr;
    bool RegWrite, MEMRead, MEMWrite;

    static Instruction* decode(int);
    Instruction();
    void print();
    void getname();
};

// class Inst_R : public Instruction{
// public:
//     Inst_R();
//     Inst_R(int code);
//     void run(Env *env);
//     void print();
// };
//
// class Inst_I : public Instruction{
// public:
//     Inst_I();
//     Inst_I(int code);
//     void run(Env *env);
//     void print();
// };
//
// class Inst_J : public Instruction{
// public:
//     Inst_J();
//     Inst_J(int code);
//     void run(Env *env);
//     void print();
// };
//
// class Inst_S : public Instruction{
// public:
//     Inst_S();
//     Inst_S(int code);
//     void run(Env *env);
//     void print();
// };
//
#endif
