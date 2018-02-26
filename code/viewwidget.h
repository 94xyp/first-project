#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostInfo>
#include <QTimer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebEngineView>
#include <QTextBrowser>

class QTextBrowser;
namespace Ui {
class viewwidget;
}

struct upInfo{
    QString roomid;
    QString roomname;
    QString roomstatus;
    QString starttime;
    QString ownername;
    QString hn;
    QString fansnum;
};



class viewwidget : public QWidget
{
    Q_OBJECT

public:
    explicit viewwidget(QString,QWidget *parent = 0);
    ~viewwidget();

    QByteArray strToArray(QString);
    upInfo analyze(QByteArray);
    void stringToHtmlFilter(QString &str);


public slots:
    void readmessage();
    void onerror(QAbstractSocket::SocketError);
    void haveconnect();
    void reconnect();
    void sendheart();
    void getMessageId(QString);
    void finishedSlot(QNetworkReply*);
    void on_closetab(int index);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void onReconnect();

private:
    Ui::viewwidget *ui;
    QTcpSocket* m_pClient;
    QTimer* m_pTimer;
    QString roomid;
    QNetworkAccessManager *m_accessManager;
    upInfo m_up;
    QMap<QString,QTextBrowser*> lstAttention;
    bool connectstat;
};

#endif // VIEWWIDGET_H
