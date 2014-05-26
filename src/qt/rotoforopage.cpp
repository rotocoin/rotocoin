#include "rotoforopage.h"
#include "ui_rotoforopage.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "rotocoinunits.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "guiconstants.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 64
#define NUM_ITEMS 3



RotoForoPage::RotoForoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RotoForoPage),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);
    ui->setupUi(this);
    nam = new QNetworkAccessManager(this);
    DoHttpGet();
    QTimer *timer = new QTimer(this);
    ui->textBrowser->setHidden(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(DoHttpGet()));
    timer->start(35000);
    connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));
}

RotoForoPage::~RotoForoPage()
{
    delete ui;
}

void RotoForoPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void RotoForoPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}

void RotoForoPage::finished(QNetworkReply *reply) {
  ui->textBrowser->setHidden(false);
  if(reply->error() == QNetworkReply::NoError) {
    ui->textBrowser->setText(reply->readAll());
  } else {
    ui->textBrowser->setText(reply->errorString());
  }
}

void RotoForoPage::DoHttpGet() {
  QString url = "http://www.roto2.me/bitcointalk.php";
  nam->get(QNetworkRequest(QUrl(url)));
}
