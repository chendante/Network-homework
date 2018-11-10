#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>

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
    void SendMessage();
    void GetMessage();
    
private:
    Ui::MainWindow *ui;
    QUdpSocket m_UdpSocket,g_UdpSocket;
    QTimer m_timer;
};

#endif // MAINWINDOW_H
