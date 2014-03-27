#include <QtGui>
#include <QtWebKit>
#include <QSslSocket>

#include "guiconstants.h"
#include "guiutil.h"
#include "rotochatpage.h"
#include "walletmodel.h"

#define ROTOCHAT_URL "https://kiwiirc.com/client/irc.freenode.org:+6697/?nick=rotocoin?&theme=cli#therotocoinproject"

RotoChatPage::RotoChatPage(QWidget *parent) :
QWidget(parent),
ui(new Ui::RotoChatPage),
walletModel(0)
{
    ui->setupUi(this);
    ui->userNameEdit->setFocus();
    connect(ui->userNameEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(ui->userNameEdit, SIGNAL(returnPressed()), SLOT(inputPassword()));

    connect(ui->passwordEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(ui->passwordEdit, SIGNAL(returnPressed()), SLOT(doLogin()));

    ui->loginButton->setEnabled(false);
    connect(ui->loginButton, SIGNAL(clicked()), SLOT(doLogin()));

    connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(initialPage(bool)));
    connect(ui->webView, SIGNAL(loadProgress(int)),
        ui->progressBar, SLOT(setValue(int)));
    ui->webView->setUrl((QUrl(ROTOCHAT_URL)));
    ui->webView->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->webView->setZoomFactor(0.8);
    showStatus("Wait while the wallet connects to the server...");
}    

void RotoChatPage::showStatus(const QString &msg) 
{
   ui->statusLabel->setText(msg);
   ui->stackedWidget->setCurrentIndex(0);
}

void RotoChatPage::showError(const QString &msg)
{
   ui->progressBar->hide();
   showStatus(QString("Error: %1").arg(msg));
}

void RotoChatPage::adjustLoginButton()
{
   userName = ui->userNameEdit->text();
   password = ui->passwordEdit->text();
   bool ok = !userName.isEmpty() && !password.isEmpty(); // MIRAR PASSWORD NULA
   ui->loginButton->setEnabled(ok);
}

void RotoChatPage::inputPassword()
{
   if (!ui->userNameEdit->text().isEmpty())
       ui->passwordEdit->setFocus();
}

void RotoChatPage::doLogin()
{
   userName = ui->userNameEdit->text();
   password = ui->passwordEdit->text();
   bool ok = !userName.isEmpty() && !password.isEmpty();
   if (!ok)
       return;

   ui->progressBar->setValue(0);
   ui->progressBar->show();
   connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(loginPage(bool)));
   connect(ui->webView, SIGNAL(loadProgress(int)),
       ui->progressBar, SLOT(setValue(int)));
   showStatus("Logging in...");

   ui->webView->page()->mainFrame()->documentElement()->findFirst("#server_select_nick").setAttribute("value", userName);
   ui->webView->page()->mainFrame()->documentElement()->findFirst("#server_select_show_pass").setAttribute("checked", "true");
   ui->webView->page()->mainFrame()->documentElement()->findFirst("#server_select_password").setAttribute("value", password);
   ui->webView->page()->mainFrame()->documentElement()->findFirst("button").evaluateJavaScript("this.submit();"); /// probar con click si no va submit
    // code viejo
    //QWebFrame *frame = ui->webView->page()->mainFrame();
    //QWebElement nick = frame->documentElement();
    //nick.findFirst("button").evaluateJavaScript("this.click();");
}

void RotoChatPage::initialPage(bool ok)
{
   if (!QSslSocket::supportsSsl()) {
       showError("The Rotocoin Chat requires SSL support!");
       return;
   }

   if (ok) {
       /*QWebElement email = document().findFirst("#server_select_nick");
       QWebElement passwd = document().findFirst("#server_select_show_pass");
       QWebElement loginForm = document().findFirst("button");
       if (!email.isNull() && !passwd.isNull() && !loginForm.isNull()) {
           ui->stackedWidget->setCurrentIndex(1);
           ui->userNameEdit->setFocus();
           ui->webView->disconnect();*/
           return;
       //}
   }
   showError("SERVICE unavailable.");
}

void RotoChatPage::hideElements()
{
   ui->webView->page()->mainFrame()->documentElement()->findFirst(".show_more").removeFromDocument();
   ui->webView->page()->mainFrame()->documentElement()->findFirst(".have_pass").removeFromDocument();
   ui->webView->page()->mainFrame()->documentElement()->findFirst(".kiwi_logo").removeFromDocument();   
   QTimer::singleShot(2000, this, SLOT(hideElements()));
}

void RotoChatPage::loginPage(bool ok)
{
   QString location = ui->webView->url().toString();
   if (!ok) {
       //if (location.indexOf("CheckCookie"))
           return;
       showError("Service unavailable");
   } else {
         // check for any error message

       QWebElement  e = ui->webView->page()->mainFrame()->documentElement()->findFirst(".errormsg");
       if (e.isNull()) {
           ui->stackedWidget->setCurrentIndex(2);
           QTimer::singleShot(500, this, SLOT(hideElements()));
           return;
       }

       QString err = "Unknown login failure.";
       const QString errorMessage = e.toPlainText();
       if (!errorMessage.isEmpty()) {
           err = errorMessage;
           err = err.simplified();
       }
       showError(err);
   }
}

RotoChatPage::~RotoChatPage()
{
    delete ui;
}