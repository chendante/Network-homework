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
    //向客户端发送消息，表明连接建立成功
    QByteArray data("220 ready \r\n");
    QString on_connect = data.data();
    m_tcp->write(data);
    //将发送内容展示在主窗口
    on_connect = "S: " + on_connect;

    //将有消息请求的信号同getmessage()函数连接
    connect(m_tcp,SIGNAL(readyRead()),this,SLOT(getMessage()));
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

void mytcpsocket::savefile(QByteArray t_data)
{
    QFile file("log.txt");
    file.open((QIODevice::WriteOnly));
    QDataStream out(&file);
    out<<t_data;
    file.close();
}

void mytcpsocket::sendDir()
{
    QString FilePath = this->pp->GetFilePath();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDir dir(FilePath);
    dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QFileInfoList d_list = dir.entryInfoList();
    qDebug()<<d_list.size();
    //将d_list 中内容整理为JSON格式
    QJsonArray json_array;
    for (int i = 0; i < d_list.size(); i++)
    {
        QFileInfo fileInfo = d_list.at(i);
        QJsonObject file_json;
        file_json.insert("name",fileInfo.fileName());
        file_json.insert("time",fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
        file_json.insert("size",QString::number(fileInfo.size()));
        file_json.insert("path",fileInfo.absoluteFilePath());
        json_array.append(QJsonValue(file_json));
    }
    QJsonDocument json_doc;
    json_doc.setArray(json_array);
    out<<2<<json_doc.toJson(QJsonDocument::Compact);
    tcpSocket->write(data);
}
