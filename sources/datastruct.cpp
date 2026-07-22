#include "datastruct.h"

Data::Data(const std::string& iName)
{
    SetName(iName);
}
////////////////////////////////////////

const std::string& Data::GetName(){
    return _name;
}
////////////////////////////////////////

void Data::SetName(const std::string& iName){
    char sep1 = '\\';
    char sep2 = '/';
    std::string tmpStr = iName;

    auto pos = tmpStr.find_last_of(sep1);
    tmpStr = tmpStr.substr(pos + 1);

    pos = tmpStr.find_last_of(sep2);
    _name = tmpStr.substr(pos + 1);
}
////////////////////////////////////////
