#ifndef ROTOFOROPAGE_H
#define ROTOFOROPAGE_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QTimer>

namespace Ui {
    class RotoForoPage;
}
class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Roto foro page widget */
class RotoForoPage : public QWidget
{
    Q_OBJECT

public:
    explicit RotoForoPage(QWidget *parent = 0);
    ~RotoForoPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);

public slots:

signals:

private:
    Ui::RotoForoPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;

    QNetworkAccessManager *nam;

private slots:
    void finished(QNetworkReply *reply);
    void DoHttpGet();
};

#endif // ROTOFOROPAGE_H
