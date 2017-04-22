#include <iostream>
#include <map>
#include <cstdio>
using namespace std;

#include "Error.h"
#include "Instruction.h"
#include "Env.h"
#include "IMemory.h"
#include "Simulate.h"

int main()
{
    Env* env = new Env();

    // FILE *fp = fopen("iimage.bin","r");
    FILE *ferror =  fopen("error_dump.rpt", "w");
    FILE *fresult = fopen("snapshot.rpt", "w");

    FILE *finst = fopen("iimage.bin","rb");
    FILE *fdm =   fopen("dimage.bin","rb");
    if(finst==NULL){
        cout << "no iimage.bin!";
        return 0;
    }
    if(fdm==NULL){
        cout << "no dimage.bin!";
        return 0;
    }

    try
    {
        env->im->loadInst(finst);
    }
    catch(Error e)
    {
        cout << e.illegal << endl;
        return 0;
    }
    fclose(finst);

    try
    {
        env->loadMem(fdm); // load memory
    }
    catch(Error e){
        cout << e.illegal << endl;
        return 0;
    }
    fclose(fdm);

    env->PC = env->im->start;
    env->fresult = fresult;
    // env->im->print();
    int cycle=0;
    for(cycle=0; cycle<=500000; cycle++){
        fprintf(env->fresult,"cycle %d\n",cycle);

        for(auto i: env->err.message)
            fprintf(ferror, "In cycle %d: %s\n",cycle, i.c_str());
        env->err.message.clear();
        try{
            run(env);
        }catch(Error e){
            cout << e.illegal << endl;
            return 0;
        }
        fprintf(env->fresult, "\n\n");

        if(env->halt) break;
    }
    for(auto i: env->err.message)
        fprintf(ferror, "In cycle %d: %s\n",cycle+1, i.c_str());
    fclose( fresult );
    fclose( ferror );
    if(cycle>500000) printf("illegal cycles, over 500,000 cycles\n");
    return 0;
}
