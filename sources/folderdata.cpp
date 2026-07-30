#include "folderdata.h"
#include "filedata.h"

#include <QTreeWidgetItem>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QDateTime>
#include <QStringList>

#include <iostream>

FolderData::FolderData(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime)
    : Data(iName, iBirthTime, iModifiedTime)
{}
////////////////////////////////////////

std::list<std::shared_ptr<Data>>& FolderData::GetChildren(){
    return _children;
}
////////////////////////////////////////

void FolderData::addChild(std::shared_ptr<Data> iChild){
    _children.push_back(iChild);
}
////////////////////////////////////////

void FolderData::Populate(const std::string& iPath, std::vector<std::string> iFilters ,bool iIsBlackList){

    QDir currentDirectory(QString::fromStdString(iPath), "", QDir::Name | QDir::IgnoreCase, QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QList<QFileInfo> dirContents = currentDirectory.entryInfoList();
    foreach(auto& entry, dirContents){
        std::string fullName = entry.absoluteFilePath().toStdString();
        QDateTime birthTime = entry.birthTime();
        std::string birthStr = "";
        if (birthTime.isValid()){
            birthStr = birthTime.toString().toStdString();
            auto pos = birthStr.find_first_of(' ');
            if (pos != std::string::npos){
                birthStr = birthStr.substr(pos + 1);
            }
        }

        QDateTime modifiedTime = entry.lastModified();
        std::string modifiedStr = "";
        if (modifiedTime.isValid()){
            modifiedStr = modifiedTime.toString().toStdString();
            auto pos = modifiedStr.find_first_of(' ');
            if (pos != std::string::npos){
                modifiedStr = modifiedStr.substr(pos + 1);
            }
        }

        if (entry.isDir()){
            FolderData newFolder(fullName, birthStr, modifiedStr);
            newFolder.Populate(fullName, iFilters, iIsBlackList);
            addChild(std::make_shared<FolderData>(newFolder));
        }
        else if (entry.isFile()){
            FileData newFile(fullName, birthStr, modifiedStr, entry.size());
            std::string fileExtension = newFile.GetName();
            auto periodPos = fileExtension.find_last_of('.');
            fileExtension = fileExtension.substr(periodPos + 1);
            bool blacklisted = false;
            for (auto& filter : iFilters){
                if (filter == fileExtension){
                    if (iIsBlackList){
                        blacklisted = true;
                    }
                    else {
                        addChild(std::make_shared<FileData>(newFile));
                    }
                    break;
                }
            }
            if (iIsBlackList && !blacklisted){
                addChild(std::make_shared<FileData>(newFile));
            }
        }
    }
}
////////////////////////////////////////

bool FolderData::PopulateFromXML(const std::string& iXML, std::string& oErrMssg){
    //Remove top and bottom <Folder>
    std::string tmpStr = iXML;

    //Removing first balise and grabbing name from it
    {
        auto firstPos = tmpStr.find_first_of('>');
        if (firstPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find first '>' in \"" + iXML + "\"";
            return false;
        }
        std::string firstBalise = tmpStr.substr(0, firstPos + 1);//<Folder name=TEST birthTime=Jul 30 18:18:01 2026 modifiedTime=Jul 30 18:18:24 2026>
        tmpStr = tmpStr.substr(firstPos + 1);

        auto tmpPos = firstBalise.find("name=");
        if (tmpPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find \"name=\" in \"" + iXML + "\"";
            return false;
        }
        firstBalise = firstBalise.substr(tmpPos + 5);//TEST birthTime=Jul 30 18:18:01 2026 modifiedTime=Jul 30 18:18:24 2026>

        tmpPos = firstBalise.find("birthTime=");
        if (tmpPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find \"birthTime=\" in \"" + iXML + "\"";
            return false;
        }
        std::string name = firstBalise.substr(0, tmpPos - 1);//TEST
        firstBalise = firstBalise.substr(tmpPos + 10);//Jul 30 18:18:01 2026 modifiedTime=Jul 30 18:18:24 2026>

        tmpPos = firstBalise.find("modifiedTime=");
        if (tmpPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find \"modifiedTime=\" in \"" + iXML + "\"";
            return false;
        }
        std::string birthTime = firstBalise.substr(0, tmpPos - 1);//Jul 30 18:18:01 2026
        std::string modifiedTime = firstBalise.substr(tmpPos + 13);//Jul 30 18:18:24 2026>
        modifiedTime.pop_back();//Jul 30 18:18:24 2026

        SetName(name);
        SetBirthTime(birthTime);
        SetModifiedTime(modifiedTime);
    }

    //Removing last balise
    {
        auto lastPos = tmpStr.find_last_of('<');
        if (lastPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find last '<' in \"" + iXML + "\"";
            return false;
        }
        tmpStr = tmpStr.substr(0, lastPos);
    }
    while (!tmpStr.empty()){
        auto nextPos = tmpStr.find_first_of('>');
        if (nextPos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find first '>' in \"" + tmpStr + "\"";
            return false;
        }

        std::string nextBalise = tmpStr.substr(0, nextPos + 1);
        tmpStr = tmpStr.substr(nextPos + 1);

        if (nextBalise.size() < 9){
            oErrMssg += "FolderData::PopulateFromXML detected balise is too small: \"" + nextBalise + "\"";
            return false;
        }
        // Balise is a File balise
        else if (nextBalise.find("<File") != std::string::npos){
            FileData newFile("", "", "", 0); // We create a new file
            // fill its data from the xml
            if (!newFile.PopulateFromXML(nextBalise, oErrMssg)){
                return false;
            }
            addChild(std::make_shared<FileData>(newFile)); // add it to our children
        }
        // Balise is a /Folder balise
        else if (nextBalise.find("</Folder") != std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML detected balise is incorrect: \"" + nextBalise + "\"";
            return false;
        }
        // Balise is a Folder balise
        else if (nextBalise.find("<Folder") != std::string::npos){
            auto nextFolderPos = tmpStr.find("<Folder");
            auto nextFolderEndPos = tmpStr.find("</Folder");
            while (nextFolderPos < nextFolderEndPos && nextFolderEndPos != std::string::npos && nextFolderPos != std::string::npos){
                nextFolderPos = tmpStr.find("<Folder", nextFolderPos + 1);
                nextFolderEndPos = tmpStr.find("</Folder", nextFolderEndPos + 1);
            }

            nextFolderEndPos = tmpStr.find("</Folder", nextFolderEndPos);
            nextBalise += tmpStr.substr(0, nextFolderEndPos + 9);
            tmpStr = tmpStr.substr(nextFolderEndPos + 9);

            FolderData newFolder("", "", "");
            if (!newFolder.PopulateFromXML(nextBalise, oErrMssg)){
                return false;
            }
            addChild(std::make_shared<FolderData>(newFolder));
        }
    }
    return true;
}
////////////////////////////////////////

std::string FolderData::PrintToXML(){
    std::string xmlLine = "<Folder name=" + GetName()
                          + " birthTime=" + GetBirthTime()
                          + " modifiedTime=" + GetModifiedTime()
                          + ">";

    for (std::shared_ptr<Data>& children : GetChildren()){
        xmlLine += children->PrintToXML();
    }
    xmlLine += "</Folder>";
    return xmlLine;
}
////////////////////////////////////////

void FolderData::FillTreeView(QTreeWidgetItem * oItem){
    oItem->setText(0, QString::fromStdString(GetName()));
    oItem->setText(1, QString::fromStdString(GetModifiedTime()));
    oItem->setText(2, "Folder");
    for (std::shared_ptr<Data>& children : GetChildren()){
        QTreeWidgetItem * newChildItem = new QTreeWidgetItem();
        oItem->addChild(newChildItem);
        children->FillTreeView(newChildItem);
    }
}
////////////////////////////////////////

bool FolderData::RemoveEmptyDirectories(){
    std::list<std::shared_ptr<Data>>::iterator it;
    std::list<std::shared_ptr<Data>>& children = GetChildren();
    for (it = children.begin(); it != children.end(); ++it){
        bool toBeRemoved = it->get()->RemoveEmptyDirectories();
        if (toBeRemoved){
            children.erase(it);
        }
    }

    if (children.empty()){
        return true;
    }
    return false;
}
////////////////////////////////////////


