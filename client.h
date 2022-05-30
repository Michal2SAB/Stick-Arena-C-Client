#ifndef CLIENT_H
#define CLIENT_H

#include "mainwindow.h"
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QNetworkProxy>

class Client:public QObject{
    Q_OBJECT

public:
    explicit Client(class MainWindow *GUIHandle, QString Username, QString Password, QString Server, QObject *Parent = 0);
    QString sendPacket(QString Packet, bool Receive = false);
    void connectToServer(QString Server, QString Username, QString Password);
    void sendPrivateMessage(QString UserID, QString Message);
    void sendPublicMessage(QString Message);
    ~Client();

signals:
    void gotSpinnerList(QList<QString> SpinnerList);

private slots:
    void handleConnection();
    void startKeepAlive();
    void stopKeepAlive();
    void startRoomList();
    void stopRoomList();

private:
    QTcpSocket *SocketConn;
    QNetworkProxy ClientProxy;
    class MainWindow *GUIHandle;

    QByteArray Buffer;
    char Buf;

    QString MyUserID;
    QString MyUsername;
    QString MyCred;

    QList<QString> Login = {"0a", "02Z900_", "03_"};

    QHash<QString, QString> IDToUsername;
    QHash<QString, QString> UsernameToID;
    QHash<QString, QString> UserRGB;
    QHash<QString, QString> UserAccess;
    QHash<QString, QString> UserLabpass;
    QHash<QString, QString> UserKills;
    
    // server ips are different now, dont care to check and update
    QList<QString> ServerName = {"2D Central", "Paper Thin City", "Fine Line Island", "U of SA", "Amsterdam", "Mobius Metro", "Cartesian"};
    QList<QString> ServerIP = {"ballistick5.xgenstudios.com:1138", "74.86.43.8:1138", "198.58.106.101:1138", "69.164.207.72:1138", "139.162.151.57:1138", "45.56.72.83:1138", "198.58.106.101:1139"};
};

#endif // CLIENT_H
