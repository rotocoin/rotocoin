/*
 * W.J. van der Laan 2011-2012
 */

#include <QApplication>

#include "rotocoingui.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "init.h"
#include "util.h"
#include "ui_interface.h"
#include "paymentserver.h"
#include "splashscreen.h"

#include <QMessageBox>
#if QT_VERSION < 0x050000
#include <QTextCodec>
#endif
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include <QLibraryInfo>

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#if defined(ROTOCOIN_NEED_QT_PLUGINS) && !defined(_ROTOCOIN_QT_PLUGINS_INCLUDED)
#define _ROTOCOIN_QT_PLUGINS_INCLUDED
#define __INSURE__
#include <QtPlugin>
Q_IMPORT_PLUGIN(qcncodecs)
Q_IMPORT_PLUGIN(qjpcodecs)
Q_IMPORT_PLUGIN(qtwcodecs)
Q_IMPORT_PLUGIN(qkrcodecs)
Q_IMPORT_PLUGIN(qtaccessiblewidgets)
#endif

// Declare meta types used for QMetaObject::invokeMethod
Q_DECLARE_METATYPE(bool*)

// Need a global reference for the notifications to find the GUI
static RotocoinGUI *guiref;
static SplashScreen *splashref;

static bool ThreadSafeMessageBox(const std::string& message, const std::string& caption, unsigned int style)
{
    // Message from network thread
    if(guiref)
    {
        bool modal = (style & CClientUIInterface::MODAL);
        bool ret = false;
        // In case of modal message, use blocking connection to wait for user to click a button
        QMetaObject::invokeMethod(guiref, "message",
                                   modal ? GUIUtil::blockingGUIThreadConnection() : Qt::QueuedConnection,
                                   Q_ARG(QString, QString::fromStdString(caption)),
                                   Q_ARG(QString, QString::fromStdString(message)),
                                   Q_ARG(unsigned int, style),
                                   Q_ARG(bool*, &ret));
        return ret;
    }
    else
    {
        printf("%s: %s\n", caption.c_str(), message.c_str());
        fprintf(stderr, "%s: %s\n", caption.c_str(), message.c_str());
        return false;
    }
}

static bool ThreadSafeAskFee(int64 nFeeRequired)
{
    if(!guiref)
        return false;
    if(nFeeRequired < CTransaction::nMinTxFee || nFeeRequired <= nTransactionFee || fDaemon)
        return true;

    bool payFee = false;

    QMetaObject::invokeMethod(guiref, "askFee", GUIUtil::blockingGUIThreadConnection(),
                               Q_ARG(qint64, nFeeRequired),
                               Q_ARG(bool*, &payFee));

    return payFee;
}

static void InitMessage(const std::string &message)
{
    if(splashref)
    {
        splashref->showMessage(QString::fromStdString(message), Qt::AlignBottom|Qt::AlignHCenter, QColor(Qt::white));
        qApp->processEvents();
    }
    printf("init message: %s\n", message.c_str());
}

/*
   Translate string to current locale using Qt.
 */
static std::string Translate(const char* psz)
{
    return QCoreApplication::translate("rotocoin-core", psz).toStdString();
}

/* Handle runaway exceptions. Shows a message box with the problem and quits the program.
 */
static void handleRunawayException(std::exception *e)
{
    PrintExceptionContinue(e, "Runaway exception");
    QMessageBox::critical(0, "Runaway exception", RotocoinGUI::tr("A fatal error occurred. Rotocoin can no longer continue safely and will quit.") + QString("\n\n") + QString::fromStdString(strMiscWarning));
    exit(1);
}

