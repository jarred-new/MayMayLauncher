#include "crashdialog.h"
#include "ui_crashdialog.h"

CrashDialog::CrashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton,
            SIGNAL(clicked(bool)),
            this,
            SLOT(fullClose()));
}

CrashDialog::~CrashDialog()
{
    delete ui;
}

void CrashDialog::setCrashText(const QString &text)
{
    ui->textEdit->setPlainText(text);
}

void CrashDialog::fullClose()
{
    qApp->closeAllWindows();
    RestoreUpAll();
    qApp->quit();
}
