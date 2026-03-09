#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Установка информации о приложении
    app.setApplicationName("Radxa ROCK 4D Converter");
    app.setOrganizationName("Radxa");
    app.setApplicationVersion("3.1.0");
    app.setOrganizationDomain("radxa.com");
    
    // Установка стиля
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Загрузка темной темы (опционально)
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}