#include "viewwidget.h"
#include "ui_viewwidget.h"
#include <QDebug>
#include <QTextCodec>
#include <QThread>
#include <QIODevice>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>
#include <QDateTime>
#include "selectfile.h"




viewwidget::viewwidget(QString roomid,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::viewwidget)
{
    ui->setupUi(this);
    connectstat = false;
    this->roomid = roomid;
    m_pClient = new QTcpSocket(this);
    m_pClient->connectToHost("openbarrage.douyutv.com",8601);
    connect(m_pClient,SIGNAL(readyRead()),this,SLOT(readmessage()));
    connect(m_pClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onerror(QAbstractSocket::SocketError)));
    connect(m_pClient,SIGNAL(disconnected()),this,SLOT(reconnect()));
    connect(m_pClient,SIGNAL(connected()),this,SLOT(haveconnect()));
    m_pTimer = new QTimer(this);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendheart()));

    m_accessManager = new QNetworkAccessManager(this);
    QObject::connect(m_accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

    QNetworkRequest request;
    QUrl httpurl = QUrl(QString("http://open.douyucdn.cn/api/RoomApi/room/%1").arg(roomid));
    //QUrl httpurl = QUrl(QString("http://www.baidu.com"));
    request.setUrl(httpurl);
    m_accessManager->get(request);

    ui->tabWidget->setTabsClosable(true);
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(on_closetab(int)));

    QTimer::singleShot(10000,this,SLOT(onReconnect()));



}
void viewwidget::haveconnect(){
    connectstat = true;
    QString mess1 = QString("type@=loginreq/roomid@=%1/").arg(roomid);
    QString mess2 = QString("type@=joingroup/rid@=%1/gid@=-9999/").arg(roomid);
    m_pClient->write(strToArray(mess1),strToArray(mess1).length());
    m_pClient->write(strToArray(mess2),strToArray(mess2).length());
    m_pTimer->start(45000);
    ui->textBrowser->append(QString::fromLocal8Bit("已成功连接到房间%1").arg(roomid));
}

void viewwidget::onReconnect(){
    if(!connectstat){
        m_pClient->abort();
        m_pClient->connectToHost("openbarrage.douyutv.com",8601);
        ui->textBrowser->append(QString::fromLocal8Bit("正在重连"));
    }
    QTimer::singleShot(10000,this,SLOT(onReconnect()));
}

void viewwidget::sendheart(){
    QString mess = QString("type@=mrkl/");
    m_pClient->write(strToArray(mess),strToArray(mess).length());
}

QByteArray viewwidget::strToArray(QString data){
    QByteArray array;
    int length = 9+ data.toUtf8().length();
    array.append((unsigned char)(0x000000ff & length));
    array.append((unsigned char)((0x0000ff00 & length)>>8));
    array.append((unsigned char)((0x00ff0000 & length)>>16));
    array.append((unsigned char)((0xff000000 & length)>>24));



    array.append((unsigned char)(0x000000ff & length));
    array.append((unsigned char)((0x0000ff00 & length)>>8));
    array.append((unsigned char)((0x00ff0000 & length)>>16));
    array.append((unsigned char)((0xff000000 & length)>>24));


    int i = 689;
    array.append((unsigned char)(0x00ff & i));
    array.append((unsigned char)((0xff00 & i)>>8));


    array.append((char)0x00);
    array.append((char)0x00);
    array.append(data.toUtf8());
    array.append((char)0x00);
    return array;
}

void viewwidget::reconnect(){
    connectstat = false;
    m_pTimer->stop();

    //m_pClient->abort();
    //m_pClient->connectToHost("openbarrage.douyutv.com",8601);
}

