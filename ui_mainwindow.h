/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version x.x.x
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QListWidget *user_list;
    QTextBrowser *client_out;
    QLineEdit *client_in;
    QListWidget *game_list;
    QPushButton *joinGame;
    QPushButton *createGame;
    QPushButton *logOut;
    QPushButton *LobbyBtn;
    QCheckBox *kickCheck;
    QCheckBox *checkWalk;
    QCheckBox *checkSuicide;
    QPlainTextEdit *altids;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(882, 603);
        QIcon icon;
        icon.addFile(QStringLiteral(":/assets/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        user_list = new QListWidget(centralWidget);
        user_list->setObjectName(QStringLiteral("user_list"));
        user_list->setGeometry(QRect(655, 210, 221, 241));
        client_out = new QTextBrowser(centralWidget);
        client_out->setObjectName(QStringLiteral("client_out"));
        client_out->setGeometry(QRect(10, 10, 641, 521));
        client_out->setOpenLinks(false);
        client_in = new QLineEdit(centralWidget);
        client_in->setObjectName(QStringLiteral("client_in"));
        client_in->setGeometry(QRect(10, 540, 641, 21));
        game_list = new QListWidget(centralWidget);
        game_list->setObjectName(QStringLiteral("game_list"));
        game_list->setGeometry(QRect(655, 10, 221, 201));
        joinGame = new QPushButton(centralWidget);
        joinGame->setObjectName(QStringLiteral("joinGame"));
        joinGame->setGeometry(QRect(660, 455, 211, 31));
        createGame = new QPushButton(centralWidget);
        createGame->setObjectName(QStringLiteral("createGame"));
        createGame->setGeometry(QRect(660, 490, 211, 31));
        logOut = new QPushButton(centralWidget);
        logOut->setObjectName(QStringLiteral("logOut"));
        logOut->setGeometry(QRect(680, 570, 174, 31));
        logOut->setMouseTracking(true);
        LobbyBtn = new QPushButton(centralWidget);
        LobbyBtn->setObjectName(QStringLiteral("LobbyBtn"));
        LobbyBtn->setGeometry(QRect(660, 530, 211, 31));
        kickCheck = new QCheckBox(centralWidget);
        kickCheck->setObjectName(QStringLiteral("kickCheck"));
        kickCheck->setGeometry(QRect(10, 570, 70, 17));
        checkWalk = new QCheckBox(centralWidget);
        checkWalk->setObjectName(QStringLiteral("checkWalk"));
        checkWalk->setGeometry(QRect(80, 570, 51, 17));
        checkSuicide = new QCheckBox(centralWidget);
        checkSuicide->setObjectName(QStringLiteral("checkSuicide"));
        checkSuicide->setGeometry(QRect(130, 570, 70, 17));
        altids = new QPlainTextEdit(centralWidget);
        altids->setObjectName(QStringLiteral("altids"));
        altids->setGeometry(QRect(190, 570, 61, 21));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Stick Arena", 0));
        client_out->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Sans Serif'; font-size:9pt;\"><br /></p></body></html>", 0));
        joinGame->setText(QApplication::translate("MainWindow", "JOIN GAME", 0));
        createGame->setText(QApplication::translate("MainWindow", "CREATE GAME", 0));
        logOut->setText(QApplication::translate("MainWindow", "LOG OUT", 0));
        LobbyBtn->setText(QApplication::translate("MainWindow", "BACK TO LOBBY", 0));
        kickCheck->setText(QApplication::translate("MainWindow", "Auto Kick", 0));
        checkWalk->setText(QApplication::translate("MainWindow", "Walk", 0));
        checkSuicide->setText(QApplication::translate("MainWindow", "Suicide", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
