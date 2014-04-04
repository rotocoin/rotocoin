#ifndef ROTOCHATPAGE_H
#define ROTOCHATPAGE_H

#include <QWidget>
//#include <QtWebKit>

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
    void onPageLoadFinished();
void hideElements(bool ok);

private:
    Ui::RotoChatPage *ui;
    WalletModel *walletModel;
};

#endif // ROTOCHATPAGE_H