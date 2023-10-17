#ifndef CONNECTINPUTDIALOG_H
#define CONNECTINPUTDIALOG_H

#include <QDialog>
#include<QHostAddress>
namespace Ui {
class ConnectInputDialog;
}

class ConnectInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectInputDialog(QWidget *parent = nullptr);
    ~ConnectInputDialog();
signals:

    void connectStart(QHostAddress& add,long port);


private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::ConnectInputDialog *ui;
};

#endif // CONNECTINPUTDIALOG_H
