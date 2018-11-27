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
    this->pp->GetMessage("*** 建立连接\r\n");
    QByteArray data("220 ready \r\n");
    QString on_connect = data.data();
    m_tcp->write(data);
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
    on_connect = "S: " + on_connect;
    this->pp->GetMessage(on_connect);
}

//获取客户端发送信息
void mytcpsocket::getMessage()
{
    //获取客户端发送的信息
    QByteArray data = m_tcp->readAll();
    //对信息进行处理
    this->dealMessage(data);
}

//判断客户端发送内容种类，并作相应处理
void mytcpsocket::dealMessage(QByteArray data)
{
    //将客户发送内容转为字符串
    QString str(data);
    //处理该字符串，获取状态信息
    deal(str);
    //当状态码为0，代表客户端请求断开链接，则将链接断开，并开始处理报文
    if (status == 0) {
        this->savefile(m_data);
        this->m_tcp->disconnect();
        this->dealContent();
    }
    //当状态码为6，代表当前发送内容为报文，将所有报文叠加，等待最后处理
    if (status == 6)
    {
        m_data.append(data);
    }
}

//用于处理客户端发送的命令行
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
    else if(str.left(9) == "MAIL FROM" && status >= 2)
    {
        status = 3;
        res = "250 OK \r\n";
    }
    else if (str.left(7) == "RCPT TO" && status >= 3) {
        status = 4;
        res = "250 OK \r\n";
    }
    else if (str_list[0] == "DATA" && status >= 4) {
        status = 5;
        res = "354 Enter mail, end with \".\" on a line by itself\r\n";
    }
    else if( str_list[0] == "QUIT")
    {
        status = 0;
        res = "221 Bye";
    }
    else if (status >= 5)
    {
        status = 6;
        qDebug()<<str.right(3);
        if(str.right(3) == ".\r\n")
        {
            res = "250 message sent \r\n";
        }

    }
    else
    {
        res = "502 error \r\n";
    }

    if(status != 6)
    {
        str = "C: " + str;
        this->pp->GetMessage(str);
    }
    if(!res.isEmpty())
    {
        this->m_tcp->write(res);
        res = "S: "+res;
        this->pp->GetMessage(res);
    }
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

    //用正则表达式判断发送内容中是否包含图片：
    QRegExp reg_image("Content-Type: image");
    //图片信息的起始位置：
    int pos_image = 0;
    //匹配存在的每个图片
    while((pos_image = reg_image.indexIn(data,pos_image)) != -1)
    {
        qDebug()<<"pos_iamge: "<<pos_image;
        pos_image += reg_image.matchedLength();
        qDebug()<<"have picture";
        //用于获取图片信息的正则表达式
        QRegExp reg_in_image("\r\n\r\n.*\r\n\r\n");
        //正则匹配规则设置为最小匹配
        reg_in_image.setMinimal(true);
        //用正则表达式分析图片头的内容，获取到图片名称
        QRegExp reg_name("name=\"(.*)\"");
        //正则匹配规则设置为最小匹配
        reg_name.setMinimal(true);
        reg_name.indexIn(data,pos_image);
        QString name = reg_name.cap(1);
        // 图片信息的HTML代码
        QByteArray path = QString("<img src=\"%1\"/>\r\n").arg(name).toLatin1();

        QByteArray image_data;
        //图片信息字符串的起始位置和长度
        int start = reg_in_image.indexIn(data,pos_image);
        int len = reg_in_image.matchedLength();

        //图片信息的内容
        image_data = reg_in_image.cap().toLatin1();

        //将图片信息那一长串字符串替换成对应HTML
        data.replace(start,len,path);

        //处理图片信息：
        //邮件传送的图片编码方式为base64,依照这种方式将其解码
        image_data = mytcpsocket::frombase64(image_data);
        //将信息保存为文件
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

//将base64编码格式的ASCII码转码为二进制格式的信息
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
