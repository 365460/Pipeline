#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <vector>

enum ErrorType{
    WriteTo0,
    NumOverF,
    OverWriteHI,
    MemAddOverF,
    DataMisaligned,
    Halt
};
class Error{

public:
    std::vector<std::string> message;
    std::string illegal;
    int halt;
    Error();
    Error(std::string);

    void addError(int id);
};
#endif
