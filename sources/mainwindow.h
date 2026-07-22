#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class QListWidget;
class QTreeWidget;
class QListWidgetItem;
class QTreeWidgetItem;
class QVBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void BrowseButtonPressed();
    void ChangeDirectory();
    void NdpSelectionChanged();

    void ToggleButtons();
    void CreateButtonPressed();

    void CreateAccepted(std::string iPath, std::string iName, std::vector<std::string> iFilters, bool iBlackList, bool iRemoveEmptyFolders);
    void UnFreezeUI();

    void FilterTreeView();

private:
    void CreateErrorDialog(const std::string& iMssg);

    void UnHideAllItems();
    void UnHideAllItems(QTreeWidgetItem * iItem);
    bool FilterItem(QTreeWidgetItem * iItem, QString iFilter);

private:
    QPushButton * _browseButton;
    QLineEdit * _directoryLineEdit;

    QPushButton * _createButton;
    QPushButton * _editButton;
    QPushButton * _removeButton;

    QLineEdit * _searchLineEdit;
    QPushButton * _searchButton;

    QListWidget * _imageList;

    QTreeWidget * _tree;
    QVBoxLayout * _treeLayout;

};
#endif // MAINWINDOW_H
