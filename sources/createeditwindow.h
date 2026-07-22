#ifndef CREATEEDITWINDOW_H
#define CREATEEDITWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QCheckBox>

class CreateEditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CreateEditWindow(QWidget *parent = nullptr);
    ~CreateEditWindow() override;

signals:
    void Accepted(std::string path, std::string name, std::vector<std::string> filters, bool blacklist, bool removeEmptyFolders);

private slots:

    void ToogleAcceptButton();

    void AcceptPressed();
    void BrowsePressed();
    void QuitPressed();

private:
    QLineEdit * _path;
    QPushButton * _browseButton;

    QLineEdit * _name;

    QRadioButton * _blackListRb;
    QTextEdit * _blackListTextEdit;

    QRadioButton * _whiteListRb;
    QTextEdit * _whiteListTextEdit;

    QCheckBox * _removeEmptyFolders;

    QPushButton * _acceptButton;
    QPushButton * _quitButton;
};

#endif // CREATEEDITWINDOW_H
