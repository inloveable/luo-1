#ifndef DISPLAYSETTINGDIALOG_H
#define DISPLAYSETTINGDIALOG_H

#include "qcombobox.h"
#include <QDialog>

namespace Ui {
class DisplaySettingDialog;
}

class DisplaySettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplaySettingDialog(QWidget *parent = nullptr);
    ~DisplaySettingDialog();

    void changeColorFunc();
    void onWidthChanged(const QString& text);


    void updateChartState();






signals:
    void checkBoxStateChanged(int index,int state);
    void changeLineWidthSignal(int index,int width);
    void changeLineColorSignal(int index,QColor c);
    void changeLinePattern(int index,const QString& text);

    void                       changeExSettingValue(const QString& table,
                                                    int index,int index2,const QString& value);
    void                       changeExSettingValue(const QString& table,int index,
                                                   const QList<QString>&);




private slots:
    void on_pushButton_clicked();

private:
    Ui::DisplaySettingDialog *ui;

    QMap<QWidget*,QPair<int,int>> mapToPosition;
    QStringList colorList;


};

#endif // DISPLAYSETTINGDIALOG_H
