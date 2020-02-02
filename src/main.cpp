#include <QApplication>
#include <QSurfaceFormat>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("servo");
    app.setApplicationVersion("0.1");

    MainWindow w;
    w.show();

    return app.exec();
}