viewwidget::~viewwidget()
{
    delete ui;
    m_pTimer->stop();
    m_pClient->disconnectFromHost();
}
void viewwidget::readmessage(){


    QByteArray lendata = m_pClient->read(4);
    int addr = lendata[0] & 0x000000FF;
    addr |= ((lendata[1] << 8) & 0x0000FF00);
    addr |= ((lendata[2] << 16) & 0x00FF0000);
    addr |= ((lendata[3] << 24) & 0xFF000000);
    /*
    if(addr > 100000){
       // qDebug()<<addr;
        //qDebug()<<lendata.toHex();
        QByteArray alldata = m_pClient->readAll();
        QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
        QString utf8str = utf8codec->toUnicode(alldata.mid(0));
        //getMessageId(utf8str);
        qDebug()<<alldata.toHex();
        qDebug()<<utf8str;
    }else{
    */

    QByteArray alldata = m_pClient->read(addr);
    while(alldata.length() < addr){
        m_pClient->waitForReadyRead(-1);
        alldata += m_pClient->read(addr-alldata.length());
    }
    QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
    QString utf8str = utf8codec->toUnicode(alldata.mid(8));
    //ui->textBrowser->append(QString::fromLocal8Bit("server:")+utf8str);

    /*if(addr >2000){
    qDebug()<<"length: "<<addr;
    qDebug()<<"lengthhex: "<<lendata.toHex();
    qDebug()<<"alldatahex: "<<alldata.toHex();
    }
    */
    getMessageId(utf8str);
    //}


}
void viewwidget::onerror(QAbstractSocket::SocketError x){
    qDebug()<<"error"<<x;
    qDebug()<<m_pClient->state();
}

