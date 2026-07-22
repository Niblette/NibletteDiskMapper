#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QObject>
#include <QWidget>

class ErrorDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorDialog(const std::string& iMssg, QWidget *parent = nullptr);
    ~ErrorDialog() override;

private slots:
    void ExitPressed();

private:
};

#endif // ERRORDIALOG_H
