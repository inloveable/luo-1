#ifndef MAINWINDOW_H
#define MAINWINDOW_H





#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class firstWindow;
class UIManager;
class SYSBackEnd;
class ExArgSettingDialog;
class ExSettingReplyDialog;
class NetworkManager;
class PrintDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class ExType{SHOCKReply,HorizontalSHOCK};

private slots:
    void on_channelArgBtn_clicked();

    void on_moveLeftBtn_clicked();

    void on_moveRightBtn_clicked();

    void on_moveUpBtn_clicked();

    void on_moveDownBtn_clicked();

    void on_clearPVBtn_clicked();

    void on_resetBtn_clicked();

    void on_zoomBtn_clicked();

    void on_displaySettingBtn_clicked();

    void on_mouseSettingBtn_clicked();

    void on_pushButton_9_clicked();

    void on_commentBtn_clicked();


    void updatePeekVal(double x,double y);
    friend class UIManager;
    friend class PrintDialog;

    void on_pushButton_5_clicked();

    void on_zoomBtn_2_clicked();

    void on_commentBtn_2_clicked();

    void on_moveLeftBtn_2_clicked();

private:
    Ui::MainWindow *ui;
    firstWindow* prewindow;


    UIManager* uiManager=nullptr;

    //NetworkManager* network;
    //QThread* networkThread;





    void actualShow(ExType mark);
    void prewindowConfiguration();
    void networkConfiguration();
    void uiConfiguration(ExType type);

    ExType uiType;

    void startAcquisition();

    void channelArgDialogReconstruct();
    void exArgDialogReconstruct();

    ExArgSettingDialog* horizontalShockDialog;
    ExSettingReplyDialog*    shockReplyDialog;

    double getTimeZoneChartRange();
    double getTimeZoneYChartRange();
    QPair<double,double>* YPeekVal=nullptr;




};
#endif // MAINWINDOW_H