void viewwidget::getMessageId(QString message){
    QStringList lstmess = message.split("/");
    QMap<QString,QString> mapID;
    if(!lstmess.isEmpty()){
        QStringList type= lstmess.at(0).split("@=");
        mapID.insert(type[0],type[1]);
        if(mapID["type"] == QString("chatmsg")){
            foreach(QString mes,lstmess){
                QStringList newpage = mes.split("@=");
                if(!mapID.contains(newpage[0]) && newpage.count() > 1){
                    mapID.insert(newpage[0],newpage[1]);
                }
            }
            QString color;
            switch(mapID["col"].toInt()){
            case 2:color = "#436EEE";break;
            case 3:color = "#00CD00";break;
            case 4:color = "#EE7600";break;//EE82EE
            case 5:color = "#9400D3";break;//EE7942
            case 6:color = "#FF34B3";break;//FF00FF
            case 1:color = "#FF0000";break;//FF0000
            default:color = "#000000";break;
            }
            QString guizu;
            QString fans;
            switch(mapID["nl"].toInt()){
            case 1:guizu = QString::fromLocal8Bit("[骑士]");break;
            case 2:guizu = QString::fromLocal8Bit("[子爵]");;break;
            case 3:guizu = QString::fromLocal8Bit("[伯爵]");;break;
            case 4:guizu = QString::fromLocal8Bit("[公爵]");;break;
            case 5:guizu = QString::fromLocal8Bit("[国王]");;break;
            case 6:guizu = QString::fromLocal8Bit("[皇帝]");;break;
            case 7:guizu = QString::fromLocal8Bit("[游侠]");;break;
            default:guizu = QString::fromLocal8Bit("");;break;
            }
            if(!mapID["bnn"].isEmpty()){
               fans = QString("[%1(%2)]").arg(mapID["bnn"],mapID["bl"]);
            }
            QString level = QString("[%1]").arg(mapID["level"]);
            /*
            QString str = QString("<span style=\" color:#B23AEE\";\>%2</span>"
                                  "<span style=\" color:#EE799F\";\>%3</span>"
                                  "<span style=\" color:#00CD00\";\>%4</span>"
                                  "<span style=\" color:#6495ED\">%5 : </span>"
                                  "<span style=\" color:%6;\">%7</span>")
                    .arg(guizu,fans,level,mapID["nn"],color,mapID["txt"]);
            ui->textBrowser->append(str);
            */
            //qDebug()<<message;
            //qDebug()<<ui->textBrowser->loadResource(2,QUrl("https://shark.douyucdn.cn/app/douyu/res/page/room-normal/face/dy101.png?v=20170626")).toByteArray();
            if(lstAttention.keys().contains(mapID["nn"])){
                QString str = QString("<span style=\" color:#000000;\">%7</span>"
                                      "<span style=\" color:#9400D3;background-color:#FFE4B5\";>%1</span>"
                                      "<span style=\" color:#FF34B3;background-color:#FFE4B5\";>%2</span>"
                                      "<span style=\" color:#00CD00;background-color:#FFE4B5\">%3</span>"
                                      "<span style=\" color:#436EEE;background-color:#FFE4B5\">%4 : </span>"
                                      "<span style=\" color:%5;background-color:#FFE4B5\">%6</span>"
                                      "<span style=\" color:#000000;\">%7</span>")
                        .arg(guizu,fans,level,mapID["nn"],color,mapID["txt"],QString::fromLocal8Bit("【")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+QString::fromLocal8Bit("】"));
                ui->textBrowser->append(str);
                lstAttention.value(mapID["nn"])->append(str);
            }else{
                QString str = QString("<span style=\" color:#000000;\">%7</span>"
                                      "<span style=\" color:#9400D3\";\>%1</span>"
                                      "<span style=\" color:#FF34B3\";\>%2</span>"
                                      "<span style=\" color:#00CD00\";\>%3</span>"
                                      "<span style=\" color:#436EEE\">%4 : </span>"
                                      "<span style=\" color:%5;\">%6</span>"
                                      "")
                        .arg(guizu,fans,level,mapID["nn"],color,mapID["txt"],QString::fromLocal8Bit("【")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+QString::fromLocal8Bit("】"));
                ui->textBrowser->append(str);
            }
        }else if(mapID["type"] == QString("uenter")){
            foreach(QString mes,lstmess){
                QStringList newpage = mes.split("@=");
                if(!mapID.contains(newpage[0]) && newpage.count() > 1){
                    mapID.insert(newpage[0],newpage[1]);
                }
            }
            QString guizu;
            QString fans;
            switch(mapID["nl"].toInt()){
            case 1:guizu = QString::fromLocal8Bit("[骑士]");break;
            case 2:guizu = QString::fromLocal8Bit("[子爵]");;break;
            case 3:guizu = QString::fromLocal8Bit("[伯爵]");;break;
            case 4:guizu = QString::fromLocal8Bit("[公爵]");;break;
            case 5:guizu = QString::fromLocal8Bit("[国王]");;break;
            case 6:guizu = QString::fromLocal8Bit("[皇帝]");;break;
            case 7:guizu = QString::fromLocal8Bit("[游侠]");;break;
            default:guizu = QString::fromLocal8Bit("");;break;
            }
            if(!mapID["bnn"].isEmpty()){
               fans = QString("[%1(%2)]").arg(mapID["bnn"],mapID["bl"]);
            }
            QString level = QString("[%1]").arg(mapID["level"]);
            /*
            QString str = QString("<span style=\"color:#ffffff ;background-color:#FF00FF\">%2</span>"
                                  "<span style=\"color:#ffffff ;background-color:#FF00FF\">%3</span>"
                                  "<span style=\"color:#ffffff ;background-color:#FF00FF\">%4</span>"
                                  "<span style=\"color:#ffffff ;background-color:#FF00FF\">%5</span>"
                                  "<span style=\"color:#ffffff ;background-color:#FF00FF\">%6</span>")
                    .arg(guizu,fans,level,mapID["nn"],QString::fromLocal8Bit("欢迎来到房间"));
            ui->textBrowser->append(str);
            */
            //qDebug()<<message;
            //qDebug()<<ui->textBrowser->loadResource(2,QUrl("https://shark.douyucdn.cn/app/douyu/res/page/room-normal/face/dy101.png?v=20170626")).toByteArray();
            if(lstAttention.keys().contains(mapID["nn"])){
                QString str = QString("<span style=\" color:#000000;\">%6</span>"
                                      "<span style=\"color:#000000 ;background-color:#FFE4B5\">%1</span>"
                                      "<span style=\"color:#000000 ;background-color:#FFE4B5\">%2</span>"
                                      "<span style=\"color:#000000 ;background-color:#FFE4B5\">%3</span>"
                                      "<span style=\"color:#000000 ;background-color:#FFE4B5\">%4</span>"
                                      "<span style=\"color:#000000 ;background-color:#FFE4B5\">%5</span>"
                                      "")
                        .arg(guizu,fans,level,mapID["nn"],QString::fromLocal8Bit("欢迎来到房间"),QString::fromLocal8Bit("【")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+QString::fromLocal8Bit("】"));
                ui->textBrowser->append(str);
                lstAttention.value(mapID["nn"])->append(str);
            }else{
                QString str = QString("<span style=\" color:#000000;\">%6</span>"
                                      "<span style=\"color:#ffffff ;background-color:#FF00FF\">%1</span>"
                                      "<span style=\"color:#ffffff ;background-color:#FF00FF\">%2</span>"
                                      "<span style=\"color:#ffffff ;background-color:#FF00FF\">%3</span>"
                                      "<span style=\"color:#ffffff ;background-color:#FF00FF\">%4</span>"
                                      "<span style=\"color:#ffffff ;background-color:#FF00FF\">%5</span>"
                                      "")
                        .arg(guizu,fans,level,mapID["nn"],QString::fromLocal8Bit("欢迎来到房间"),QString::fromLocal8Bit("【")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+QString::fromLocal8Bit("】"));
                ui->textBrowser->append(str);
            }
        }
        //ui->textBrowser->append(mapID["type"]);
    }
}

