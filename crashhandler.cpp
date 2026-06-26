#include "crashhandler.h"
#include "crashdialog.h"

#include <QApplication>
#include <QDateTime>

#include <signal.h>
#include <exception>

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

        abort();
    });
}

void CrashHandler::signalHandler(int signal)
{
    QString text;

    text += "Application crashed.\n\n";
    text += "Date: "
            + QDateTime::currentDateTime()
                  .toString()
            + "\n\n";

    switch(signal)
    {
        case SIGSEGV:
            text += "Signal: SIGSEGV\n";
            text += "Access violation.\n";
            break;

        case SIGABRT:
            text += "Signal: SIGABRT\n";
            break;

        case SIGFPE:
            text += "Signal: SIGFPE\n";
            break;

        case SIGILL:
            text += "Signal: SIGILL\n";
            break;

        default:
            text += "Unknown signal.\n";
            break;
    }

    showCrashDialog(text);

    exit(signal);
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

