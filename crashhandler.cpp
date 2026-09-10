#include "crashhandler.h"
#include "crashdialog.h"

#include <QApplication>
#include <QDateTime>

#include <signal.h>
#include <exception>
#include <cstdlib>

CrashHandler::CrashHandler()
{

}

void CrashHandler::install()
{
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE,  signalHandler);
    signal(SIGILL,  signalHandler);

    std::set_terminate([]()
    {
        showCrashDialog(
            "Unhandled C++ exception\n\n"
            "std::terminate() was called."
        );

        std::_Exit(EXIT_FAILURE);
    });
}

void CrashHandler::signalHandler(int signal)
{
    Q_UNUSED(signal);
    std::_Exit(EXIT_FAILURE);
}

void CrashHandler::showCrashDialog(const QString &text)
{
    int argc = 0;
    char **argv = 0;

    QApplication app(argc, argv);

    CrashDialog dlg;
    dlg.setCrashText(text);
    dlg.exec();
}

