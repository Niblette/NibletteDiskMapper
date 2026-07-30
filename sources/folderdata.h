#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include "datastruct.h"

#include <list>
#include <vector>
#include <memory>

class QTreeWidgetItem;

class FolderData: public Data
{
public:

    FolderData(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime);

    std::list<std::shared_ptr<Data>>& GetChildren();
    void addChild(std::shared_ptr<Data> iChild);

    void Populate(const std::string& iPath, std::vector<std::string> iFilters ,bool iIsBlackList);

    bool PopulateFromXML(const std::string& iXML, std::string& oErrMssg);

    std::string PrintToXML() override;

    void FillTreeView(QTreeWidgetItem * oItem) override;

    bool RemoveEmptyDirectories() override;

    //void print();
    //void print(const std::string& iSpacer) override;

private:
    std::list<std::shared_ptr<Data>> _children;
};

#endif // FOLDERDATA_H
