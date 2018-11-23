#include"mytcpsocket.h"
#include<QByteArray>
#include<QDataStream>
#include<QDebug>
#include<QHostAddress>
#include<QStringList>
#include<QTimer>
#include<QImage>
#include<QImageReader>
#include<QPixmap>
#include<QBuffer>
#include<QFile>

mytcpsocket::mytcpsocket(QTcpSocket* tcp,MainWindow *p)
{
    status = 1;
    m_tcp = tcp;
    this->pp = p;
    QByteArray data("220 ready \r\n");
    QString on_connect = data.data();
    m_tcp->write(data);
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
    on_connect = "S: " + on_connect;
    this->pp->GetMessage(on_connect);
}

void mytcpsocket::reSend()
{
    QByteArray data("220 ready \r\n");
    this->m_tcp->write(data);
}

void mytcpsocket::getMessage()
{
    QByteArray data = m_tcp->readAll();
    qDebug()<<"debug:get message";
    QDataStream out(&data,QIODevice::ReadOnly);
    QString db;
    out<<db;
    qDebug()<<db;
    this->dealMessage(data);
}

void mytcpsocket::dealMessage(QByteArray data)
{
    QString str(data);
    if(str.indexOf("Content-Type: image") != -1)
    {
        qDebug()<<"have picture";
        int i = data.indexOf("Content-ID:");
        qDebug()<<i;
        data.remove(0,i+60);
        QByteArray image_data = QByteArray::fromBase64(data);
//        QBuffer buffer(&image_data);
        QFile file("file.png");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out<<image_data;
        file.close();
//        buffer.open(QIODevice::ReadOnly);
//        QImageReader reader(&buffer,"PNG");
//        QImage img = reader.read();

//        QPixmap image_res;
//        if(image_res.loadFromData(image_data.data()))
//        {
//            qDebug()<<"read picture";
//            image_res.save("save.png");
//        }
    }
    deal(str);
    if (status == 0) {
        this->disconnect();
    }
}

void mytcpsocket::deal(QString str)
{
    QStringList str_list = str.split(" ");
    QByteArray res;
    if(str_list[0]=="HELO" || str_list[0]=="EHLO")
    {
        status = 2;
        res = "250 OK ";
        res += this->m_tcp->peerAddress().toString().toLatin1();
        res += "  \r\n";
    }
    else if(str.left(9) == "MAIL FROM" && status == 2)
    {
        status++;
        res = "250 OK \r\n";
    }
    else if (str.left(7) == "RCPT TO" && status >= 3) {
        status = 4;
        res = "250 OK \r\n";
    }
    else if (str.left(4) == "DATA" && status >= 4) {
        status = 5;
        res = "354 OK \r\n";
    }
    else if( str.left(4) == "QUIT")
    {
        status = 0;
        res = "221 Bye";
    }
    else if (status >= 5)
    {
        status = 6;
        res = "502 message sent \r\n";
    }
    else
    {
        res = "502 error \r\n";
    }
    this->m_tcp->write(res);
    if(status == 6)
    {
        this->pp->GetContent(str);
    }
    else
    {
        str = "C: " + str;
        this->pp->GetMessage(str);
    }
    res = "S: "+res;
    this->pp->GetMessage(res);
}
