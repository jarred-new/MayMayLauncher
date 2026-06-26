#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QString>

class CrashHandler
{
public:
    CrashHandler();

public:
    static void install();
private:
    static void signalHandler(int signal);
    static void showCrashDialog(const QString &text);
};

#endif // CRASHHANDLER_H
