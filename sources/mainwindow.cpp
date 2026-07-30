#include "mainwindow.h"
#include "folderdata.h"
#include "createeditwindow.h"
#include "errordialog.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QTextStream>

#include <iostream>
#include <fstream>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

#pragma region directory
    QHBoxLayout * directoryLayout = new QHBoxLayout();
    _directoryLineEdit = new QLineEdit();
    _directoryLineEdit->setReadOnly(true);
    directoryLayout->addWidget(_directoryLineEdit);
    _browseButton = new QPushButton("Browse");
    connect(_browseButton, &QPushButton::pressed, this, &MainWindow::BrowseButtonPressed);
    directoryLayout->addWidget(_browseButton);
#pragma endregion

#pragma region image
    QVBoxLayout * imageLayout = new QVBoxLayout();

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    _createButton = new QPushButton("Create");
    connect(_createButton, &QPushButton::pressed, this, &MainWindow::CreateButtonPressed);
    _editButton = new QPushButton("Edit");
    _editButton->setDisabled(true);
    _removeButton = new QPushButton("Delete");
    _removeButton->setDisabled(true);
    buttonLayout->addWidget(_createButton);
    buttonLayout->addWidget(_editButton);
    buttonLayout->addWidget(_removeButton);
    imageLayout->addLayout(buttonLayout);

    _imageList = new QListWidget();
    connect(_imageList, &QListWidget::itemClicked, this, &MainWindow::NdpSelectionChanged);
    imageLayout->addWidget(_imageList);

    QWidget * imageWidget = new QWidget(); // empty widget to be able the put the layout in the splitter
    imageWidget->setLayout(imageLayout);
#pragma endregion

#pragma region tree
    _treeLayout = new QVBoxLayout();

    QHBoxLayout * searchLayout = new QHBoxLayout();
    _searchLineEdit = new QLineEdit();
    _searchButton = new QPushButton("Search");
    _searchButton->setDisabled(true);
    connect(_searchButton, &QPushButton::pressed, this, &MainWindow::FilterTreeView);
    searchLayout->addWidget(_searchLineEdit);
    searchLayout->addWidget(_searchButton);
    _treeLayout->addLayout(searchLayout);

    _tree = new QTreeWidget();
    _tree->setColumnCount(4);
    _tree->setHeaderLabels({"Name", "Date Modified", "Size", "Type"});
    _treeLayout->addWidget(_tree);

    QWidget * treeWidget = new QWidget(); // empty widget to be able the put the layout in the splitter
    treeWidget->setLayout(_treeLayout);
#pragma endregion

    QSplitter * splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(imageWidget);
    splitter->addWidget(treeWidget);

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addLayout(directoryLayout);
    mainLayout->addWidget(splitter);

    this->setLayout(mainLayout);

    ToggleButtons();
}

MainWindow::~MainWindow() = default;



