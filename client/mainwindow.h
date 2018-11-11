#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

#define Ttime 1

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

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket g_UdpSocket,s_UdpSocket;
};

#endif // MAINWINDOW_H
