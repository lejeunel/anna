#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QTextStream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(MainWindow::tr("anna"));
    app.setWindowIcon(QIcon(":/images/appIcon.svg"));
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists()) {
        printf("Unable to set stylesheet, file not found\n");
    }
    else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(MainWindow::tr("[file]"), MainWindow::tr("Video file to open."));
    commandLineParser.process(QCoreApplication::arguments());
    MainWindow window;
    if (!commandLineParser.positionalArguments().isEmpty()
        && !window.loadFile(commandLineParser.positionalArguments().front())) {
        return -1;
    }
    window.show();

    //testing
    //window.loadFile("/home/patapouette/Desktop/sample.avi");
    return app.exec();
}
