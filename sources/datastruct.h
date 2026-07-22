#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>

class QTreeWidgetItem;

class Data
{
public:
    Data(const std::string& iName);

    const std::string& GetName();
    void SetName(const std::string& iName);

    virtual std::string PrintToXML() = 0;
    virtual void FillTreeView(QTreeWidgetItem * oItem) = 0;

    virtual bool RemoveEmptyDirectories() = 0;

    //virtual void print(const std::string& iSpacer) = 0;

private:
    std::string _name;
};

#endif // DATASTRUCT_H
