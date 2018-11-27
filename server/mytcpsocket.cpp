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
#include<QRegExp>

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

void mytcpsocket::getMessage()
{
    QByteArray data = m_tcp->readAll();
    qDebug()<<"debug:get message";
    this->dealMessage(data);
}

void mytcpsocket::dealMessage(QByteArray data)
{
    QString str(data);
    deal(str);
    if (status == 0) {
        this->disconnect();
        this->dealContent();
        this->pp->GetContent(m_data);
        this->savefile(m_data);
    }
    if (status == 6)
    {
        m_data.append(data);
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
        res = "250 message sent \r\n";
    }
    else
    {
        res = "502 error \r\n";
    }
    this->m_tcp->write(res);
    if(status != 6)
    {
        str = "C: " + str;
        this->pp->GetMessage(str);
    }
    res = "S: "+res;
    this->pp->GetMessage(res);
}

void mytcpsocket::savefile(QByteArray t_data)
{
    QFile file("log.txt");
    file.open((QIODevice::WriteOnly));
    QDataStream out(&file);
    out<<t_data;
    file.close();
}

void mytcpsocket::dealContent()
{
    QString str(m_data);
    QByteArray data(m_data);
    if(str.indexOf("Content-Type: image") != -1)
    {
        qDebug()<<"have picture";
        QRegExp reg("Content-Type: image");
        QRegExp reg2("\r\n\r\n.*\r\n\r\n");
//        QRegExp reg3("Content")
        QString name = "yy.png";
        QByteArray path = QString("<img src=\"%1\"/>\r\n").arg(name).toLatin1();
        reg2.setMinimal(true);

        QByteArray image_data(data);

        int k = reg2.indexIn(image_data,reg.indexIn(data));
        int len = reg2.matchedLength();
        if(k != -1)
        {
            image_data = reg2.cap().toLatin1();
        }
        qDebug()<<"K: "<<k;
        this->m_data.replace(k,len,path);
        image_data.replace("\r\n","");

        image_data = mytcpsocket::frombase64(image_data);
        QBuffer buffer(&image_data);
        buffer.open(QIODevice::ReadOnly);
        QImageReader reader(&buffer,"PNG");
        QImage img = reader.read();

        if(!img.isNull())
        {
            img.save(name, "PNG", 100);
        }
    }
}

QByteArray mytcpsocket::frombase64(const QByteArray &data)
{
    unsigned int buf = 0;
        int nbits = 0;
        QByteArray res((data.size() * 3) / 4, Qt::Uninitialized);
        int offset = 0;
        for (int i = 0; i < data.size(); ++i) {
            int ch = data.at(i);
            int d;
            if (ch >= 'A' && ch <= 'Z')
                d = ch - 'A';
            else if (ch >= 'a' && ch <= 'z')
                d = ch - 'a' + 26;
            else if (ch >= '0' && ch <= '9')
                d = ch - '0' + 52;
            else if (ch == '+' )
                d = 62;
            else if (ch == '/' )
                d = 63;
            else
                d = -1;
            if (d != -1) {
                buf = (buf << 6) | d;
                nbits += 6;
                if (nbits >= 8) {
                    nbits -= 8;
                    res[offset++] = buf >> nbits;
                    buf &= (1 << nbits) - 1;
                }
            }
        }
        res.truncate(offset);
        return res;
}
