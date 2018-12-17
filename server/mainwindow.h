#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include "myftp.h"
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void SendFile(QString file_name, int want_count);
    QString GetFilePath();

private slots:
    void SendMessage();
    void GetMessage();

    void SendDir();

    void new_connect();
    
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket UdpSocket;
    QVector<myftp*> client_list;
};

#endif // MAINWINDOW_H
