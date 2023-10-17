#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "enhancedchartviewsystem.h"
#include "qnamespace.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_5_clicked()
{
    static bool pushed=false;
    if(!pushed)
    {
        ui->pushButton_5->setFlat(true);
        ui->graphicsView->setIsRubberBand(true);
        pushed=true;
    }
    else
    {
        ui->pushButton_5->setFlat(false);
        ui->graphicsView->setIsRubberBand(false);
        pushed=false;
    }
}


void MainWindow::on_pushButton_6_clicked()
{
    ui->graphicsView->resetZoom();
}


void MainWindow::on_pushButton_clicked()
{
    ui->graphicsView->showPeekValue
            (0,EnhancedChartViewSystem::PEEK::MAX);
}


void MainWindow::on_pushButton_7_clicked()
{
    ui->graphicsView->showPeekValue
            (1,EnhancedChartViewSystem::PEEK::MIN);
}


void MainWindow::on_pushButton_8_clicked()
{
    ui->graphicsView->showPeekValue
            (1);
}





void MainWindow::on_pushButton_2_pressed()
{
    ui->graphicsView->scollOverX(true);
}


void MainWindow::on_pushButton_9_clicked()
{
    ui->graphicsView->scollOverX(false);
}


void MainWindow::on_pushButton_10_clicked()
{
    ui->graphicsView->clearCallout();
}


void MainWindow::on_pushButton_11_clicked()
{
    static bool s=true;
    ui->graphicsView->setShowCallout(s);
    s=!s;
}


void MainWindow::on_pushButton_12_clicked()
{
    static bool s=true;
    ui->graphicsView->setOpenGL(s);
    s=!s;
}


void MainWindow::on_pushButton_3_clicked()
{

}


void MainWindow::on_pushButton_13_clicked()
{
    ui->graphicsView->addZoneCursor();
}


void MainWindow::on_pushButton_14_clicked()
{
    ui->graphicsView->addLineCursor();
    ui->graphicsView->setLineCursorColor(QColor{Qt::red});
}

