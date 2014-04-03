#include "rotochatpage.h"
#include "ui_rotochatpage.h"
#include "walletmodel.h"

#include <QtWebKit>
#include <QtGui>

#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:6697/?nick=rotocoin?&theme=cli#therotocoinproject"
//#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:6697/?nick=rotocoin?#therotocoinproject"

RotoChatPage::RotoChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RotoChatPage),
    walletModel(0)
{
    ui->setupUi(this);
    ui->webView->setVisible(false);
    ui->webView->setUrl((QUrl(ROTOCHAT_URL)));
    ui->webView->setContextMenuPolicy(Qt::PreventContextMenu);
    
    //connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(hideElements(bool)));
}

/*void RotoChatPage::hideElements(bool ok)
{
    if (ok)
    {
        do
        {
        document().findFirst("a.kiwi_logo").removeFromDocument();
        document().findFirst("a.show_more").removeFromDocument();
        document().findFirst("tr.channel").removeFromDocument();
        //QTimer::singleShot(2000, this, SLOT(hideElements(bool)));
        }
        while(!document().findFirst("a.kiwi_logo").isNull() && !document().findFirst("a.show_more").isNull() && !document().findFirst("tr.channel").isNull());
        //QTimer::singleShot(2000, this, SLOT(onPageLoadFinished()));
        onPageLoadFinished();
        
    }

}*/

RotoChatPage::~RotoChatPage()
{
    delete ui;
}