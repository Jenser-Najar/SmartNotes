#include "MainWindow.h"
#include "ThemeManager.h"
#include <QApplication>
#include <QIcon>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    ThemeManager::applyDarkTheme(&app);
    app.setWindowIcon(QIcon("qrc:/icons/task_manager_icon.ico"));

    MainWindow window;
    window.resize(920, 640);
    window.show();

    return app.exec();
}
