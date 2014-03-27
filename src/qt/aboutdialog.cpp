#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "clientmodel.h"
#include "clientversion.h"
#include "version.h"
#include "util.h"

#include <QMessageBox>

// Copyright year (2009-this)
// Todo: update this when changing our copyright comments in the source
const int ABOUTDIALOG_COPYRIGHT_YEAR = 2014;
    //QString versionText     = QString("Version %1 ").arg(QString::fromStdString(FormatFullVersion()));

QString TextAboutText = QMessageBox::tr(
    "<h3>About Rotocoin</h3>"
    "<p><b>Rotocoin</b> version %1"
    "<p>Copyright &copy; 2009-%2 The Bitcoin developers<br>"
    "Copyright &copy; 2011-%2 The Litecoin developers<br>"
    "Copyright &copy; %2 The Rotocoin developers<br>"
    "Visit us at <a href='http://roto2.me/'; style='color: #3D7848; text-decoration: none;'>roto2.me</a>"
    " for more information</p>"
    ).arg("0.8.6.2").arg(ABOUTDIALOG_COPYRIGHT_YEAR);

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->copyrightLabel->setText(TextAboutText);
    ui->copyrightLabel->setTextFormat(Qt::RichText); // this make the link clickable
    ui->copyrightLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard); // this make the link clickable
    ui->copyrightLabel->setOpenExternalLinks(true);
}

void AboutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        QString versionLabel = model->formatFullVersion();
        //ui->versionLabel->setText(version);
        //ui->versionLabel->setText(version);
        // GUIUITIL - utility dialog
        // On x86 add a bit specifier to the version so that users can distinguish between
        // 32 and 64 bit builds. On other architectures, 32/64 bit may be more ambigious.
        
        /*#if defined(__x86_64__)
            version += tr(" (%1-bit)").arg(64);
        #elif defined(__i386__ )
            version += tr(" (%1-bit)").arg(32);
        #endif*/
        //ui->version->setText(versionLabel);
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonBox_accepted()
{
    close();
}