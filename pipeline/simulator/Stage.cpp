#include "Stage.h"

Stage::Stage(){}

Stage::Stage(int name){
    this->name = name;
    isRs = isRt = false;
}

void Stage::set(string instName, Status s){
    this->instName = instName;
    status = s;
}

void Stage::clearStatus(){
    isRs = isRt = false;
    status = Normal;
}

void Stage::setforwardRs(string s){
    frs = s;
}

void Stage::setforwardRt(string s){
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
