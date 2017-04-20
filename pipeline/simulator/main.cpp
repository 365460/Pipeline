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

    FILE *finst = fopen("../../archiTA/simulator/iimage.bin","rb");
    FILE *fdm =   fopen("../../archiTA/simulator/dimage.bin","rb");
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

        if(cycle==0) env->printReport(0);
        try{
            run(env);
        }catch(Error e){
            cout << e.illegal << endl;
            return 0;
        }
        fprintf(env->fresult, "\n\n");

        if(env->halt) break;
    }
    fclose( fresult );
    fclose( ferror );
    if(cycle>500000) printf("illegal cycles, over 500,000 cycles\n");

    // for(int cycle=1; cycle<=500000 && !halt; cycle++){
    //     Instruction *nowInst = NULL;
    //     int nowAddress = env->PC;
    //     try
    //     {
    //         // nowInst = im->fetch(nowAddress);
    //     }
    //     catch(Error e){
    //         cout << e.illegal << endl;
    //         halt = true;
    //         break;
    //     }
    //
    //     env->PC += 4;
    //     env->err.message.clear();
    //     try{
    //         // nowInst->run( env );
    //         if(env->err.message.size() == 0 && env->err.halt==false)
    //             env->printReport(cycle);
    //         else
    //         {
    //             if(env->err.message.size()==0) // jush halt
    //                 halt = true;
    //             else{
    //                 for(auto i: env->err.message)
    //                     fprintf(ferror, "In cycle %d: %s\n",cycle, i.c_str());
    //             }
    //
    //             if(env->err.halt == true) halt = true;
    //             else env->printReport(cycle);
    //         }
    //     }
    //     catch(Error e){
    //         printf("illegal instruction found at 0x%X\n", nowAddress);
    //         halt = true;
    //     }
    // }

    // int n;
    // while(cin>>n){}
    return 0;
}
