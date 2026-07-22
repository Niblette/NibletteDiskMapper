#include "createeditwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>


CreateEditWindow::CreateEditWindow(QWidget * iParent)
    :QWidget(iParent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout * pathLayout = new QHBoxLayout();
    _path = new QLineEdit();
    _path->setPlaceholderText("Choose the folder/drive to map");
    _path->setReadOnly(true);
    connect(_path, &QLineEdit::editingFinished, this, &CreateEditWindow::ToogleAcceptButton);
    _browseButton = new QPushButton("Browse");
    connect(_browseButton, &QPushButton::pressed, this, &CreateEditWindow::BrowsePressed);
    pathLayout->addWidget(_path);
    pathLayout->addWidget(_browseButton);

    QLabel * nameLabel = new QLabel("Name of the generated file.");
    _name = new QLineEdit();
    _name->setPlaceholderText("Choose a name");
    connect(_name, &QLineEdit::editingFinished, this, &CreateEditWindow::ToogleAcceptButton);

    QVBoxLayout * filtersLayout = new QVBoxLayout();
    _blackListRb = new QRadioButton("BlackList");
    connect(_blackListRb, &QRadioButton::clicked, this, &CreateEditWindow::ToogleAcceptButton);
    _blackListTextEdit = new QTextEdit();
    _blackListTextEdit->setPlaceholderText("Add extensions to blacklist here ex: pdf;png;mkv;mp4 ...");
    _whiteListRb = new QRadioButton("WhiteList");
    connect(_whiteListRb, &QRadioButton::clicked, this, &CreateEditWindow::ToogleAcceptButton);
    _whiteListTextEdit = new QTextEdit();
    _whiteListTextEdit->setPlaceholderText("Add extensions to whitelist here ex: pdf;png;mkv;mp4 ...");
    filtersLayout->addWidget(_blackListRb);
    filtersLayout->addWidget(_blackListTextEdit);
    filtersLayout->addWidget(_whiteListRb);
    filtersLayout->addWidget(_whiteListTextEdit);

    _removeEmptyFolders = new QCheckBox("Remove empty directories from the map?");

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    _acceptButton = new QPushButton("Accept");
    connect(_acceptButton, &QPushButton::pressed, this, &CreateEditWindow::AcceptPressed);
    _quitButton = new QPushButton("Cancel");
    connect(_quitButton, &QPushButton::pressed, this, &CreateEditWindow::QuitPressed);
    buttonLayout->addStretch(100);
    buttonLayout->addWidget(_acceptButton);
    buttonLayout->addWidget(_quitButton);

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addLayout(pathLayout);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(_name);
    mainLayout->addLayout(filtersLayout);
    mainLayout->addWidget(_removeEmptyFolders);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    ToogleAcceptButton();
}
////////////////////////////////////////

CreateEditWindow::~CreateEditWindow() = default;
////////////////////////////////////////

void CreateEditWindow::ToogleAcceptButton(){
    if (_name->text().isEmpty()){
        _acceptButton->setDisabled(true);
        return;
    }

    if (_path->text().isEmpty()){
        _acceptButton->setDisabled(true);
        return;
    }

    if (!_blackListRb->isChecked() && !_whiteListRb->isChecked()){
        _acceptButton->setDisabled(true);
        return;
    }
    _acceptButton->setDisabled(false);
}
////////////////////////////////////////

void CreateEditWindow::AcceptPressed(){

    std::string path = _path->text().toStdString();
    std::string name = _name->text().toStdString();
    bool blacklist = _blackListRb->isChecked();
    bool removeEmptyFolders = _removeEmptyFolders->isChecked();

    std::vector<std::string> filters;
    char separator = ';';
    std::string text;
    if (blacklist){
        text = _blackListTextEdit->toPlainText().toStdString();
    }
    else {
        text = _whiteListTextEdit->toPlainText().toStdString();
    }
    unsigned long long sepPos = 0;
    while (!text.empty() && sepPos != std::string::npos){
        sepPos = text.find_first_of(separator);
        std::string token = text.substr(0, sepPos);
        filters.push_back(token);
        text = text.substr(sepPos + 1, std::string::npos);
    }
    emit Accepted(path, name, filters, blacklist, removeEmptyFolders);
    this->close();
}
////////////////////////////////////////

void CreateEditWindow::BrowsePressed(){

    // user is asked to choose a directory
    QString chosenDir = QFileDialog::getExistingDirectory(this, "Choose a folder", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (chosenDir.isEmpty()){ // it being empty means user closed the window without choosing, we leave
        return;
    }
    // If user chose a directory, we update the text and change the directory !
    _path->setText(chosenDir);
}
////////////////////////////////////////

void CreateEditWindow::QuitPressed(){
    this->close();
}
////////////////////////////////////////

