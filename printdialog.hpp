#pragma once

#include <QDialog>

namespace Ui {
class PrintDialog;
}
class MainWindow;
class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = nullptr);
    ~PrintDialog();
      MainWindow* mainwindow;
      friend class MainWindow;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


private:
    Ui::PrintDialog *ui;

};

