#pragma once

#include <QWidget>
#include"publicDefs.hpp"
namespace Ui {
class shockReplayTable;
}

class shockReplayTable : public QWidget
{
    Q_OBJECT

public:
    explicit shockReplayTable(QWidget *parent = nullptr);
    ~shockReplayTable();


    void setSrsBar(SrsStateBar bar,int ch=0);

private:
    Ui::shockReplayTable *ui;
};

