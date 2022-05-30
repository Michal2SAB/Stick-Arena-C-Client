#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>

bool in_game = false;
bool auto_kick = false;
bool suicide_enabled = false;
// forgot what this was, some experimental shit with walking around the map or something
QList<QString> Walk1 = {"10538201036120000000000002400", "10538201036120000000000001800", "10538201036120000000000001690", "10538201036120000000000001680", "542"};
QList<QString> Walk = {"542", "571", "565", "533", "520", "554"};
// suicide hack, giving a selected user quick 60 or 40 kills
QList<QString> suicide = {"509", "804500700", "6807000", "71007000"};
MainWindow::MainWindow(QWidget *Parent):QMainWindow(Parent), GUI(new Ui::MainWindow){
    GUI->setupUi(this);
    QFontDatabase::addApplicationFont(":/assets/Roboto.ttf");

    QFile StyleSheet(":/assets/style.css");
    StyleSheet.open(QFile::ReadOnly);
    this->setStyleSheet(StyleSheet.readAll());
    StyleSheet.close();

    connect(GUI->logOut, SIGNAL (released()), this, SLOT (handleButtonlgout()));
    connect(GUI->client_in, &QLineEdit::returnPressed, this, &MainWindow::handleInput);
    connect(GUI->createGame, SIGNAL (released()), this, SLOT (handleButtoncreateG()));
    connect(GUI->LobbyBtn, SIGNAL (released()), this, SLOT (handleButtonjoinLobby()));
    connect(GUI->joinGame, SIGNAL (released()), this, SLOT (handleButtonjoinGame()));
    connect(GUI->kickCheck, SIGNAL (clicked()), this, SLOT (handleAutoKick()));
    connect(GUI->checkWalk, SIGNAL (clicked()), this, SLOT (handleWalk()));
    connect(GUI->checkSuicide, SIGNAL (clicked()), this, SLOT (handleSuicide()));


    connect(this, &MainWindow::appendUserMessage, [=](QString UserData, QString Message){
        QString HTML;
        QList<QString> Data = UserData.split(";");
        QString Username = Data[0];
        QString UserRGB = Data[1];
        QString MsgType = Data[2];
        int Access = Data[3].toInt();

        if ((Access == 0 ? false : true)){
            if (MsgType == "9"){
                HTML = "{M} <span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
            else if (MsgType == "P"){
                HTML = "{M} (Private) <span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
            else if (MsgType == "F"){
                HTML = "(Private to) {M} <span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
        }
        else{
            if (MsgType == "9"){
                HTML = "<span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
            else if (MsgType == "P"){
                HTML = "(Private) <span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
            // some spy on other client users shit
            else if (MsgType == "F"){
                HTML = "(Private to) <span style='color: rgb(" + UserRGB + ")'>[" + Username + "]</span> " + Message.toHtmlEscaped();
            }
        }

        GUI->client_out->append(HTML);
    });

    connect(this, &MainWindow::appendMessage, [=](QString Message){
        GUI->client_out->append(Message);
    });
}

void MainWindow::handleButtonlgout()
 {
    shouldReconnect(false);

    appendLogout();
 }

// the only way to have a real anti kick in game
void MainWindow::handleAutoKick()
 {
    QTimer::singleShot(500, this, &MainWindow::handleAutoKick);
    if (GUI->kickCheck->isChecked()){
        if (in_game == true){
            ConnHandle->sendPacket("K");
        }
    }
}

void MainWindow::handleSuicide()
 {
    QTimer::singleShot(20, this, &MainWindow::handleSuicide);
    if (GUI->checkSuicide->isChecked()){
        if (in_game == true){
            for (auto Packet : suicide){
                ConnHandle->sendPacket(Packet);
                // ConnHandle->sendPacket("71007000");
            }
        }
    }
}

void MainWindow::handleButtoncreateG()
 {
    QString Input = GUI->client_in->text().trimmed();
    appendMessage(">> Creating '" + Input + "'");
    ConnHandle->sendPacket("020200" + Input + ";");
    GUI->user_list->clear();
    in_game = true;
    ConnHandle->sendPacket("05mp=00");
    ConnHandle->sendPacket("04" + Input);
    ConnHandle->sendPacket("06" + Input + ";mp");
    ConnHandle->sendPacket("06" + Input + ";rc");
 }

void MainWindow::handleButtonjoinLobby()
 {
    ConnHandle->sendPacket("03_");
    ConnHandle->sendPacket("02Z900_");
    GUI->user_list->clear();
    in_game = false;
 }

void MainWindow::handleWalk()
 {
    QTimer::singleShot(20, this, &MainWindow::handleWalk);
    if (GUI->checkWalk->isChecked()){
        if (in_game == true){
            for (auto Packet : Walk){
                ConnHandle->sendPacket(Packet);
            }
        }
    }
 }

void MainWindow::handleButtonjoinGame()
 {
    QString Input = GUI->client_in->text().trimmed();
    appendMessage(">> Joining '" + Input + "'");
    in_game = true;
    GUI->user_list->clear();
    ConnHandle->sendPacket("03" + Input);
    ConnHandle->sendPacket("04" + Input);
    ConnHandle->sendPacket("06" + Input + ";mp");
    ConnHandle->sendPacket("06" + Input + ";rc");
    ConnHandle->sendPacket("800120010");
 }

void MainWindow::handleLogin(QString Username, QString Password, QString Server){
    ConnHandle = new Client(this, Username, Password, Server);
}

void MainWindow::handleInput(){
    QString Input = GUI->client_in->text().trimmed();
    GUI->client_in->clear();

    if (Input.startsWith("!")){
        QList<QString> InputArgs = Input.remove(0, 1).split(" ");
        QString Command = InputArgs[0].toLower();

        if (Command == "login"){
            if (ConnHandle != NULL){
                appendLogout();
            }

            if (InputArgs.length() == 3){
                Username = InputArgs[1];
                Password = InputArgs[2];
                Server = "ballistick5.xgenstudios.com:1138"; // default server

                handleLogin(Username, Password, Server);
            }
            else if (InputArgs.length() == 4){
                Username = InputArgs[1];
                Password = InputArgs[2];
                Server = InputArgs[3];

                handleLogin(Username, Password, Server);
            }
        }
        // buy oldskool spinner
        else if (Command == "buy"){
            if (InputArgs.length() == 1){
                ConnHandle->sendPacket("0b100" + InputArgs[1] + InputArgs[1]);
            }
        }
        else if (Command == "pm"){
            if (InputArgs.length() > 2 && ConnHandle != NULL){
                ConnHandle->sendPrivateMessage(InputArgs[1], Input.remove(0, 7));
            }
        }
        // send any packet to server (for test purposes)
        else if (Command == "raw"){
            if (InputArgs.length() > 1 && ConnHandle != NULL){
                ConnHandle->sendPacket(Input.remove(0, 4));
            }
        }
        else if (Command == "logout"){
            shouldReconnect(false);

            appendLogout();
        }
        // get any game info
        else if (Command == "info"){
            if (InputArgs.length() > 1){
                QString inputarg = Input.remove(0, 5);
                ConnHandle->sendPacket("04" + inputarg);
                ConnHandle->sendPacket("06" + inputarg + ";mp");
                ConnHandle->sendPacket("06" + inputarg + ";rc");
            }
        }
        else if (Command == "find"){
            if (InputArgs.length() > 1){
                QString inputarg = Input.remove(0, 5);
                ConnHandle->sendPacket("0h" + inputarg);
            }
        }
    }
    else{
        if (ConnHandle != NULL){
            ConnHandle->sendPublicMessage(Input);
        }
    }
}

void MainWindow::addUserToList(QString UserID, QString Username, QString RGBString, QString Access, QString Labpass, QString Kills){
    QListWidgetItem *User;
    QList<QString> RGB = RGBString.split(",");
    
    // some ugly ass code for adding ranks or lp icons next to users.. dont care to update it today
    if (Access[0] == '0'){
        if (Labpass[0] == '0'){
            qDebug() << Kills;
            if (Kills.toInt() == 0){
                User = new QListWidgetItem("[" + UserID + "] " + Username);
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                GUI->user_list->addItem(User);
            }else if (Kills.toInt() > 4 && Kills.toInt() < 25){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (1)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank1.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 24 && Kills.toInt() < 100){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (2)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank2.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 99 && Kills.toInt() < 300){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (3)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank3.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 299 && Kills.toInt() < 750){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (4)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank4.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 749 && Kills.toInt() < 2000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (5)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank5.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 1999 && Kills.toInt() < 5000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (6)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank6.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 4999 && Kills.toInt() < 10000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (7)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank7.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 9999 && Kills.toInt() < 20000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (8)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank8.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 19999 && Kills.toInt() < 40000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (9)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank9.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 39999 && Kills.toInt() < 60000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (10)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank10.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 59999 && Kills.toInt() < 80000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (11)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank11.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 79999 && Kills.toInt() < 100000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (12)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank12.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 99999 && Kills.toInt() < 125000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (13)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank13.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 124999 && Kills.toInt() < 150000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (14)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank14.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 149000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (15)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank15.png"));
                GUI->user_list->addItem(User);
            }else{
                User = new QListWidgetItem("[" + UserID + "] " + Username);
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                GUI->user_list->addItem(User);
            }
        }else{
            if (Kills.toInt() == 0){
                User = new QListWidgetItem("[" + UserID + "] " + Username);
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/B.png"));
                GUI->user_list->addItem(User);
            }else if (Kills.toInt() > 4 && Kills.toInt() < 25){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (1)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank1lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 24 && Kills.toInt() < 100){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (2)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank2lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 99 && Kills.toInt() < 300){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (3)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank3lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 299 && Kills.toInt() < 750){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (4)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank4lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 749 && Kills.toInt() < 2000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (5)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank5lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 1999 && Kills.toInt() < 5000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (6)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank6lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 4999 && Kills.toInt() < 10000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (7)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank7lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 9999 && Kills.toInt() < 20000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (8)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank8lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 19999 && Kills.toInt() < 40000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (9)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank9lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 39999 && Kills.toInt() < 60000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (10)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank10lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 59999 && Kills.toInt() < 80000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (11)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank11lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 79999 && Kills.toInt() < 100000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (12)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank12lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 99999 && Kills.toInt() < 125000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (13)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank13lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 124999 && Kills.toInt() < 150000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (14)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank14lp.png"));
                GUI->user_list->addItem(User);
            }else if(Kills.toInt() > 149000){
                User = new QListWidgetItem("[" + UserID + "] " + Username + " (15)");
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/rank15lp.png"));
                GUI->user_list->addItem(User);
            }else{
                User = new QListWidgetItem("[" + UserID + "] " + Username);
                User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
                User->setIcon(QIcon(":/assets/B.png"));
                GUI->user_list->addItem(User);
            }
        }
    }
    else{
        if (Kills.toInt() > 4 && Kills.toInt() < 25){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (1)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 24 && Kills.toInt() < 100){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (2)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 99 && Kills.toInt() < 300){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (3)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 299 && Kills.toInt() < 750){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (4)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 749 && Kills.toInt() < 2000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (5)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 1999 && Kills.toInt() < 5000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (6)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 4999 && Kills.toInt() < 10000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (7)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 9999 && Kills.toInt() < 20000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (8)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 19999 && Kills.toInt() < 40000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (9)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 39999 && Kills.toInt() < 60000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (10)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 59999 && Kills.toInt() < 80000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (11)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 79999 && Kills.toInt() < 100000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (12)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 99999 && Kills.toInt() < 125000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (13)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 124999 && Kills.toInt() < 150000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (14)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else if(Kills.toInt() > 149000){
            User = new QListWidgetItem("[" + UserID + "] " + Username + " (15)");
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }else{
            User = new QListWidgetItem("[" + UserID + "] " + Username);
            User->setBackground(QColor(RGB[0].toInt(), RGB[1].toInt(), RGB[2].toInt()));
            User->setIcon(QIcon(":/assets/mod.png"));
            GUI->user_list->insertItem(1, User);
        }
    }
}

void MainWindow::addGameToList(QStringList Game){
    GUI->game_list->clear();

    for ( const auto& i : Game  )
    {
        if (i != NULL){
            if (i != "_0"){
                if (i.endsWith("1")){
                    QListWidgetItem *Games = new QListWidgetItem(i.left(i.length() - 1));
                    Games->setForeground(QColor(178,150,0));
                    Games->setBackground(QColor(255, 255, 255));
                    Games->setIcon(QIcon(":/assets/B.png"));
                    GUI->game_list->addItem(Games);
                }else if (i.contains("!")){
                    QListWidgetItem *Games = new QListWidgetItem("Quick Start " + i.left(i.length() - 1));
                    //Games->setForeground(QColor(178,150,0));
                    //Games->setBackground(QColor(255, 255, 255));
                    GUI->game_list->addItem(Games);
                }else if (i.endsWith("0")){
                    QListWidgetItem *Games = new QListWidgetItem(i.left(i.length() - 1));
                    //Games->setBackground(QColor(255, 255, 255));
                    GUI->game_list->addItem(Games);
                }
            }
        }
    }
}

void MainWindow::removeUserFromList(QString UserID, QString Username){
    for(int RowNum = 0; RowNum < GUI->user_list->count(); ++RowNum){
        QListWidgetItem *UserItem = GUI->user_list->item(RowNum);

        if (UserItem->text().contains("[" + UserID + "] " + Username)){
            delete UserItem;
        }
    }
}

void MainWindow::shouldReconnect(bool Should, QString Message){
    Reconnect = Should;

    if (!Reconnect){
        DisconnectMsg = Message;
    }
}

void MainWindow::appendLogout(){
    if (ConnHandle != NULL){
        ConnHandle->deleteLater();
        ConnHandle = NULL;
    }

    if (Reconnect){
        Reconnect = false;

        GUI->user_list->clear();
        GUI->game_list->clear();
        GUI->client_out->clear();

        appendMessage("Disconnected; reconnecting to server...");
        handleLogin(Username, Password, Server);
    }
    else{
        GUI->user_list->clear();
        GUI->game_list->clear();
        GUI->client_out->clear();

        if (!DisconnectMsg.isEmpty()){
            appendMessage(DisconnectMsg);
        }
    }
}

MainWindow::~MainWindow(){
    delete GUI;
}
