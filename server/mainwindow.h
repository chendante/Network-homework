#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTcpServer>
#include <QString>
#include "myserver.h"
#include "mytcpsocket.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void NewConnect();
    void SendMessage();
    void GetMessage();
    void SendDir();

    void addString(QString);
    
private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
};

#endif // MAINWINDOW_H
