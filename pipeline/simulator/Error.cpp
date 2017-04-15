#include "Error.h"

Error::Error(){

}
Error::Error(std::string s):illegal(s), halt(true){

}
void Error::addError(int id){
    switch (id)
    {
    case WriteTo0:
        message.push_back("Write $0 Error");
        break;
    case NumOverF:
        message.push_back("Number Overflow");
        break;
    case OverWriteHI:
        message.push_back("Overwrite HI-LO registers");
        break;
    case MemAddOverF:
        message.push_back("Address Overflow");
        halt = true;
        break;
    case DataMisaligned:
        message.push_back("Misalignment Error");
        halt = true;
        break;
    default:    // just halt
        halt = true;
    }
}
