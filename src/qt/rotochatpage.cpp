#include "rotochatpage.h"
#include "ui_rotochatpage.h"
#include "walletmodel.h"

#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:6697/?nick=rotocoin?&theme=cli#therotocoinproject"

RotoChatPage::RotoChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RotoChatPage),
    walletModel(0)
{
    ui->setupUi(this);
    ui->webView->setUrl((QUrl(ROTOCHAT_URL)));
    ui->webView->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->webView->setZoomFactor(0.8);
}    

RotoChatPage::~RotoChatPage()
{
    delete ui;
}