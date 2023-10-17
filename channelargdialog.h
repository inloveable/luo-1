#ifndef CHANNELARGDIALOG_H
#define CHANNELARGDIALOG_H

#include <QDialog>
#include<QMap>
namespace Ui {
class ChannelArgDialog;
}

class QComboBox;
class QTableWidgetItem;
class ChannelArgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelArgDialog(QWidget *parent = nullptr);
    ~ChannelArgDialog();




signals:
    void                       changeChannelValue(const QString& table,int index,
                                                  const QString& v1,const QString& v2);
    void                       changeChannelValueEx(const QString& table,int index,
                                                   const QList<QString>&);
    void                       rebuild();

private slots:
    void on_unitButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::ChannelArgDialog *ui;


    void inputChannelTableConfiguration();
    void flurTableConfiguration();
    void stopRateConfiguration();
    void callTableConfiguraion();
    QMap<QComboBox*,QPair<int,int>> mapFromCombobox_inputChannel;
    QMap<QComboBox*,QPair<int,int>> mapFromCombobox_Callibrate;
    QMap<QComboBox*,QPair<int,int>> mapFromCombobox_Flur;

    void updateArgData();


    void processStopRate(QTableWidgetItem* i);
    void processCallibrate(int i);
};

#endif // CHANNELARGDIALOG_H
