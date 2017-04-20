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
    this->instName = inst;
    if(s==Stalled) status = s;
    else if(isRt || isRs) status = Fwd;
    else status = s;
}

void Stage::clearStatus(){
    isRs = isRt = false;
    status = Normal;
}

void Stage::setNop(){
    isRs = isRt = false;
    status = NOP;
}

void Stage::setforwardRs(string s){
    status = Fwd;
    isRs = true;
    frs = s;
}

void Stage::setforwardRt(string s){
    status = Fwd;
    isRt = true;
    frt = s;
}

void Stage::print(FILE* fp){
    if(status==NOP) fprintf(fp,"%s: NOP\n",name.c_str());
    else{
        fprintf(fp,"%s: %s",name.c_str(), instName.c_str());
        switch (status) {
            case Stalled:
                fprintf(fp," to_be_stalled\n");
                break;
            case Flushed:
                fprintf(fp," to_be_flushed\n");
                break;
            case Fwd:
                if(isRs) fprintf(fp," %s",frs.c_str());
                if(isRt) fprintf(fp," %s",frt.c_str());
                fprintf(fp,"\n");
                break;
            case Normal:
                fprintf(fp,"\n");
                break;
        }
    }
}
