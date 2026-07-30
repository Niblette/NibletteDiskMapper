#include "datastruct.h"

Data::Data(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime)
{
    SetName(iName);
    SetBirthTime(iBirthTime);
    SetModifiedTime(iModifiedTime);
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

const std::string& Data::GetBirthTime(){
    return _birthTime;
}
////////////////////////////////////////

void Data::SetBirthTime(const std::string& iBirthTime){
    _birthTime = iBirthTime;
}
////////////////////////////////////////

const std::string& Data::GetModifiedTime(){
    return _modifiedTime;
}
////////////////////////////////////////

void Data::SetModifiedTime(const std::string& iModifiedTime){
    _modifiedTime = iModifiedTime;
}
////////////////////////////////////////
