#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <QFontDatabase>
#include <QFile>
#include <QPushButton>

extern bool in_game;
extern bool auto_kick;
extern bool suicide_enabled;

namespace Ui{
    class MainWindow;
}

class MainWindow:public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *Parent = 0);
    void handleLogin(QString Username, QString Password, QString Server);
    void addUserToList(QString UserID, QString Username, QString RGBString, QString Access, QString Labpass, QString Kills);
    void addGameToList(QStringList Game);
    void removeUserFromList(QString UserID, QString Username);
    void shouldReconnect(bool Should, QString Message = "");
    ~MainWindow();

signals:
    void appendUserMessage(QString UserData, QString Message);
    void appendMessage(QString Message);
    void appendUser(QString UserData);

public slots:
    void appendLogout();

private slots:
    void handleButtonlgout();
    void handleButtoncreateG();
    void handleButtonjoinLobby();
    void handleButtonjoinGame();
    void handleAutoKick();
    void handleWalk();
    void handleSuicide();
    void handleInput();

private:
    bool Reconnect = false;
    QString DisconnectMsg;
    QString Username, Password, Server;

    Ui::MainWindow *GUI;
    class Client *ConnHandle = NULL;
};

#endif // MAINWINDOW_H
