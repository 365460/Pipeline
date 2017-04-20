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
    int code, opcode, rs, rt, rd, funct, C_R, C_I, C_J;
    int regDst; // for regWrite
    bool RegWrite, MEMRead, MEMWrite;
    bool needRs, needRt;

    static Instruction* decode(int);
    Instruction();
    void print();
    void getname();
};
#endif
