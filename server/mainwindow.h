#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTcpServer>

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
//    void SendMessage();
    void GetMessage(QTcpSocket*);
//    void SendDir();
    
private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
};

#endif // MAINWINDOW_H
