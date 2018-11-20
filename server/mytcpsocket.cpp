#include"mytcpsocket.h"
#include<QByteArray>
#include<QDataStream>
#include<QDebug>

mytcpsocket::mytcpsocket(QTcpSocket* tcp)
{
    m_tcp = tcp;
    qDebug()<<"new tcp"<<endl;
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
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
