#include "IMemory.h"

IMemory::IMemory(){
    for(int i=0; i<300; i++) inst[i] = decode(0);
}

void IMemory::loadInst(FILE *fp){
    char s[4];
    int T = 0, nowaddress = 0;
    while(fscanf(fp,"%c",&s[0])==1){
            for(int i=1; i<4; i++) fscanf(fp,"%c",&s[i]);
            int code = 0;
        int base = 24;
        for(int j=0; j<4; j++, base -= 8){
            int t2 = (unsigned)s[j]<<24>>24;
            code |= (t2<<base);
        }
        if(T==0){
            if(code%4 != 0)
                throw Error("illegal, I Memory address missaline");
            start = nowaddress = code;
        }
        else if(T==1) end = start+(code-1)*4;
        else{
            // printf("%d: 0x%08X\n",nowaddress, code);
            if(nowaddress>1020)
                throw Error("illegal, I-M loaded address is over 1024");
            inst[nowaddress/4] = decode(code);
            // inst[nowaddress/4]->print();
            nowaddress += 4;
        }
        T++;
    }
}

Instruction* IMemory::fetch(unsigned int pc){
    if(pc%4 != 0)
        throw Error("illegal, fetch I-Memroy using missaline");

    if(pc>1020)
        throw Error("illegal, loaded address is over 1k");
    return inst[ pc/4 ];
}

void IMemory::print(){
    printf("start  = %d, end = %d\n", start, end);

    printf("There are %d instructions.\n",end-start+1);

    for(int i=start; i<=end; i+=4){
        inst[i/4]->print();
        printf("0x%08X\n\n",inst[i/4]->code);
    }
}