void viewwidget::finishedSlot(QNetworkReply* reply){
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        //QString string = QString::fromUtf8(bytes);
        upInfo info = analyze(bytes);
        ui->listWidget->addItem(QString::fromLocal8Bit("房间号：%1").arg(info.roomid));
        ui->listWidget->addItem(QString::fromLocal8Bit("房间名：%1").arg(info.roomname));
        ui->listWidget->addItem(QString::fromLocal8Bit("昵称：%1").arg(info.ownername));
        ui->listWidget->addItem(QString::fromLocal8Bit("状态：%1").arg((info.roomstatus == "1")?QString::fromLocal8Bit("在线"):QString::fromLocal8Bit("不在线")));
        ui->listWidget->addItem(QString::fromLocal8Bit("上次时间：%1").arg(info.starttime));
        ui->listWidget->addItem(QString::fromLocal8Bit("热度：%1").arg(info.hn));
        ui->listWidget->addItem(QString::fromLocal8Bit("关注：%1").arg(info.fansnum));


        //cout<<std::string(aaa.data());
    }
    else
    {
        qDebug()<<"handle errors here";
        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
        qDebug( "found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
        qDebug(qPrintable(reply->errorString()));
    }
    reply->deleteLater();
}
upInfo viewwidget::analyze(QByteArray byte_array){
    upInfo info;
    QJsonParseError jsonError;
    QJsonDocument jsonDoucment = QJsonDocument::fromJson(byte_array, &jsonError);
    if(jsonError.error == QJsonParseError::NoError)
    {
        if(jsonDoucment.isObject())
        {
            QJsonObject jsonObject = jsonDoucment.object();
            if(jsonObject.contains("data")){
                QJsonValue value = jsonObject.take("data");
                QJsonObject jsonObj = value.toObject();
                info.roomid = jsonObj.take("room_id").toString();
                info.roomname = jsonObj.take("room_name").toString();
                info.roomstatus = jsonObj.take("room_status").toString();
                info.starttime = jsonObj.take("start_time").toString();
                info.ownername = jsonObj.take("owner_name").toString();
                info.hn = QString::number(jsonObj.take("hn").toInt());
                info.fansnum = jsonObj.take("fans_num").toString();
            }
        }
    }
    return info;
}

void viewwidget::on_pushButton_clicked()
{
    if(!ui->lineEdit->text().isEmpty() && !lstAttention.contains(ui->lineEdit->text())){
        QTextBrowser *text = new QTextBrowser(this);
        lstAttention.insert(ui->lineEdit->text(),text);
        ui->tabWidget->addTab(text,ui->lineEdit->text());
    }else{
        QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("空或者已添加"));
    }

}

void viewwidget::on_closetab(int index){
    lstAttention.value(ui->tabWidget->tabText(index))->deleteLater();
    lstAttention.remove(ui->tabWidget->tabText(index));
    ui->tabWidget->removeTab(index);
}
void viewwidget::stringToHtmlFilter(QString &str)
{
    //注意这几行代码的顺序不能乱，否则会造成多次替换
    str.replace("&","&amp;");
    str.replace(">","&gt;");
    str.replace("<","&lt;");
    str.replace("\"","&quot;");
    str.replace("\'","&#39;");
    str.replace(" ","&nbsp;");
    str.replace("\n","<br>");
    str.replace("\r","<br>");
}


void viewwidget::on_pushButton_2_clicked()
{
    QString html = ui->textBrowser->toHtml();
    SelectFile file(html);
    file.exec();
    if(file.result() == QDialog::Accepted){
        QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("导出成功"));
    }else{
        QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("导出失败"));
    }
}
