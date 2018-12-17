#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QFileInfoList>
#include <QJsonArray>
#include <QHostAddress>
#include <QJsonObject>
#include <QTimer>
#include <QDataStream>

#define Ttime 1
#define Tdir  2

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
    void GetMessage();
    void SendMessage(QString);

    void on_pushButton_2_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);
    void Download_file();

    void Get_data(QDataStream*);
    void Get_dir(QDataStream*);
    void Get_end(QDataStream*);


    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket g_UdpSocket,s_UdpSocket;

    // 下载文件名
    QString download_file_name;
    // 下载文件的二进制格式
    QByteArray download_file_data;
    // 下载文件希望的下一个编号
    int want_count;

    // 上传文件名
    QString upload_file_name;
    // 上传文件希望的下一个编号
    int need_count;


    // 服务器ip地址
    QHostAddress remote_host;
    // 服务器端口
    int remote_port;

    // 定时器
    QTimer *timer;
};

#endif // MAINWINDOW_H
