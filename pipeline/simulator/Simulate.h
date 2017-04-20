#ifndef SIMULATE_H
#define SIMULATE_H

#include "Env.h"
#include "Instruction.h"

bool checkStalled_lw(Env *env,int rs,int id);
bool checkForward(Env *env,int rs, int id);
int ADD(int a,int b,Env *env);
void MEM(Env *env);
void EXE(Env *env);
void ID(Env *env);
void IF(Env *env);
void run(Env *env);

#endif /* end of include guard: SIMULATE_H */
