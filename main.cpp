#include "snowmainwnd.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);
    const QString applicationName =
            QString::fromLocal8Bit("高通量多组学序列数据可视化浏览器 - 数据库初始化");
    QApplication app(argc, argv);
    QApplication::setApplicationName(applicationName);
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationDomain("xuefeng.space");
    SnowMainWnd w;
    w.setWindowTitle(applicationName);
    w.show();

    return app.exec();
}

