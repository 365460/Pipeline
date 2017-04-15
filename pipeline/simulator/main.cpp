#include <iostream>
#include <map>
#include <cstdio>
using namespace std;

#include "Error.h"
#include "Instruction.h"
#include "Env.h"
#include "IMemory.h"

int main()
{
    bool halt = false;

    IMemory *im = new IMemory();
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
        im->loadInst(finst);
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

    env->PC = im->start;
    env->fresult = fresult;
    env->printReport(0);

    for(int cycle=1; cycle<=500000 && !halt; cycle++){
        Instruction *nowInst = NULL;
        int nowAddress = env->PC;
        try
        {
            nowInst = im->fetch(nowAddress);
        }
        catch(Error e){
            cout << e.illegal << endl;
            halt = true;
            break;
        }

        env->PC += 4;
        env->err.message.clear();
        try{
            nowInst->run( env );
            if(env->err.message.size() == 0 && env->err.halt==false)
                env->printReport(cycle);
            else
            {
                if(env->err.message.size()==0) // jush halt
                    halt = true;
                else{
                    for(auto i: env->err.message)
                        fprintf(ferror, "In cycle %d: %s\n",cycle, i.c_str());
                }

                if(env->err.halt == true) halt = true;
                else env->printReport(cycle);
            }
        }
        catch(Error e){
            printf("illegal instruction found at 0x%X\n", nowAddress);
            halt = true;
        }
    }
    fclose( fresult );
    fclose( ferror );
    if(!halt) printf("illegal cycles, over 500,000 cycles\n");

    // int n;
    // while(cin>>n){}
    return 0;
}
