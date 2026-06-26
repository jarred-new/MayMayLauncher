#ifndef CRASHDIALOG_H
#define CRASHDIALOG_H

#include <QDialog>

#include <windows.h>
#include <ShlDisp.h>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Ole32.lib")

namespace Ui {
class CrashDialog;
}

class CrashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashDialog(QWidget *parent = 0);
    ~CrashDialog();

public:
    void setCrashText(const QString &text);
    void RestoreUpAll() {
        // Initialize COM library
            CoInitialize(NULL);

            IShellDispatch* pShell = NULL;

            // Create an instance of the Shell object
            HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pShell);

            if (SUCCEEDED(hr) && pShell != NULL) {
                // Call the official UndoMinimizeAll method
                pShell->UndoMinimizeALL();
                pShell->Release();
            }

            // Uninitialize COM
            CoUninitialize();
    }

public slots:
    void fullClose();

private:
    Ui::CrashDialog *ui;
};

#endif // CRASHDIALOG_H
