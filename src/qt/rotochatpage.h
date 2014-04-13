#ifndef ROTOCHATPAGE_H
#define ROTOCHATPAGE_H

//#include <QWidget>
//#include <QtWebKit>
#include <QSplitter>
#include <QHash>

class IrcBuffer;
class IrcMessage;
class IrcUserModel;
class IrcCompleter;
class IrcConnection;
class IrcBufferModel;
class IrcCommandParser;

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QListView)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QTextDocument)

namespace Ui {
class RotoChatPage;
}

class WalletModel;

class RotoChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit RotoChatPage(QWidget *parent = 0);
    ~RotoChatPage();

    void setWalletModel(WalletModel *walletModel);
    QWidget *setupTabChain(QWidget *prev);

private slots:
    void onConnected();
    void onConnecting();
    void onDisconnected();

    void onTextEdited();
    void onTextEntered();

    void onCompletion();
    void onCompleted(const QString& text, int cursor);

    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);

    void onBufferActivated(const QModelIndex& index);
    void onUserActivated(const QModelIndex& index);

    void receiveMessage(IrcMessage* message);

private:
    Ui::RotoChatPage *ui;
    WalletModel *walletModel;

    void createLayout();
    void createCompleter();
    void createParser();
    void createUserList();
    void createBufferList();
    void createConnection();

    QLineEdit* lineEdit;
    QTextEdit* textEdit;
    QListView* userList;
    QListView* bufferList;

    IrcCompleter* completer;
    IrcCommandParser* parser;
    IrcConnection* connection;
    IrcBufferModel* bufferModel;
    QHash<IrcBuffer*, IrcUserModel*> userModels;
    QHash<IrcBuffer*, QTextDocument*> documents;
};

#endif // ROTOCHATPAGE_H