#include "myftp.h"

myftp::myftp(QHostAddress other_host, int other_port, MainWindow *parent)
{
    this->pp = parent;
    this->client_host = other_host;
    this->client_port = other_port;
}

bool myftp::Is_equal(QHostAddress other_host, int other_port)
{
    if(this->client_host.isEqual(other_host)&&other_port==client_port)
    {
        return true;
    }
    else{
        return false;
    }
}

// 发送信息
void myftp::SendMessage(QByteArray data)
{
    UdpSocket.writeDatagram(data,client_host, client_port);
}

void myftp::SendDir()
{
    QString file_path = this->pp->GetFilePath();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDir dir(file_path);
    // 当填写目录不存在时，不发送任何内容
    if(!dir.exists())
    {
        this->ui->textEdit->append("wrong path\n");
        return;
    }
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
    out<<QString("send dir")<<json_doc.toJson(QJsonDocument::Compact);
    UdpSocket.writeDatagram(data,client_host, client_port);
}
