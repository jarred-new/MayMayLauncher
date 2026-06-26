#include "mainwindow.h"
#include "crashhandler.h"
#include "setup.h"

#include <QApplication>

#include <QSettings>
#include <QMessageBox>
#include <QCheckBox>

#include <windows.h>
#include <ShlDisp.h>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Ole32.lib")

void MinimieAllWindows() {
    // Initialize COM library
        CoInitialize(NULL);

        IShellDispatch* pShell = NULL;

        // Create an instance of the Shell object
        HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pShell);

        if (SUCCEEDED(hr) && pShell != NULL) {
            // Call the official MinimizeAll method
            pShell->MinimizeAll();
            pShell->Release();
        }

        // Uninitialize COM
        CoUninitialize();
}

void createWizard() {
    RegistrationWizard wizard;

    if(wizard.exec() == QDialog::Accepted)
    {
        QString nickname =
            wizard.field("nickname").toString();

        QString picture =
            wizard.property(
                "profilePicture").toString();

        QSettings settings("JarredApps", "MayMayLauncher");

        settings.setValue(
            "Profile/Nickname",
            nickname);

        settings.setValue(
            "Profile/Picture",
            picture);

//        QMessageBox::information(
//            NULL,
//            "Saved",
//            "Profile information has been saved.");

        settings.setValue("firstTime",
                          false);
    }
    else {
//        QMessageBox::StandardButton comfirm = QMessageBox::question(wizard,
//                                                                    "Are you sure?",
//                                                                    "Do you want to quit?",
//                                                                    QMessageBox::Yes | QMessageBox::No);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("JarredApps", "MayMayLauncher");

    if (settings.value("firstTime", true).toBool()) {
        createWizard();
        // 1. Check if the user previously checked the box
        if (settings.value("skip_warning_msg", false).toBool()) {
    //        QMessageBox::information(nullptr,
    //            "Cannot Continue",
    //            "The warning message has been skipped as per your preference.");
    //        return 0;
            MinimieAllWindows();
            CrashHandler::install();
            MainWindow w;
            w.show();

            return a.exec();
        }

        // 2. Configure the QMessageBox instance
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle("Warning!");
        msgBox.setWindowIcon(QIcon(":/default/MayMayLauncher.ico"));
        msgBox.setIcon(QMessageBox::Icon::Warning);
        msgBox.setText("This app will minimize all windows "
                       "\n"
                       "(which it will never ruin your work but it will be buggy.)"
                       "\n"
                       "As you close this launcher, it will restore all apps\n\n"
                       "Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        // 3. Create and attach the checkbox
        QCheckBox *checkBox = new QCheckBox("Don't show this message again", &msgBox);
        msgBox.setCheckBox(checkBox);

        // 4. Display the dialog and save user preference if accepted
        int result = msgBox.exec();
        if (result == QMessageBox::Yes) {
            if (checkBox->isChecked()) {
                settings.setValue("skip_warning_msg", true);
            }
            MinimieAllWindows();
            CrashHandler::install();
            MainWindow w;
            w.show();

            return a.exec();
        }
        else {
            return 0;
        }
    }
    else {

        // 1. Check if the user previously checked the box
        if (settings.value("skip_warning_msg", false).toBool()) {
    //        QMessageBox::information(nullptr,
    //            "Cannot Continue",
    //            "The warning message has been skipped as per your preference.");
    //        return 0;
            MinimieAllWindows();
            CrashHandler::install();
            MainWindow w;
            w.show();

            return a.exec();
        }

        // 2. Configure the QMessageBox instance
        QMessageBox msgBox(nullptr);
        msgBox.setWindowTitle("Warning!");
        msgBox.setWindowIcon(QIcon(":/default/MayMayLauncher.ico"));
        msgBox.setIcon(QMessageBox::Icon::Warning);
        msgBox.setText("This app will minimize all windows "
                       "\n"
                       "(which it will never ruin your work but it will be buggy.)"
                       "\n"
                       "As you close this launcher, it will restore all apps\n\n"
                       "Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        // 3. Create and attach the checkbox
        QCheckBox *checkBox = new QCheckBox("Don't show this message again", &msgBox);
        msgBox.setCheckBox(checkBox);

        // 4. Display the dialog and save user preference if accepted
        int result = msgBox.exec();
        if (result == QMessageBox::Yes) {
            if (checkBox->isChecked()) {
                settings.setValue("skip_warning_msg", true);
            }
            MinimieAllWindows();
            CrashHandler::install();
            MainWindow w;
            w.show();

            return a.exec();
        }
        else {
            return 0;
        }
    }

    return 0;
}
