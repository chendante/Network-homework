#include"mytcpsocket.h"
#include<QByteArray>
#include<QDataStream>
#include<QDebug>
#include<QHostAddress>
#include<QStringList>

mytcpsocket::mytcpsocket(QTcpSocket* tcp)
{
    m_tcp = tcp;
    qDebug()<<"new tcp"<<endl;
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    QString on_connect("220 simple mail server ready for mail");
    out<<on_connect;
    m_tcp->write(data);
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
    emit sendString(on_connect);
}

void mytcpsocket::getMessage()
{
    QByteArray data = m_tcp->readLine();
    qDebug()<<data;
    QString str(data);
    deal(str);
}

void mytcpsocket::deal(QString str)
{
    QStringList str_list = str.split(" ");
    QString res;
    if(str_list[0]=="HELO")
    {
        qDebug()<<111;
        res = "250 OK ";
        res += this->m_tcp->peerAddress().toString();
    }
    emit sendString(res);
}
