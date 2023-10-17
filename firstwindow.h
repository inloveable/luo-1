#ifndef FIRSTWINDOW_H
#define FIRSTWINDOW_H


#include"mainwindow.h"
#include <QHostAddress>
#include <QDialog>

namespace Ui {
class firstWIndow;
}


class NetworkManager;
class firstWindow : public QDialog
{
    Q_OBJECT

public:
    explicit firstWindow(QWidget *parent = nullptr);
    ~firstWindow();


    void isConnectedToDevice(bool s);

   signals:
    void activateMainwindow(MainWindow::ExType mark);

    void connectToDevice(QHostAddress&,long port);

private slots:
    void on_pushButton_2_clicked();

    void on_connectToHostButton_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::firstWIndow *ui;

    bool isDeviceConnected=false;

    QHostAddress add;
    long         port;


};

#endif // FIRSTWINDOW_H
