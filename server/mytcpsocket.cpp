#include"mytcpsocket.h"
#include<QByteArray>
#include<QDataStream>
#include<QDebug>

mytcpsocket::mytcpsocket(QTcpSocket* tcp)
{
    m_tcp = tcp;
    qDebug()<<"new tcp"<<endl;
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    QString on_connect("220 hello");
    out<<on_connect;
    m_tcp->write(data);
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
    emit sendString(on_connect);
}

void mytcpsocket::getMessage()
{
    QByteArray data = m_tcp->readAll();
    QDataStream in(&data,QIODevice::ReadWrite);
    QString str;
    int a;
    in>>a>>str;
    emit sendString(str);
}
