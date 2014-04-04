#include "rotochatpage.h"
#include "ui_rotochatpage.h"
#include "walletmodel.h"

#include <QtWebKitWidgets>
#include <QtWidgets>
#include <QtGui>

#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:6697/?nick=rotocoin?#therotocoinproject"
//#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:6697/?nick=rotocoin?#therotocoinproject"

RotoChatPage::RotoChatPage(QWidget *parent) 
    : QWidget(parent),    ui(new Ui::RotoChatPage),    walletModel(0)
{
    ui->setupUi(this);
    ui->webView->setVisible(false);
    ui->webView->setUrl((QUrl(ROTOCHAT_URL)));
    ui->webView->setContextMenuPolicy(Qt::PreventContextMenu);
    
    connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(hideElements(bool)));
}

void RotoChatPage::onPageLoadFinished()
{
    if (true)
    {
        QWebFrame* frame = ui->webView->page()->currentFrame();
        //QTimer::singleShot(2000, this, SLOT(hideElements(ok))); // WE CALL TWICE TO ENSURE
        if(frame!=NULL)
        {
            //frame->mainFrame()->documentElement().findFirst("input:([type=text])").setAttribute("display", "none");
            //get collection of the input elements with name "nick"
            //function introduced in Qt 4.6
            //QWebElementCollection collection = frame->findAllElements("input[id=\"server_select_nick\"]");
            //foreach (QWebElement element, collection)
                //element.setAttribute("value","rotoworldchange");
            ui->webView->setZoomFactor(1);
            ui->webView->setVisible(true);
        }
    }
        //ui->webView->page()->mainFrame()->documentElement().findFirst("#server_select_show_pass").setAttribute("checked", "true");
        //ui->webView->page()->mainFrame()->documentElement().findFirst("input:([type=text])").setAttribute("display", "none");
        //return;
}

void RotoChatPage::hideElements(bool ok)
{
    if (ok)
    {
        //QWebFrame* frame = ui->webView->page()->currentFrame();// pasar por parametro
        QWebElement dom = ui->webView->page()->mainFrame()->documentElement();
        dom.findFirst("tr.channel").setStyleProperty("display","none");
        dom.findFirst("a.show_more").setStyleProperty("display","none");
        dom.findFirst("a.kiwi_logo").setStyleProperty("display","none");
        dom.findFirst("i.icon-upload-alt").setStyleProperty("display","none");
        dom.findFirst("div.app_tools").setStyleProperty("display","none");
        dom.findFirst("i.icon-signout.channel_part").setStyleProperty("display","none");
        //dom.findFirst("li.active").setStyleProperty("display","none");

        /*do
        {

        
        }
        while(frame->documentElement().findFirst("tr.channel").attribute("display")!="none");*/
        QTimer::singleShot(2000, this, SLOT(onPageLoadFinished()));
        //onPageLoadFinished();
        
    }

}

RotoChatPage::~RotoChatPage()
{
    delete ui;
}

