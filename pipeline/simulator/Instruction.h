#ifndef INSTRUCTION_h
#define INSTRUCTION_h

#include "Env.h"
#include <cstdio>
#include <map>

enum InstType {
    R_Type = 1,
    I_Type,
    J_Type,
    S_type
};

class Instruction{
public:
    static std::map<int,int> Totype;
    static std::map<int,int> buildMap();
    int code;
    virtual void run(Env *env) = 0;
    virtual void print() = 0;
};

class Inst_R : public Instruction{
public:
    int opcode, rs, rt, rd, C, funct;
    Inst_R();
    Inst_R(int code);
    void run(Env *env);
    void print();
};

class Inst_I : public Instruction{
public:
    int opcode, rs, rt, C;
    Inst_I();
    Inst_I(int code);
    void run(Env *env);
    void print();
};

class Inst_J : public Instruction{
public:
    int opcode, C;
    Inst_J();
    Inst_J(int code);
    void run(Env *env);
    void print();
};

class Inst_S : public Instruction{
public:
    int opcode;
    Inst_S();
    Inst_S(int code);
    void run(Env *env);
    void print();
};

Instruction* decode(int code);
#endif
