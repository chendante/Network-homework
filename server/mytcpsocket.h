#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include<QTcpSocket>
#include<QWidget>
#include<QString>
#include<QByteArray>
#include"mainwindow.h"

class mytcpsocket: public QObject
{
    Q_OBJECT
public:
    mytcpsocket(QTcpSocket*,MainWindow*);
    QTcpSocket* m_tcp;
    MainWindow *pp;
    int status;
    QByteArray m_data;
    QByteArray frombase64(const QByteArray&);


signals:
    void sendString(QString);

public slots:
    void getMessage();
    void deal(QString);
    void dealMessage(QByteArray);
    void dealContent();
    void savefile(QByteArray);

};

#endif // MYTCPSOCKET_H
