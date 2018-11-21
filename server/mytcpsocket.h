#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include<QTcpSocket>
#include<QWidget>
#include<QString>
#include"mainwindow.h"

class mytcpsocket: public QObject
{
    Q_OBJECT
public:
    mytcpsocket(QTcpSocket*,MainWindow*);
    QTcpSocket* m_tcp;
    MainWindow *pp;


signals:
    void sendString(QString);

public slots:
    void getMessage();
    void deal(QString);
    void reSend();

};

#endif // MYTCPSOCKET_H
