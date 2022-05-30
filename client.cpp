#include "client.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

Client::Client(MainWindow *GUIHandle, QString Username, QString Password, QString Server, QObject *Parent):QObject(Parent){
    this->GUIHandle = GUIHandle;
    GUIHandle->shouldReconnect(true);
    
    // connect through a tor socks proxy
    ClientProxy.setType(QNetworkProxy::Socks5Proxy);
    ClientProxy.setHostName("127.0.0.1");
    ClientProxy.setPort(9150);

    connectToServer(Server, Username, Password);
}

QString Client::sendPacket(QString Packet, bool Receive){
    if (SocketConn->isOpen()){
        SocketConn->write(Packet.toUtf8() + '\0');

        if (Receive){
            SocketConn->waitForReadyRead();

            QString Data = SocketConn->read(4096);
            return Data.remove("\0");
        }
    }

    return "";
}

void Client::startKeepAlive(){
    startRoomList();
    QTimer::singleShot(20000, this, &Client::startKeepAlive);

    sendPacket("0");
}

void Client::stopKeepAlive(){
    stopRoomList();
}

void Client::startRoomList(){
    QTimer::singleShot(3000, this, &Client::startRoomList);

    sendPacket("01");
}

void Client::stopRoomList(){

}

void Client::sendPrivateMessage(QString UserID, QString Message){
    if (IDToUsername.contains(UserID)){
        QString Username = IDToUsername[UserID];

        sendPacket("00" + UserID + "P" + Message);

        GUIHandle->appendUserMessage(Username + ";" + UserRGB[Username] + ";F;" + UserAccess[Username], Message);
    }
}

void Client::sendPublicMessage(QString Message){
    sendPacket("9" + Message);
}

void Client::connectToServer(QString Server, QString Username, QString Password){
    SocketConn = new QTcpSocket(this);
    SocketConn->setSocketOption(QTcpSocket::LowDelayOption, true);
    SocketConn->setProxy(ClientProxy);
    SocketConn->connectToHost(Server.split(":")[0], Server.split(":")[1].toInt());

    connect(SocketConn, &QTcpSocket::connected, [=](){
        if (sendPacket("08HxO9TdCC62Nwln1P", true) == "08"){
            QString Auth = sendPacket("09" + Username + ";" + Password, true);

            if (Auth.startsWith("A")){
                Auth = Auth.remove(0, 1);
                MyUserID = Auth.mid(0, 3);
                MyUsername = Auth.remove(MyUserID).mid(0, 20).remove("#");
                MyCred = Auth.split(";")[8];

                int R = Auth.remove(0, 20).mid(0, 3).toInt();
                int G = Auth.remove(0, 3).mid(0, 3).toInt();
                int B = Auth.remove(0, 3).mid(0, 3).toInt();
                R = (R < 0 ? 100 : (R + 100 <= 255 ? R + 100 : 255));
                G = (G < 0 ? 100 : (G + 100 <= 255 ? G + 100 : 255));
                B = (B < 0 ? 100 : (B + 100 <= 255 ? B + 100 : 255));

                IDToUsername[MyUserID] = MyUsername;
                UsernameToID[MyUsername] = MyUserID;
                UserRGB[MyUsername] = QString::number(R) + "," + QString::number(G) + "," + QString::number(B);
                UserAccess[MyUsername] = Auth.split(";")[9];

                GUIHandle->addUserToList(MyUserID, MyUsername, UserRGB[MyUsername], UserAccess[MyUsername], "0", "0");

                sendPacket("03_");
                sendPacket("02Z900_");

                for (auto Packet : Login){
                    sendPacket(Packet);
                }

                connect(SocketConn, &QTcpSocket::readyRead, this, &Client::handleConnection);

                connect(this, &Client::gotSpinnerList, [=](QList<QString> SpinnerList){
                    GUIHandle->appendMessage(SpinnerList.join(", "));
                });
            }
            else if (Auth == "091"){
                GUIHandle->shouldReconnect(false, "This account or the connection IP is currently under a ban.");
            }else if (Auth == "09"){
                GUIHandle->shouldReconnect(false, "Couldn't authenticate account.");
                GUIHandle->appendMessage("Couldn't authenticate account.");
            }
        }
    });

    connect(SocketConn, &QTcpSocket::connected, this, &Client::startKeepAlive);
    connect(SocketConn, &QTcpSocket::disconnected, this, &Client::stopKeepAlive);
    connect(SocketConn, &QTcpSocket::disconnected, GUIHandle, &MainWindow::appendLogout);
}

