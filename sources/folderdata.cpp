#include "folderdata.h"
#include "filedata.h"

#include <iostream>
#include <filesystem>

#include <QTreeWidgetItem>

FolderData::FolderData(const std::string& iName)
    : Data(iName)
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
    std::filesystem::path path(iPath);
    std::error_code ec;
    auto dirEntry = std::filesystem::begin(std::filesystem::directory_iterator(path, ec));
    for (; dirEntry != std::filesystem::end(std::filesystem::directory_iterator(path, ec)); dirEntry.increment(ec)){
        if (dirEntry->is_directory()){
            FolderData newFolder(dirEntry->path().string());
            newFolder.Populate(dirEntry->path().string(), iFilters, iIsBlackList);
            addChild(std::make_shared<FolderData>(newFolder));
        }
        else if (dirEntry->is_regular_file()){
            FileData newFile(dirEntry->path().string(), dirEntry->file_size());

            std::string fileExtension = newFile.GetName();
            auto periodPos = fileExtension.find_last_of('.');
            fileExtension = fileExtension.substr(periodPos + 1);
            bool blacklisted = false;
            for (std::string filter : iFilters){
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
        std::string firstBalise = tmpStr.substr(0, firstPos + 1);
        tmpStr = tmpStr.substr(firstPos + 1);

        auto namePos = firstBalise.find_first_of('=');
        if (namePos == std::string::npos){
            oErrMssg += "FolderData::PopulateFromXML failed to find first '=' in \"" + iXML + "\"";
            return false;
        }
        firstBalise = firstBalise.substr(namePos + 1);
        firstBalise.pop_back(); // remove trailing '>'
        SetName(firstBalise);
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
            FileData newFile("", 0); // We create a new file
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

            FolderData newFolder("");
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
    std::string xmlLine = "<Folder name=" + GetName()  + ">";
    for (std::shared_ptr<Data>& children : GetChildren()){
        xmlLine += children->PrintToXML();
    }
    xmlLine += "</Folder>";
    return xmlLine;
}
////////////////////////////////////////

void FolderData::FillTreeView(QTreeWidgetItem * oItem){
    oItem->setText(0, QString::fromStdString(GetName()));
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

//void FolderData::print(){
//    print("");
//}
////////////////////////////////////////

//void FolderData::print(const std::string& iSpacer){
//    std::cout << iSpacer << GetName() << std::endl;
//    for (std::shared_ptr<Data>& children : GetChildren()){
//        children->print(iSpacer + " ");
//    }
//}
////////////////////////////////////////
