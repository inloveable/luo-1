#pragma once

#include "qdialog.h"
#include "ui_exdocumentinformationdialog.h"

class ExDocumentInformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExDocumentInformationDialog(QWidget *parent = nullptr);

private:
    Ui::ExDocumentInformationDialog ui;
};

