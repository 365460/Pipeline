#ifndef STAGE_H
#define STAGE_H

#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

enum Status {
    Normal,
    Stalled,
    Flushed,
    Fwd,
    NOP
};

class Stage{
public:
    string name, instName;
    Status status;
    bool isRs, isRt;
    string frs, frt;

    Stage();
    Stage(string name);

    void clearStatus();
    void set(string inst,Status st=Normal);
    void setforwardRs(string s);
    void setforwardRt(string s);

    void print();
};


#endif /* end of include guard: STAGE_H */