#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include<QTcpSocket>
#include<QWidget>
#include<QString>

class mytcpsocket: public QObject
{
    Q_OBJECT
public:
    mytcpsocket(QTcpSocket*);
    QTcpSocket* m_tcp;

signals:
    void sendString(QString);

public slots:
    void getMessage();

};

#endif // MYTCPSOCKET_H
