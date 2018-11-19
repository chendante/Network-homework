#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QWidget>

class mytcpsocket:public QTcpSocket
{
public:
    mytcpsocket(QWidget* parent,qintptr p);
private slots:
    void on_discon();

public:
    void on_connected();
};

#endif // MYTCPSOCKET_H
