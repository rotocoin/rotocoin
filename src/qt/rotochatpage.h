#ifndef ROTOCHATPAGE_H
#define ROTOCHATPAGE_H

#include <QWidget>
#include <QWebElement>

#include "ui_rotochatpage.h"

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

protected:
    void showStatus(const QString &msg);
    void showError(const QString &msg);

private slots:
     void adjustLoginButton();
     void inputPassword();
     void doLogin();
     void initialPage(bool ok);
     void loginPage(bool ok);
     void hideElements();


private:
    Ui::RotoChatPage *ui;
    WalletModel *walletModel;
    QString userName;
    QString password;

};

#endif // ROTOCHATPAGE_H