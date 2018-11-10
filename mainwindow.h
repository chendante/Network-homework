#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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
    void SendMessage();

private:
    Ui::MainWindow *ui;
    QUdpSocket g_UdpSocket,s_UdpSocket;
};

#endif // MAINWINDOW_H
