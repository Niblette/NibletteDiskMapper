#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>

class QTreeWidgetItem;

class Data
{
public:
    Data(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime);

    const std::string& GetName();
    void SetName(const std::string& iName);

    const std::string& GetBirthTime();
    void SetBirthTime(const std::string& iBirthTime);

    const std::string& GetModifiedTime();
    void SetModifiedTime(const std::string& iModifiedTime);

    virtual std::string PrintToXML() = 0;
    virtual void FillTreeView(QTreeWidgetItem * oItem) = 0;

    virtual bool RemoveEmptyDirectories() = 0;

private:
    std::string _name;
    std::string _birthTime;
    std::string _modifiedTime;
};

#endif // DATASTRUCT_H
