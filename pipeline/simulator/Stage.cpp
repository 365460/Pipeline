#include "Stage.h"

Stage::Stage()
{
    isRs = isRt = false;
    status = NOP;
}

Stage::Stage(string name){
    this->name = name;
    isRs = isRt = false;
    status = NOP;
}

void Stage::set(string inst, Status s){
    clearStatus();
    this->instName = inst;
    status = s;
}

void Stage::clearStatus(){
    isRs = isRt = false;
    status = Normal;
}

void Stage::setforwardRs(string s){
    status = Fwd;
    frs = s;
}

void Stage::setforwardRt(string s){
    status = Fwd;
    frt = s;
}

void Stage::print(){
    if(status==NOP) printf("%s: NOP\n",name.c_str());
    else{
        printf("%s: %s",name.c_str(), instName.c_str());
        switch (status) {
            case Stalled:
                printf(" to_be_stalled\n");
                break;
            case Flushed:
                printf(" to_be_flushed\n");
                break;
            case Fwd:
                if(isRs) printf(" %s",frs.c_str());
                if(isRt) printf(" %s",frt.c_str());
                printf("\n");
                break;
            case Normal:
                printf("\n");
                break;
        }
    }
}
