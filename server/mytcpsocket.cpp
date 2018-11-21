#include"mytcpsocket.h"
#include<QByteArray>
#include<QDataStream>
#include<QDebug>
#include<QHostAddress>
#include<QStringList>
#include<QTimer>

mytcpsocket::mytcpsocket(QTcpSocket* tcp,MainWindow *p)
{
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(reSend()));
    m_tcp = tcp;
    this->pp = p;
    timer->start(10);
    qDebug()<<"new connect"<<endl;
    QByteArray data("220 ready");
    QString on_connect = data.data();
    m_tcp->write(data);
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
    this->pp->GetMessage(on_connect);
}

void mytcpsocket::reSend()
{
    QByteArray data("220 ready");
    this->m_tcp->write(data);
}

void mytcpsocket::getMessage()
{
    QByteArray data = m_tcp->readAll();
    qDebug()<<111;
    qDebug()<<data;
    QString str(data);
    deal(str);
}

void mytcpsocket::deal(QString str)
{
    QStringList str_list = str.split(" ");
    QByteArray res;
    if(str_list[0]=="HELO")
    {
        qDebug()<<111;
        res = "250 OK ";
        res += this->m_tcp->peerAddress().toString().toLatin1();
        this->m_tcp->write(res);
    }
    this->pp->GetMessage(res);
}
