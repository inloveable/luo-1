#ifndef MOUSESETTINGDIALOG_H
#define MOUSESETTINGDIALOG_H


#include "qnamespace.h"
#include <QDialog>
#include<QMap>
namespace Ui {
class mouseSettingDialog;
}
class LineItem;
class mouseSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mouseSettingDialog(QWidget *parent = nullptr);
    ~mouseSettingDialog();

private slots:
    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    LineItem* addLineCursor();

private:
    Ui::mouseSettingDialog *ui;

    static QMap<QString,LineItem*> lineMap;

    QColor color=Qt::red;

    static int current;
};

#endif // MOUSESETTINGDIALOG_H