void Client::handleConnection(){
    while (SocketConn->getChar(&Buf)){
        Buffer += Buf;

        if (!Buf){
           QString Receive = Buffer;

            if (Receive.startsWith("0")){
                Receive = Receive.remove(0, 1);

                if (Receive.startsWith("c")){
                    emit gotSpinnerList(Receive.remove(0, 1).split(";"));
                }
                else if (Receive.startsWith("e") || Receive.startsWith("f")){
                    QList<QString> Data = Receive.remove(0, 1).split(";");
                    QString BanDetails = "This account has just been banned [Time: " + Data[0] + " | Reason: " + Data[1] + "]";
                    GUIHandle->shouldReconnect(false, BanDetails);

                    break;
                }
                else if (Receive.startsWith("g") || Receive.startsWith("j")){
                    GUIHandle->appendUserMessage("{Server};255,255,255;9;2", Receive.remove(0, 1));
                }
                else if (Receive == "93"){
                    GUIHandle->shouldReconnect(true, "A secondary login has been detected.");

                    break;
                }
                else if (Receive.startsWith("1")){
                    Receive = Receive.remove(0, 1);

                    //QList<QString> Game = Receive.remove(0, 1).split("0;");
                    //Receive = Receive.split(";");
                    GUIHandle->addGameToList(Receive.split(";"));//Receive.split("0;"));

                }

                else if (Receive.startsWith("h")){
                    Receive = Receive.remove(0, 1);

                    GUIHandle->appendMessage(Receive);

                }
                else if (Receive.startsWith("4")){
                    Receive = Receive.remove(0, 1);
                    QString Players = Receive.mid(2, 1);
                    QString TimeLeft = Receive.mid(3, 3);
                    QString Mode = Receive.mid(1, 1);
                    if (Mode == "2"){
                        Mode = "Repeat";
                    }else if (Mode == "0"){
                        Mode = "Cycle";
                    }else if (Mode == "1"){
                        Mode = "Random";
                    }
                    int TimeLeft1 = TimeLeft.toInt();
                    QString TimeLeft2 = QDateTime::fromTime_t(TimeLeft1).toUTC().toString("mm:ss");
                    GUIHandle->appendMessage("Players: " + Players + "/4\nTime Left: " + TimeLeft2 + "\nMode: " + Mode);
                }
                else if (Receive.startsWith("6")){
                    if (Receive.mid(1, 2) == "rc"){
                        QString creator = Receive.mid(4, 500);
                        GUIHandle->appendMessage("Creator: " + creator + "\n");
                    }
                    if (Receive.mid(1, 2) == "mp"){
                        QString map = Receive.mid(4, 1);
                        if (map == "0"){
                            GUIHandle->appendMessage("Map: XGen Hq");
                        }else if (map == "1"){
                            GUIHandle->appendMessage("Map: Sunnyvale Trailer Park");
                        }else if (map == "2"){
                            GUIHandle->appendMessage("Map: Toxic Spillway");
                        }else if (map == "3"){
                            GUIHandle->appendMessage("Map: Workplace Anxiety");
                        }else if (map == "4"){
                            GUIHandle->appendMessage("Map: Storage Yard");
                        }else if (map == "5"){
                            GUIHandle->appendMessage("Map: Green Labirynth");
                        }else if (map == "6"){
                            GUIHandle->appendMessage("Map: Floor Thirteen");
                        }else if (map == "7"){
                            GUIHandle->appendMessage("Map: The Pit");
                        }else if (map == "8"){
                            GUIHandle->appendMessage("Map: Industrial Drainage");
                        }else if (map == "9"){
                            GUIHandle->appendMessage("Map: Globalmegacorp LTD");
                        }else if (map == "A"){
                            GUIHandle->appendMessage("Map: Concrete Jungle");
                        }else if (map == "B"){
                            GUIHandle->appendMessage("Map: Nuclear Underground");
                        }else if (map == "C"){
                            GUIHandle->appendMessage("Map: Unstable Terrace");
                        }else if (map == "D"){
                            GUIHandle->appendMessage("Map: Office Space");
                        }else if (map == "E"){
                            GUIHandle->appendMessage("Map: The Foundation");
                        }else if (map == "F"){
                            GUIHandle->appendMessage("Map: Brawlers Burrow");
                        }else if (map == "G"){
                            GUIHandle->appendMessage("Map: Trench Run");
                        }else if (map == "H"){
                            GUIHandle->appendMessage("Map: Corporate Wasteland");
                        }else if (map == "I"){
                            GUIHandle->appendMessage("Map: Sewage Treatment");
                        }else if (map == "J"){
                            GUIHandle->appendMessage("Map: Storm Drain");
                        }else{
                            GUIHandle->appendMessage("Map: LabPass Map");
                    }
                    }
                }
            }
            else{
                if (Receive.startsWith("D")){
                    Receive = Receive.remove(0, 1);

                    QString UserID = Receive.mid(0, 3);
                    QString Username = IDToUsername[UserID];

                    GUIHandle->removeUserFromList(UserID, Username);
                    IDToUsername.remove(UserID);
                    UsernameToID.remove(Username);
                    if (in_game == true){
                        GUIHandle->appendMessage(">> <span style='color: rgb(" + UserRGB[Username] + ")'>" + Username + "</span> has left the area");
                    }
                }
                else if (Receive.startsWith("M")){
                    Receive = Receive.remove(0, 1);

                    QString MessageType = Receive.mid(3, 1);

                    if (MessageType == "9" || MessageType == "P"){
                        QString Username = IDToUsername[Receive.mid(0, 3)];
                        QString RGB = UserRGB[Username];
                        QString Access = UserAccess[Username];
                        GUIHandle->appendUserMessage(Username + ";" + RGB + ";" + MessageType + ";" + Access, Receive.mid(4, -1));
                    }else if (MessageType == "7"){
                        if (in_game == true){
                            QString diedUser = IDToUsername[Receive.mid(0, 3)];
                            QString Attacker = IDToUsername[Receive.mid(4, 3)];
                            GUIHandle->appendMessage(">> <span style='color: rgb(" + UserRGB[Attacker] + ")'>" + Attacker + "</span> killed <span style='color: rgb(" + UserRGB[diedUser] + ")'>" + diedUser + "</span>");
                        }
                    }else if (MessageType == "K"){
                        if (in_game == true && Receive.length() > 4){
                            QString kickedUser = IDToUsername[Receive.mid(4, 3)];
                            QString Kicker = IDToUsername[Receive.mid(0, 3)];
                            if (kickedUser == MyUsername){
                                GUIHandle->appendMessage(">> <span style='color: rgb(" + UserRGB[Kicker] + ")'>" + Kicker + "</span> is trying to kick me.");
                            }else{
                                GUIHandle->appendMessage(">> <span style='color: rgb(" + UserRGB[Kicker] + ")'>" + Kicker + "</span> is trying to kick <span style='color: rgb(" + UserRGB[kickedUser] + ")'>" + kickedUser + "</span>");
                            }
                        }
                    }
                }
                else if (Receive.startsWith("U")){
                    Receive = Receive.remove(0, 1);
                    if (in_game == false){
                        QString UserID = Receive.mid(0, 3);
                        QString Username = Receive.mid(3, 20).remove("#");

                        int R = Receive.mid(23, 3).toInt();
                        int G = Receive.mid(26, 3).toInt();
                        int B = Receive.mid(29, 3).toInt();
                        R = (R < 0 ? 100 : (R + 100 <= 255 ? R + 100 : 255));
                        G = (G < 0 ? 100 : (G + 100 <= 255 ? G + 100 : 255));
                        B = (B < 0 ? 100 : (B + 100 <= 255 ? B + 100 : 255));

                        IDToUsername[UserID] = Username;
                        UsernameToID[Username] = UserID;
                        UserRGB[Username] = QString::number(R) + "," + QString::number(G) + "," + QString::number(B);
                        UserKills[Username] = Receive.split(";")[0];
                        UserKills[Username] = UserKills[Username].remove("#");
                        UserKills[Username] = UserKills[Username].mid(3+Username.length()+9, 6);
                        UserLabpass[Username] = Receive.split(";")[5];
                        UserAccess[Username] = Receive.split(";")[6];
                        GUIHandle->addUserToList(UserID, Username, UserRGB[Username], UserAccess[Username], UserLabpass[Username], UserKills[Username]);
                        qDebug() << UserKills[Username];
                    }else{
                        QString UserID = Receive.mid(0, 3);
                        QString Kills = Receive.mid(4, 2);
                        QString Deaths = Receive.mid(6, 2);
                        QString Username = Receive.mid(8, 20).remove("#");
                        QString tempArr1 = Receive.remove("#");
                        tempArr1 = tempArr1.mid(8+Username.length()+2, 9);

                        int R = tempArr1.mid(0, 3).toInt();
                        int G = tempArr1.mid(3, 3).toInt();
                        int B = tempArr1.mid(6, 3).toInt();
                        R = (R < 0 ? 100 : (R + 100 <= 255 ? R + 100 : 255));
                        G = (G < 0 ? 100 : (G + 100 <= 255 ? G + 100 : 255));
                        B = (B < 0 ? 100 : (B + 100 <= 255 ? B + 100 : 255));

                        IDToUsername[UserID] = Username;
                        UsernameToID[Username] = UserID;
                        UserRGB[Username] = QString::number(R) + "," + QString::number(G) + "," + QString::number(B);
                        GUIHandle->addUserToList(UserID, Username, UserRGB[Username], "0", "0", "0");
                        GUIHandle->appendMessage(">> <span style='color: rgb(" + UserRGB[Username] + ")'>" + Username + "</span> has entered the area");
                        GUIHandle->appendMessage("Kills: " + Kills + " / Deaths: " + Deaths);
                        //qDebug() << tempArr1.mid(0, 3) + tempArr1.mid(3, 3) + tempArr1.mid(6, 3);
                        qDebug() << Receive;
                        //qDebug() << UserKills[Username];
                    }
                }
            }

            Buffer.clear();
        }
    }
}

Client::~Client(){
    delete SocketConn;
}
