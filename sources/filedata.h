#ifndef FILEDATA_H
#define FILEDATA_H

#include "datastruct.h"

class FileData: public Data
{
public:
    FileData(const std::string& iName, int iSize);

    int GetSize();
    void SetSize(int iSize);

    bool PopulateFromXML(const std::string& iXML, std::string& oErrMssg);

    std::string PrintToXML() override;

    void FillTreeView(QTreeWidgetItem * oItem) override;

    bool RemoveEmptyDirectories() override;

    //void print(const std::string& iSpacer) override;

private:
    int _size;
};


#endif // FILEDATA_H