#ifndef ROTOCOIN_QT_TEST
int main(int argc, char *argv[])
{
    // Command-line options take precedence:
    ParseParameters(argc, argv);

#if QT_VERSION < 0x050000
    // Internal string conversion is all UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForTr());
#endif

    Q_INIT_RESOURCE(rotocoin);
    QApplication app(argc, argv);

    /*QString darkStyle = "* {   background: #191919;    color: #DDDDDD;    border: 1px solid #5A5A5A;}";
    darkStyle +="QWidget::item:selected { background: #3D7848;}";
    darkStyle +="QCheckBox, QRadioButton { border: none;}";
    darkStyle +="QRadioButton::indicator, QCheckBox::indicator {    width: 13px;    height: 13px;}";
    darkStyle +="QRadioButton::indicator::unchecked, QCheckBox::indicator::unchecked {    border: 1px solid #5A5A5A;    background: none;}";
    darkStyle +="QRadioButton::indicator:unchecked:hover, QCheckBox::indicator:unchecked:hover {    border: 1px solid #DDDDDD;}";
    darkStyle +="QRadioButton::indicator::checked, QCheckBox::indicator::checked {    border: 1px solid #5A5A5A;    background: #5A5A5A;}";
    darkStyle +="QRadioButton::indicator:checked:hover, QCheckBox::indicator:checked:hover {    border: 1px solid #DDDDDD;    background: #DDDDDD;}";
    darkStyle +="QGroupBox {    margin-top: 6px;}";
    darkStyle +="QGroupBox::title {    top: -7px;    left: 7px;}";
    darkStyle +="QScrollBar {    border: 1px solid #5A5A5A;    background: #191919;}";
    darkStyle +="QScrollBar:horizontal {    height: 15px;    margin: 0px 0px 0px 32px;}";
    darkStyle +="QScrollBar:vertical {    width: 15px;    margin: 32px 0px 0px 0px;}";
    darkStyle +="QScrollBar::handle {    background: #353535;    border: 1px solid #5A5A5A;}";
    darkStyle +="QScrollBar::handle:horizontal {    border-width: 0px 1px 0px 1px;}";
    darkStyle +="QScrollBar::handle:vertical {    border-width: 1px 0px 1px 0px;}";
    darkStyle +="QScrollBar::handle:horizontal {    min-width: 20px;}";
    darkStyle +="QScrollBar::handle:vertical {    min-height: 20px;}";
    darkStyle +="QScrollBar::add-line, QScrollBar::sub-line {    background:#353535;    border: 1px solid #5A5A5A;    subcontrol-origin: margin;}";
    darkStyle +="QScrollBar::add-line {   position: absolute;}";
    darkStyle +="QScrollBar::add-line:horizontal {    width: 15px;    subcontrol-position: left;    left: 15px;}";
    darkStyle +="QScrollBar::add-line:vertical {    height: 15px;    subcontrol-position: top;    top: 15px;}";
    darkStyle +="QScrollBar::sub-line:horizontal {    width: 15px;    subcontrol-position: top left;}";
    darkStyle +="QScrollBar::sub-line:vertical {    height: 15px;    subcontrol-position: top;}";
    darkStyle +="QScrollBar:left-arrow, QScrollBar::right-arrow, QScrollBar::up-arrow, QScrollBar::down-arrow {    border: 1px solid #5A5A5A;    width: 3px;    height: 3px;}";
    darkStyle +="QScrollBar::add-page, QScrollBar::sub-page {    background: none;}";
    darkStyle +="QAbstractButton:hover {    background: #353535;}";
    darkStyle +="QAbstractButton:pressed {    background: #5A5A5A;}";
    darkStyle +="QAbstractItemView {    show-decoration-selected: 1;    selection-background-color: #3D7848;    selection-color: #DDDDDD;    alternate-background-color: #353535;}";
    darkStyle +="QHeaderView {    border: 1px solid #5A5A5A;}";
    darkStyle +="QHeaderView::section {    background: #191919;    border: 1px solid #5A5A5A;    padding: 4px;}";
    darkStyle +="QHeaderView::section:selected, QHeaderView::section::checked {    background: #353535;}";
    darkStyle +="QTableView {    gridline-color: #5A5A5A;}";
    darkStyle +="QTabBar {    margin-left: 2px;}";
    darkStyle +="QTabBar::tab {    border-radius: 0px;    padding: 4px;    margin: 4px;}";
    darkStyle +="QTabBar::tab:selected {    background: #353535;}";
    darkStyle +="QComboBox::down-arrow {    border: 1px solid #5A5A5A;    background: #353535;}";
    darkStyle +="QComboBox::drop-down {    border: 1px solid #5A5A5A;    background: #353535;}";
    darkStyle +="QComboBox::down-arrow {    width: 3px;    height: 3px;    border: 1px solid #5A5A5A;}";
    darkStyle +="QAbstractSpinBox {    padding-right: 15px;}";
    darkStyle +="QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {    border: 1px solid #5A5A5A;    background: #353535;    subcontrol-origin: border;}";
    darkStyle +="QAbstractSpinBox::up-arrow, QAbstractSpinBox::down-arrow {    width: 3px;    height: 3px;    border: 1px solid #5A5A5A;}";
    darkStyle +="QSlider {    border: none;}QSlider::groove:horizontal {   height: 5px;    margin: 4px 0px 4px 0px;}";
    darkStyle +="QSlider::groove:vertical {    width: 5px;    margin: 0px 4px 0px 4px;}";
    darkStyle +="QSlider::handle {    border: 1px solid #5A5A5A;    background: #353535;}";
    darkStyle +="QSlider::handle:horizontal {    width: 15px;    margin: -4px 0px -4px 0px;}";
    darkStyle +="QSlider::handle:vertical {   height: 15px;    margin: 0px -4px 0px -4px;}";
    darkStyle +="QSlider::add-page:vertical, QSlider::sub-page:horizontal {    background: #3D7848;}";
    darkStyle +="QSlider::sub-page:vertical, QSlider::add-page:horizontal {    background: #353535;}";
    darkStyle +="QLabel {    border: none;}";
    darkStyle +="QProgressBar {    text-align: center;}";
    darkStyle +="QProgressBar::chunk {    width: 1px;    background-color: #3D7848;}";
    darkStyle +="QMenu::separator {    background: #353535;}";
app.setStyleSheet(darkStyle);*/


    // Register meta types used for QMetaObject::invokeMethod
    qRegisterMetaType< bool* >();

    // Do this early as we don't want to bother initializing if we are just calling IPC
    // ... but do it after creating app, so QCoreApplication::arguments is initialized:
    if (PaymentServer::ipcSendCommandLine())
        exit(0);
    PaymentServer* paymentServer = new PaymentServer(&app);

    // Install global event filter that makes sure that long tooltips can be word-wrapped
    app.installEventFilter(new GUIUtil::ToolTipToRichTextFilter(TOOLTIP_WRAP_THRESHOLD, &app));

    // ... then rotocoin.conf:
    if (!boost::filesystem::is_directory(GetDataDir(false)))
    {
        // This message can not be translated, as translation is not initialized yet
        // (which not yet possible because lang=XX can be overridden in rotocoin.conf in the data directory)
        QMessageBox::critical(0, "Rotocoin",
                              QString("Error: Specified data directory \"%1\" does not exist.").arg(QString::fromStdString(mapArgs["-datadir"])));
        return 1;
    }
    ReadConfigFile(mapArgs, mapMultiArgs);

    // Application identification (must be set before OptionsModel is initialized,
    // as it is used to locate QSettings)
    QApplication::setOrganizationName("Rotocoin");
    QApplication::setOrganizationDomain("bitcoin.org");
    if(GetBoolArg("-testnet")) // Separate UI settings for testnet
        QApplication::setApplicationName("Rotocoin-Qt-testnet");
    else
        QApplication::setApplicationName("Rotocoin-Qt");

    // ... then GUI settings:
    OptionsModel optionsModel;

    // Get desired locale (e.g. "de_DE") from command line or use system locale
    QString lang_territory = QString::fromStdString(GetArg("-lang", QLocale::system().name().toStdString()));
    QString lang = lang_territory;
    // Convert to "de" only by truncating "_DE"
    lang.truncate(lang_territory.lastIndexOf('_'));

    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;
    // Load language files for configured locale:
    // - First load the translator for the base language, without territory
    // - Then load the more specific locale translator

    // Load e.g. qt_de.qm
    if (qtTranslatorBase.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslatorBase);

    // Load e.g. qt_de_DE.qm
    if (qtTranslator.load("qt_" + lang_territory, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);

    // Load e.g. rotocoin_de.qm (shortcut "de" needs to be defined in rotocoin.qrc)
    if (translatorBase.load(lang, ":/translations/"))
        app.installTranslator(&translatorBase);

    // Load e.g. rotocoin_de_DE.qm (shortcut "de_DE" needs to be defined in rotocoin.qrc)
    if (translator.load(lang_territory, ":/translations/"))
        app.installTranslator(&translator);

    // Subscribe to global signals from core
    uiInterface.ThreadSafeMessageBox.connect(ThreadSafeMessageBox);
    uiInterface.ThreadSafeAskFee.connect(ThreadSafeAskFee);
    uiInterface.InitMessage.connect(InitMessage);
    uiInterface.Translate.connect(Translate);

    // Show help message immediately after parsing command-line options (for "-lang") and setting locale,
    // but before showing splash screen.
    if (mapArgs.count("-?") || mapArgs.count("--help"))
    {
        GUIUtil::HelpMessageBox help;
        help.showOrPrint();
        return 1;
    }

#ifdef Q_OS_MAC
    // on mac, also change the icon now because it would look strange to have a testnet splash (green) and a std app icon (orange)
    if(GetBoolArg("-testnet")) {
        MacDockIconHandler::instance()->setIcon(QIcon(":icons/rotocoin_testnet"));
    }
#endif

    SplashScreen splash(QPixmap(), 0);
    if (GetBoolArg("-splash", true) && !GetBoolArg("-min"))
    {
        splash.show();
        splash.setAutoFillBackground(true);
        splashref = &splash;
    }

    app.processEvents();
    app.setQuitOnLastWindowClosed(false);

    try
    {
#ifndef Q_OS_MAC
        // Regenerate startup link, to fix links to old versions
        // OSX: makes no sense on mac and might also scan/mount external (and sleeping) volumes (can take up some secs)
        if (GUIUtil::GetStartOnSystemStartup())
            GUIUtil::SetStartOnSystemStartup(true);
#endif

        boost::thread_group threadGroup;

        RotocoinGUI window;
        guiref = &window;

        QTimer* pollShutdownTimer = new QTimer(guiref);
        QObject::connect(pollShutdownTimer, SIGNAL(timeout()), guiref, SLOT(detectShutdown()));
        pollShutdownTimer->start(200);

        if(AppInit2(threadGroup))
        {
            {
                // Put this in a block, so that the Model objects are cleaned up before
                // calling Shutdown().

                optionsModel.Upgrade(); // Must be done after AppInit2

                if (splashref)
                    splash.finish(&window);

                ClientModel clientModel(&optionsModel);
                WalletModel *walletModel = 0;
                if(pwalletMain)
                    walletModel = new WalletModel(pwalletMain, &optionsModel);

                window.setClientModel(&clientModel);
                if(walletModel)
                {
                    window.addWallet("~Default", walletModel);
                    window.setCurrentWallet("~Default");
                }

                // If -min option passed, start window minimized.
                if(GetBoolArg("-min"))
                {
                    window.showMinimized();
                }
                else
                {
                    window.show();
                }

                // Now that initialization/startup is done, process any command-line
                // rotocoin: URIs
                QObject::connect(paymentServer, SIGNAL(receivedURI(QString)), &window, SLOT(handleURI(QString)));
                QTimer::singleShot(100, paymentServer, SLOT(uiReady()));

                app.exec();

                window.hide();
                window.setClientModel(0);
                window.removeAllWallets();
                guiref = 0;
                delete walletModel;
            }
            // Shutdown the core and its threads, but don't exit Rotocoin-Qt here
            threadGroup.interrupt_all();
            threadGroup.join_all();
            Shutdown();
        }
        else
        {
            threadGroup.interrupt_all();
            threadGroup.join_all();
            Shutdown();
            return 1;
        }
    } catch (std::exception& e) {
        handleRunawayException(&e);
    } catch (...) {
        handleRunawayException(NULL);
    }
    return 0;
}
#endif // ROTOCOIN_QT_TEST
