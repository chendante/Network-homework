#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QFileInfoList>
#include <QJsonArray>
#include <QJsonObject>

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
    void SendMessage(int);

    void on_pushButton_2_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);
    void Download_file(QString);

private:
    Ui::MainWindow *ui;
    QUdpSocket g_UdpSocket,s_UdpSocket;
    QString download_file_name;
    qint64 download_file_size;
    QByteArray download_file_data;
    int want_count;
};

#endif // MAINWINDOW_H
