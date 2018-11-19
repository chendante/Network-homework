#include"mytcpsocket.h"
#include<QString>

mytcpsocket::mytcpsocket(QWidget *parent, qintptr p):QTcpSocket(parent)
{
    this->setSocketDescriptor(p);
    this->on_connected();
    this->connect(this,mytcpsocket::disconnect,this,mytcpsocket::on_discon);
}

void mytcpsocket::on_connected()
{
    QByteArray data;
    QDataStream out(&arr,QIODevice::WriteOnly);
    out<<QString("220 MYSMTPSERVER");
    this->write(data);
}
