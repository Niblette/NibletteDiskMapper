#include "filedata.h"

#include <QTreeWidgetItem>

FileData::FileData(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime, std::size_t iSize)
    : Data(iName, iBirthTime, iModifiedTime),
    _size(iSize)
{}
////////////////////////////////////////

std::size_t FileData::GetSize(){
    return _size;
}
////////////////////////////////////////

void FileData::SetSize(std::size_t iSize){
    _size = iSize;
}
////////////////////////////////////////

bool FileData::PopulateFromXML(const std::string& iXML, std::string& oErrMssg){
    std::string tmpStr = iXML; //<File name=test.txt birthTime=Jul 30 18:15:15 2026 modifiedTime=Jul 30 18:16:06 2026 size=10/>
    auto tmpPos = tmpStr.find("name=");
    if (tmpPos == std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"name=\" in \"" + iXML + "\"";
        return false;
    }
    tmpStr = tmpStr.substr(tmpPos + 5); //test.txt birthTime=Jul 30 18:15:15 2026 modifiedTime=Jul 30 18:16:06 2026 size=10/>

    tmpPos = tmpStr.find("birthTime=");
    if (tmpPos == std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"birthTime=\" in \"" + iXML + "\"";
        return false;
    }
    std::string name = tmpStr.substr(0, tmpPos - 1); //test.txt
    tmpStr = tmpStr.substr(tmpPos + 10); //Jul 30 18:15:15 2026 modifiedTime=Jul 30 18:16:06 2026 size=10/>
    tmpPos = tmpStr.find("modifiedTime=");
    if (tmpPos ==std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"modifiedTime=\" in \"" + iXML + "\"";
        return false;
    }
    std::string birthTime = tmpStr.substr(0, tmpPos - 1);//Jul 30 18:15:15 2026
    tmpStr = tmpStr.substr(tmpPos + 13); //Jul 30 18:16:06 2026 size=10/>
    tmpPos = tmpStr.find("size=");
    if (tmpPos == std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"size=\" in \"" + iXML + "\"";
        return false;
    }
    std::string modifiedTime = tmpStr.substr(0, tmpPos - 1);//Jul 30 18:16:06 2026
    std::string size = tmpStr.substr(tmpPos + 5);//10/>
    size.pop_back();//10/
    size.pop_back();//10

    SetName(name);
    SetBirthTime(birthTime);
    SetModifiedTime(modifiedTime);
    try {
        SetSize(std::stoull(size));
    }
    catch(...){
        oErrMssg += "FileData::PopulateFromXML failed to convert \"" + size + "\" into a number. From: \"" + iXML + "\"";
        return false;
    }
    return true;
}
////////////////////////////////////////

std::string FileData::PrintToXML(){
    return "<File name=" + GetName()
           + " birthTime=" + GetBirthTime()
           + " modifiedTime=" + GetModifiedTime()
           + " size=" + std::to_string(_size)
           + "/>";
}
////////////////////////////////////////

void FileData::FillTreeView(QTreeWidgetItem * oItem){
    oItem->setText(0, QString::fromStdString(GetName()));
    oItem->setText(1, QString::fromStdString(GetModifiedTime()));
    oItem->setText(2, "File");
    oItem->setText(3, QString::fromStdString(GetHumanReadableSize()));
}
////////////////////////////////////////

bool FileData::RemoveEmptyDirectories(){
    return false;
}
////////////////////////////////////////

std::string FileData::GetHumanReadableSize(){
    if (_size < sizes[0]){
        return std::to_string(_size) + prefixes[0];
    }
    for (std::size_t i = 1; i < sizes.size(); i++){
        if (_size < sizes[i]){
            std::string readableSize = std::to_string((float)_size / (float)sizes[i - 1]);
            auto periodPos = readableSize.find('.');
            if (periodPos != std::string::npos){
                readableSize = readableSize.substr(0, periodPos + 2);
            }
            return readableSize + prefixes[i];
        }
    }

    std::string readableSize = std::to_string((float)_size / (float)sizes.back());
    auto periodPos = readableSize.find('.');
    if (periodPos != std::string::npos){
        readableSize = readableSize.substr(0, periodPos + 2);
    }
    return readableSize + prefixes.back();
}
////////////////////////////////////////

