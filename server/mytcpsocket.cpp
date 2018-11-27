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
#include<QMap>

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
        this->savefile(m_data);
        this->m_tcp->disconnect();
        this->dealContent();
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
//        res = "250 message sent \r\n";
        res = "502 message sent \r\n";
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
    QByteArray data(m_data);
    QString res;
    QRegExp reg_image("Content-Type: image");
    int pos_image = 0;
    while((pos_image = reg_image.indexIn(data,pos_image)) != -1)
    {
        qDebug()<<"pos_iamge: "<<pos_image;
        pos_image += reg_image.matchedLength();
        qDebug()<<"have picture";
        QRegExp reg_in_image("\r\n\r\n.*\r\n\r\n");
        QRegExp reg_name("name=\"(.*)\"");
        reg_name.setMinimal(true);
        reg_name.indexIn(data,pos_image);
        QString name = reg_name.cap(1);
        qDebug()<<"image_name: "<<name;
        QByteArray path = QString("<img src=\"%1\"/>\r\n").arg(name).toLatin1();
        reg_in_image.setMinimal(true);

        QByteArray image_data;

        int k = reg_in_image.indexIn(data,pos_image);
        int len = reg_in_image.matchedLength();
        if(k != -1)
        {
            image_data = reg_in_image.cap().toLatin1();
        }
        qDebug()<<"K: "<<k;
        qDebug()<<"len: "<<len;
        data.replace(k,len,path);
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

    QRegExp reg_content("(<html>.*</html>)|(<img.*/>)");
    reg_content.setMinimal(true);
    int pos_content = 0;
    while( (pos_content = reg_content.indexIn(data,pos_content)) != -1)
    {
        qDebug()<<"pos_content: "<<pos_content;
        pos_content += reg_content.matchedLength();
        res.append(reg_content.cap());
    }
    res.replace("=0A","");
    qDebug()<<"content: "<<res;

    this->pp->GetContent(res);
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
