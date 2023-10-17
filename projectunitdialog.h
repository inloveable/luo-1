#ifndef PROJECTUNITDIALOG_H
#define PROJECTUNITDIALOG_H

#include <QDialog>
#include<QMap>

namespace Ui {
class projectUnitDialog;
}

class QComboBox;
class projectUnitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit projectUnitDialog(QWidget *parent = nullptr);
    ~projectUnitDialog();


    void comboboxCellConfiguration();

    void onComboBoxIndexChanged(const QString&);

signals:
    void                       changeChannelValue(const QString& table,int index,
                                                  const QString& v1,const QString& v2);


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::projectUnitDialog *ui;
    QMap<QString,QString> mapToFactor;
    QMap<QComboBox*,int> mapFromCombobox;
};

#endif // PROJECTUNITDIALOG_H
