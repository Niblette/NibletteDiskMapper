#ifndef FILEDATA_H
#define FILEDATA_H

#include "datastruct.h"

#include <array>

class FileData: public Data
{
    constexpr static std::array<const char*, 7> prefixes = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
    constexpr static std::array<std::size_t, 6> sizes = {1'000, 1'000'000, 1'000'000'000, 1'000'000'000'000, 1'000'000'000'000'000, 1'000'000'000'000'000'000};


public:
    FileData(const std::string& iName, const std::string& iBirthTime, const std::string& iModifiedTime, std::size_t iSize);

    std::size_t GetSize();
    void SetSize(std::size_t iSize);

    bool PopulateFromXML(const std::string& iXML, std::string& oErrMssg);

    std::string PrintToXML() override;

    void FillTreeView(QTreeWidgetItem * oItem) override;

    bool RemoveEmptyDirectories() override;

private:
    std::string GetHumanReadableSize();

private:
    std::size_t _size;
};


#endif // FILEDATA_H
