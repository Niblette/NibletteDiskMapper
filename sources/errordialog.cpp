#include "errordialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

ErrorDialog::ErrorDialog(const std::string& iMssg, QWidget * iParent)
    : QWidget(iParent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * mainLayout = new QVBoxLayout();

    QTextEdit * text = new QTextEdit("iMssg");
    text->setReadOnly(true);
    mainLayout->addWidget(text);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(100);
    QPushButton * exitButton = new QPushButton("Exit");
    connect(exitButton, &QPushButton::clicked, this, &ErrorDialog::ExitPressed);
    buttonLayout->addWidget(exitButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}
////////////////////////////////////////

ErrorDialog::~ErrorDialog() = default;

void ErrorDialog::ExitPressed(){
    this->close();
}
////////////////////////////////////////