void MainWindow::BrowseButtonPressed(){
    //User is asked to choose a directory
    QString chosenDir = QFileDialog::getExistingDirectory(this, "Open the images directory", _directoryLineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //It being empty means user closed the window without choosing, we leave
    if (chosenDir.isEmpty()){
        return;
    }
    //If user chose a directory, we update the text and change the directory !
    _directoryLineEdit->setText(chosenDir);
    ChangeDirectory();
}
////////////////////////////////////////

void MainWindow::ChangeDirectory(){
    QDir newDirectory(_directoryLineEdit->text());
    //Get list of *.ndp files on the directory
    QStringList ndpFiles = newDirectory.entryList(QStringList() << "*.ndp" << "*.NDP",QDir::Files);
    //Empty the previous entries
    _imageList->clear();
    //For each of those files make an entry in the list
    foreach(QString fileName, ndpFiles) {
        QListWidgetItem * newEntry = new QListWidgetItem(fileName);
        _imageList->addItem(newEntry);
    }
    //Set correct buttons state
    ToggleButtons();
}
////////////////////////////////////////

void MainWindow::NdpSelectionChanged(){
    //We start by clearing the tree view
    _tree->clear();
    //Also clear the search bar and temporarily disable it
    _searchLineEdit->clear();
    _searchButton->setDisabled(true);
    //See if any item is selected
    if (_imageList->selectedItems().empty()){
        //Disable the search and leave if selection is empty
        return;
    }

    //Recreate the npd path with the current directory and the item name
    QString ndpPath = _directoryLineEdit->text() + "/" + _imageList->currentItem()->text();

    //Read entire file
    std::ifstream input(ndpPath.toStdString());
    std::ostringstream oss;
    //Failed to open the file !
    if (!input){
        std::string errorMssg = "Failed to open the file at \"" + ndpPath.toStdString() + "\"";
        CreateErrorDialog(errorMssg);
        return;
    }

    oss << input.rdbuf();

    //create our folder data structure and populate it from the xml data in the ndp file
    FolderData topFolder("", "", "");
    std::string errorMssg;
    if (!topFolder.PopulateFromXML(oss.str(), errorMssg)){
        std::string dialogErrorMssg = "Failed to create internal data structure: " + errorMssg;
        CreateErrorDialog(dialogErrorMssg);
        return;
    }

    // create new top lvl item, add it to the tree and recursively populate it
    QTreeWidgetItem * topItem = new QTreeWidgetItem();
    _tree->addTopLevelItem(topItem);
    topFolder.FillTreeView(topItem);
    _searchButton->setDisabled(false);
}
////////////////////////////////////////

void MainWindow::ToggleButtons(){
    if (_directoryLineEdit->text().isEmpty()){
        _createButton->setDisabled(true);
        return;
    }
    _createButton->setDisabled(false);
}
////////////////////////////////////////

void MainWindow::CreateButtonPressed(){
    //Dsiable all the mainwindow UI
    this->setDisabled(true);
    //Create the creation window
    CreateEditWindow * newWindow = new CreateEditWindow();
    //Unfreeze the UI when the creation window is closed/destroyed
    connect(newWindow, &CreateEditWindow::destroyed, this, &MainWindow::UnFreezeUI);
    //When user click accept on the new window call CreateAccepted()
    connect(newWindow, &CreateEditWindow::Accepted, this, &MainWindow::CreateAccepted);
    newWindow->show();
}
////////////////////////////////////////

void MainWindow::CreateAccepted(std::string iPath, std::string iName, std::vector<std::string> iFilters, bool iBlackList, bool iRemoveEmptyFolders){
    FolderData newFolder(iPath, "", "");
    newFolder.SetName(iName);
    newFolder.Populate(iPath, iFilters, iBlackList);


    if (iRemoveEmptyFolders){
        newFolder.RemoveEmptyDirectories();
    }

    std::string ndpContent = newFolder.PrintToXML();

    QString fileName = _directoryLineEdit->text() + "/" + QString::fromStdString(iName) + ".ndp";
    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << QString::fromStdString(ndpContent);
    }
    ChangeDirectory();
}

void MainWindow::UnFreezeUI(){
    this->setDisabled(false);
}

void MainWindow::FilterTreeView(){
    QString filter = _searchLineEdit->text();
    UnHideAllItems();
    if (filter.isEmpty()){
        return;
    }
    FilterItem(_tree->invisibleRootItem(), filter);
}

void MainWindow::CreateErrorDialog(const std::string& iMssg){
    this->setDisabled(true);
    ErrorDialog * errorDialog = new ErrorDialog(iMssg);
    connect(errorDialog, &ErrorDialog::destroyed, this, &MainWindow::UnFreezeUI);
    errorDialog->show();
}

void MainWindow::UnHideAllItems(){
    UnHideAllItems(_tree->invisibleRootItem());
}

void MainWindow::UnHideAllItems(QTreeWidgetItem * iItem){
    iItem->setHidden(false);
    for (int i = 0; i < iItem->childCount(); i++){
        QTreeWidgetItem * child = iItem->child(i);
        UnHideAllItems(child);
    }
}

bool MainWindow::FilterItem(QTreeWidgetItem * iItem, QString iFilter){
    bool retVal = false; //Return value, false means this item was hidden, true means it's not hidden;
    bool atLeastOneChildVisible = false;
    for (int i = 0; i < iItem->childCount(); i++){
        QTreeWidgetItem * child = iItem->child(i);

        if (FilterItem(child, iFilter)){
            atLeastOneChildVisible = true;
        }
    }
    bool containsFilter = iItem->text(0).contains(iFilter, Qt::CaseInsensitive)?true:false;
    if (atLeastOneChildVisible || containsFilter){
        iItem->setHidden(false);
        return true;
    }
    iItem->setHidden(true);
    return false;
}









