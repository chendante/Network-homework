#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>

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

private slots:
    void SendMessage();
    void GetMessage();
    void SendDir();
    
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket m_UdpSocket,g_UdpSocket;
};

#endif // MAINWINDOW_H
