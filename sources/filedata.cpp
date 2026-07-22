#include "filedata.h"

#include <iostream>

#include <QTreeWidgetItem>

FileData::FileData(const std::string& iName, int iSize)
    : Data(iName),
    _size(iSize)
{}
////////////////////////////////////////

int FileData::GetSize(){
    return _size;
}
////////////////////////////////////////

void FileData::SetSize(int iSize){
    _size = iSize;
}
////////////////////////////////////////

bool FileData::PopulateFromXML(const std::string& iXML, std::string& oErrMssg){
    std::string tmpStr = iXML;
    auto namePos = tmpStr.find("name=");
    if (namePos == std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"names=\" in \"" + iXML + "\"";
        return false;
    }
    tmpStr = tmpStr.substr(namePos + 5);
    auto sizePos = tmpStr.find("size=");
    if (sizePos == std::string::npos){
        oErrMssg += "FileData::PopulateFromXML failed to find \"size=\" in \"" + iXML + "\"";
        return false;
    }
    std::string name = tmpStr.substr(0, sizePos);
    std::string size = tmpStr.substr(sizePos + 5);
    size.pop_back();
    size.pop_back();

    SetName(name);
    try {
        SetSize(std::stoi(size));
    }
    catch(...){
        oErrMssg += "FileData::PopulateFromXML failed to convert \"" + size + "\" into a number. From: \"" + iXML + "\"";
        return false;
    }
    return true;
}
////////////////////////////////////////

std::string FileData::PrintToXML(){
    return "<File name=" + GetName() + " size=" + std::to_string(_size) + "/>";
}
////////////////////////////////////////

void FileData::FillTreeView(QTreeWidgetItem * oItem){
    oItem->setText(0, QString::fromStdString(GetName()));
    oItem->setText(1, QString::fromStdString(std::to_string(GetSize())));
    oItem->setText(2, "File");
}
////////////////////////////////////////

bool FileData::RemoveEmptyDirectories(){
    return false;
}
////////////////////////////////////////

//void FileData::print(const std::string& iSpacer){
//    std::cout << iSpacer << GetName() << std::endl;
//}
////////////////////////////////////////




