#include "login.h"
#include <QApplication>
#include <QDebug>

// Custom message handler to suppress messages
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    // Do nothing; suppress all messages
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Install the custom message handler
    qInstallMessageHandler(customMessageHandler);

    login w;
    w.show();
    return a.exec();
}
