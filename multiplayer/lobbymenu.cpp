#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>

#include "3rd_party/oxygine-framework/oxygine/actor/Stage.h"

#include "multiplayer/lobbymenu.h"
#include "multiplayer/networkcommands.h"
#include "multiplayer/dialogpassword.h"
#include "multiplayer/dialogpasswordandadress.h"
#include "multiplayer/multiplayermenu.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/settings.h"
#include "coreengine/audiothread.h"

#include "menue/mainwindow.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

#include "objects/base/chat.h"
#include "objects/dialogs/dialogmessagebox.h"
#include "objects/dialogs/customdialog.h"
#include "objects/tableView/stringtableitem.h"
#include "objects/tableView/xofytableitem.h"
#include "objects/tableView/locktableitem.h"

#include "network/mainserver.h"
#include "network/JsonKeys.h"

LobbyMenu::LobbyMenu()
{
    Mainapp* pApp = Mainapp::getInstance();
    moveToThread(pApp->getWorkerthread());
    CONSOLE_PRINT("Entering Lobby Menue", Console::eDEBUG);
    Interpreter::setCppOwnerShip(this);

    if (!Settings::getServer())
    {
        m_pTCPClient = spTCPClient::create(nullptr);
        m_pTCPClient->moveToThread(Mainapp::getInstance()->getNetworkThread());
        connect(m_pTCPClient.get(), &TCPClient::recieveData, this, &LobbyMenu::recieveData, Qt::QueuedConnection);
        connect(m_pTCPClient.get(), &TCPClient::sigConnected, this, &LobbyMenu::connected, Qt::QueuedConnection);
        emit m_pTCPClient->sig_connect(Settings::getServerAdress(), Settings::getServerPort(), Settings::getSecondaryServerAdress());
    }

    BackgroundManager* pBackgroundManager = BackgroundManager::getInstance();
    // load background
    oxygine::spSprite sprite = oxygine::spSprite::create();
    addChild(sprite);
    oxygine::ResAnim* pBackground = pBackgroundManager->getResAnim("lobbymenu");
    sprite->setResAnim(pBackground);
    sprite->setPosition(-1, -1);
    // background should be last to draw
    sprite->setPriority(static_cast<qint32>(Mainapp::ZOrder::Background));
    sprite->setScaleX(Settings::getWidth() / pBackground->getWidth());
    sprite->setScaleY(Settings::getHeight() / pBackground->getHeight());

    pApp->getAudioThread()->clearPlayList();
    pApp->getAudioThread()->loadFolder("resources/music/multiplayer");
    pApp->getAudioThread()->playRandom();

    oxygine::spButton pButtonExit = ObjectManager::createButton(tr("Exit"));
    addChild(pButtonExit);
    pButtonExit->setPosition(10, Settings::getHeight() - pButtonExit->getScaledHeight() - 10);
    pButtonExit->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigExitMenue();
    });
    connect(this, &LobbyMenu::sigExitMenue, this, &LobbyMenu::exitMenue, Qt::QueuedConnection);

    oxygine::spButton pButtonHost = ObjectManager::createButton(tr("Direct Host"));
    addChild(pButtonHost);
    pButtonHost->setPosition(Settings::getWidth() - pButtonHost->getScaledWidth() - 10,
                             Settings::getHeight() - pButtonExit->getScaledHeight() - 10);
    pButtonHost->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigHostLocal();
    });
    connect(this, &LobbyMenu::sigHostLocal, this, &LobbyMenu::hostLocal, Qt::QueuedConnection);

    m_pButtonHostOnServer = ObjectManager::createButton(tr("Server Host"));
    addChild(m_pButtonHostOnServer);
    m_pButtonHostOnServer->setPosition(Settings::getWidth() - pButtonHost->getScaledWidth() - 10,
                                       Settings::getHeight() - pButtonExit->getScaledHeight() * 2 - 10);
    m_pButtonHostOnServer->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigHostServer();
    });
    connect(this, &LobbyMenu::sigHostServer, this, &LobbyMenu::hostServer, Qt::QueuedConnection);
    m_pButtonHostOnServer->setEnabled(false);

    m_pButtonGameJoin = ObjectManager::createButton(tr("Join Game"));
    addChild(m_pButtonGameJoin);
    m_pButtonGameJoin->setPosition(Settings::getWidth() / 2 + 10,
                                   Settings::getHeight() - pButtonExit->getScaledHeight() - 10);
    m_pButtonGameJoin->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigJoinGame();
    });
    connect(this, &LobbyMenu::sigJoinGame, this, &LobbyMenu::joinGame, Qt::QueuedConnection);
    m_pButtonGameJoin->setEnabled(false);

    m_pButtonGameObserve = ObjectManager::createButton(tr("Observe Game"));
    addChild(m_pButtonGameObserve);
    m_pButtonGameObserve->setPosition(Settings::getWidth() / 2 + 10, m_pButtonGameJoin->getY() - m_pButtonGameJoin->getScaledHeight());
    m_pButtonGameObserve->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigObserveGame();
    });
    connect(this, &LobbyMenu::sigObserveGame, this, &LobbyMenu::observeGame, Qt::QueuedConnection);
    m_pButtonGameObserve->setEnabled(false);

    oxygine::spButton pButtonJoinAdress = ObjectManager::createButton(tr("Join Address"));
    addChild(pButtonJoinAdress);
    pButtonJoinAdress->setPosition(Settings::getWidth() / 2 - 10 - pButtonJoinAdress->getScaledWidth(),
                                   Settings::getHeight() - pButtonExit->getScaledHeight() - 10);
    pButtonJoinAdress->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigJoinAdress();
    });
    connect(this, &LobbyMenu::sigJoinAdress, this, &LobbyMenu::joinAdress, Qt::QueuedConnection);

    oxygine::spButton pButtonObserveAdress = ObjectManager::createButton(tr("Observe Address"));
    addChild(pButtonObserveAdress);
    pButtonObserveAdress->setPosition(Settings::getWidth() / 2 - 10 - pButtonJoinAdress->getScaledWidth(),
                                      pButtonJoinAdress->getY() - pButtonJoinAdress->getScaledHeight());
    pButtonObserveAdress->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigObserveAdress();
    });
    connect(this, &LobbyMenu::sigObserveAdress, this, &LobbyMenu::observeAdress, Qt::QueuedConnection);

    m_pButtonSwapLobbyMode = ObjectManager::createButton(tr("Show my games"));
    addChild(m_pButtonSwapLobbyMode);
    m_pButtonSwapLobbyMode->setPosition(Settings::getWidth() / 2 - m_pButtonSwapLobbyMode->getScaledWidth() - 5, 10);
    m_pButtonSwapLobbyMode->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigChangeLobbyMode();
    });
    m_pButtonSwapLobbyMode->setEnabled(false);
    connect(this, &LobbyMenu::sigChangeLobbyMode, this, &LobbyMenu::changeLobbyMode, Qt::QueuedConnection);

    m_pButtonUpdateGamesMode = ObjectManager::createButton(tr("Refresh games"));
    addChild(m_pButtonUpdateGamesMode);
    m_pButtonUpdateGamesMode->setPosition(Settings::getWidth() / 2 + 5, 10);
    m_pButtonUpdateGamesMode->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event * )->void
    {
        emit sigRequestUpdateGames();
    });
    m_pButtonUpdateGamesMode->setEnabled(false);
    connect(this, &LobbyMenu::sigRequestUpdateGames, this, &LobbyMenu::requestUpdateGames, Qt::QueuedConnection);

    qint32 height = m_pButtonHostOnServer->getY() - 220 - 10 - m_pButtonSwapLobbyMode->getScaledHeight();
    if (Settings::getSmallScreenDevice())
    {
        height = m_pButtonHostOnServer->getY() - 20 - m_pButtonSwapLobbyMode->getScaledHeight();
    }

    QStringList header = {tr("Map"), tr("Players"), tr("Description"), tr("Mods"), tr("Locked")};
    qint32 itemWidth = (Settings::getWidth() - 20 - 80 - 100 - 90) / 3;
    QVector<qint32> widths = {itemWidth, 100, itemWidth, itemWidth, 90};
    m_gamesview = spComplexTableView::create(widths, header, height);
    m_gamesview->setPosition(10, 10 + 10 + pButtonJoinAdress->getScaledHeight());
    addChild(m_gamesview);

    spNetworkInterface pInterface = m_pTCPClient;
    if (Settings::getServer())
    {
        pInterface = MainServer::getInstance()->getGameServer();
    }

    height = m_pButtonHostOnServer->getY() - m_gamesview->getY() - m_gamesview->getScaledHeight() - 20;

    spChat pChat = spChat::create(pInterface, QSize(Settings::getWidth() - 20, height), NetworkInterface::NetworkSerives::LobbyChat);
    pChat->setPosition(10, m_gamesview->getY() + m_gamesview->getScaledHeight() + 10);
    if (Settings::getSmallScreenDevice())
    {
        pChat->setVisible(false);
    }
    addChild(pChat);
    connect(this, &LobbyMenu::sigUpdateGamesView, this, &LobbyMenu::updateGamesView, Qt::QueuedConnection);
    connect(m_gamesview.get(), &ComplexTableView::sigItemClicked, this, &LobbyMenu::selectGame, Qt::QueuedConnection);
}

void LobbyMenu::requestUpdateGames()
{
    if (m_mode == GameViewMode::OpenGames)
    {
        requestUserUpdateGames();
    }
    else
    {
        requestServerGames();
    }
}

void LobbyMenu::requestServerGames()
{
    if (m_pTCPClient.get() != nullptr)
    {
        QJsonObject data;
        data.insert(JsonKeys::JSONKEY_COMMAND, NetworkCommands::SERVERREQUESTGAMES);
        QJsonDocument doc(data);
        emit m_pTCPClient->sig_sendData(0, doc.toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
    }
}

void LobbyMenu::requestUserUpdateGames()
{
    if (m_pTCPClient.get() != nullptr)
    {
        QJsonObject data;
        data.insert(JsonKeys::JSONKEY_COMMAND, NetworkCommands::SERVERREQUESTUSERGAMES);
        data.insert(JsonKeys::JSONKEY_USERNAME, Settings::getUsername());
        QJsonDocument doc(data);
        emit m_pTCPClient->sig_sendData(0, doc.toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
    }
}

void LobbyMenu::enableServerButtons(bool enable)
{
    m_pButtonGameObserve->setEnabled(enable);
    m_pButtonGameJoin->setEnabled(enable);
    m_pButtonHostOnServer->setEnabled(enable);
    m_pButtonSwapLobbyMode->setEnabled(enable);
    m_pButtonUpdateGamesMode->setEnabled(enable);
}

void LobbyMenu::changeLobbyMode()
{
    QString newLabel;
    if (m_mode == GameViewMode::OpenGames)
    {
        m_mode = GameViewMode::OwnGames;
        newLabel = tr("Show open games");
        requestUserUpdateGames();
    }
    else
    {
        m_mode = GameViewMode::OpenGames;
        newLabel = tr("Show my games");
        requestServerGames();
    }
    static_cast<Label*>(m_pButtonSwapLobbyMode->getFirstChild().get())->setText(newLabel);
}

void LobbyMenu::leaveServer()
{
    m_pTCPClient = nullptr;
    enableServerButtons(false);
}

void LobbyMenu::exitMenue()
{    
    CONSOLE_PRINT("Leaving Lobby Menue", Console::eDEBUG);
    auto window = spMainwindow::create("ui/menu/mainmenu.xml");
    oxygine::Stage::getStage()->addChild(window);
    oxygine::Actor::detach();
}

void LobbyMenu::hostLocal()
{    
    CONSOLE_PRINT("Leaving Lobby Menue", Console::eDEBUG);
    oxygine::Stage::getStage()->addChild(spMultiplayermenu::create("", "", Settings::getGamePort(), "", Multiplayermenu::NetworkMode::Host));
    oxygine::Actor::detach();
}

void LobbyMenu::hostServer()
{
    if (m_pTCPClient.get() != nullptr &&
        m_pTCPClient->getIsConnected())
    {
        CONSOLE_PRINT("Leaving Lobby Menue", Console::eDEBUG);
        oxygine::Stage::getStage()->addChild(spMultiplayermenu::create(m_pTCPClient, "",  Multiplayermenu::NetworkMode::Host));
        oxygine::Actor::detach();
    }
}

void LobbyMenu::joinGame()
{
    if (m_currentGame.get() &&
       (m_mode == GameViewMode::OwnGames || m_currentGame->hasOpenPlayers()))
    {
        if (m_currentGame->getLocked())
        {
            spDialogPassword pDialogTextInput = spDialogPassword::create(tr("Enter Password"), true, "");
            addChild(pDialogTextInput);
            connect(pDialogTextInput.get(), &DialogPassword::sigTextChanged, this, &LobbyMenu::joinGamePassword, Qt::QueuedConnection);
            
        }
        else
        {
            joinGamePassword("");
        }
    }
}

void LobbyMenu::joinGamePassword(QString password)
{
    bool exists = false;
    if (m_currentGame.get() != nullptr)
    {
        for (const auto & game : qAsConst(m_games))
        {
            if (m_currentGame.get() == game.get())
            {
                exists = true;
                break;
            }
        }
    }
    if (exists)
    {
        CONSOLE_PRINT("Leaving Lobby Menue to join server game", Console::eDEBUG);
        QString command = QString(NetworkCommands::SERVERJOINGAME);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);

        QJsonObject data;
        data.insert(JsonKeys::JSONKEY_COMMAND, command);
        data.insert(JsonKeys::JSONKEY_SLAVENAME, m_currentGame->getSlaveName());
        QJsonDocument doc(data);
        emit m_pTCPClient->sig_sendData(0, doc.toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
        m_password = password;
    }
}

void LobbyMenu::joinAdress()
{
    spDialogPasswordAndAdress pDialogTextInput = spDialogPasswordAndAdress::create(tr("Enter Host Adress"));
    addChild(pDialogTextInput);
    connect(pDialogTextInput.get(), &DialogPasswordAndAdress::sigTextChanged, this, &LobbyMenu::join, Qt::QueuedConnection);
}

void LobbyMenu::join(QString adress, QString password)
{    
    CONSOLE_PRINT("Leaving Lobby Menue to join game by adress", Console::eDEBUG);
    oxygine::Stage::getStage()->addChild(spMultiplayermenu::create(adress.trimmed(), "", Settings::getGamePort(), password, Multiplayermenu::NetworkMode::Client));
    oxygine::Actor::detach();
}

void LobbyMenu::observeAdress()
{
    spDialogPasswordAndAdress pDialogTextInput = spDialogPasswordAndAdress::create(tr("Enter Host Adress"));
    addChild(pDialogTextInput);
    connect(pDialogTextInput.get(), &DialogPasswordAndAdress::sigTextChanged, this, &LobbyMenu::observe, Qt::QueuedConnection);
}

void LobbyMenu::observe(QString adress, QString password)
{
    CONSOLE_PRINT("Leaving Lobby Menue to observe game by adress", Console::eDEBUG);
    oxygine::Stage::getStage()->addChild(spMultiplayermenu::create(adress.trimmed(), "", Settings::getGamePort(), password, Multiplayermenu::NetworkMode::Observer));
    oxygine::Actor::detach();
}

void LobbyMenu::observeGame()
{
    if (m_currentGame.get())
    {
        if (m_currentGame->getLocked())
        {
            spDialogPassword pDialogTextInput = spDialogPassword::create(tr("Enter Password"), true, "");
            addChild(pDialogTextInput);
            connect(pDialogTextInput.get(), &DialogPassword::sigTextChanged, this, &LobbyMenu::observeGamePassword, Qt::QueuedConnection);

        }
        else
        {
            observeGamePassword("");
        }
    }
}

void LobbyMenu::observeGamePassword(QString password)
{
    bool exists = false;
    if (m_currentGame.get() != nullptr)
    {
        for (const auto & game : qAsConst(m_games))
        {
            if (m_currentGame.get() == game.get())
            {
                exists = true;
                break;
            }
        }
    }
    if (exists)
    {
        CONSOLE_PRINT("Leaving Lobby Menue to observe server game", Console::eDEBUG);
        m_pTCPClient->setIsObserver(true);
        oxygine::Stage::getStage()->addChild(spMultiplayermenu::create(m_pTCPClient, password, Multiplayermenu::NetworkMode::Observer));
        QString command = QString(NetworkCommands::SERVERJOINGAME);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        QJsonObject data;
        data.insert(JsonKeys::JSONKEY_COMMAND, command);
        data.insert(JsonKeys::JSONKEY_SLAVENAME, m_currentGame->getSlaveName());
        QJsonDocument doc(data);
        emit m_pTCPClient->sig_sendData(0, doc.toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
        oxygine::Actor::detach();
    }
}

void LobbyMenu::recieveData(quint64 socketID, QByteArray data, NetworkInterface::NetworkSerives service)
{
    if (service == NetworkInterface::NetworkSerives::ServerHostingJson)
    {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject objData = doc.object();
        QString messageType = objData.value(JsonKeys::JSONKEY_COMMAND).toString();
        CONSOLE_PRINT("LobbyMenu Command received: " + messageType, Console::eDEBUG);
        if (messageType == NetworkCommands::SERVERGAMEDATA)
        {
            if (m_loggedIn && m_mode == GameViewMode::OpenGames)
            {
                updateGameData(objData);
            }
        }
        else if (messageType == NetworkCommands::SERVERUSERGAMEDATA)
        {
            if (m_loggedIn  && m_mode == GameViewMode::OwnGames)
            {
                updateGameData(objData);
            }
        }
        else if (messageType == NetworkCommands::SLAVEADDRESSINFO)
        {
            joinSlaveGame(objData);
        }
        else if (messageType == NetworkCommands::SERVERGAMENOLONGERAVAILABLE)
        {
            spDialogMessageBox pDialogMessageBox;
            pDialogMessageBox = spDialogMessageBox::create(tr("Host game is no longer available."));
            addChild(pDialogMessageBox);
        }
        else if (messageType == NetworkCommands::SENDPUBLICKEY)
        {
            auto action = static_cast<NetworkCommands::PublicKeyActions>(objData.value(JsonKeys::JSONKEY_RECEIVEACTION).toInt());
            if (action == NetworkCommands::PublicKeyActions::CreateAccount)
            {
                onPublicKeyCreateAccount(socketID, objData, action);
            }
            else if (action == NetworkCommands::PublicKeyActions::LoginAccount)
            {
                onPublicKeyLoginAccount(socketID, objData, action);
            }
            else if (action == NetworkCommands::PublicKeyActions::ResetPassword)
            {
                onPublicKeyResetAccount(socketID, objData, action);
            }
            else if (action == NetworkCommands::PublicKeyActions::ChangePassword)
            {
                onPublicKeyChangePassword(socketID, objData, action);
            }
            else
            {
                CONSOLE_PRINT("Unknown public key action " + QString::number(static_cast<qint32>(action)) + " received", Console::eDEBUG);
            }
        }
        else if (messageType == NetworkCommands::SERVERACCOUNTMESSAGE)
        {
            handleAccountMessage(socketID, objData);
        }
        else
        {
            CONSOLE_PRINT("Unknown command " + messageType + " received", Console::eDEBUG);
        }
    }
}

void LobbyMenu::updateGameData(const QJsonObject & objData)
{
    m_games.clear();
    QJsonObject games = objData.value(JsonKeys::JSONKEY_GAMES).toObject();
    for (const auto & game : games)
    {
        auto gameData = spNetworkGameData::create();
        gameData->fromJson(game.toObject());
        m_games.append(gameData);
    }
    emit sigUpdateGamesView();
}

void LobbyMenu::joinSlaveGame(const QJsonObject & objData)
{
    QString slaveAddress = objData.value(JsonKeys::JSONKEY_ADDRESS).toString();
    QString secondarySlaveAddress = objData.value(JsonKeys::JSONKEY_SECONDARYADDRESS).toString();
    quint16 slavePort = objData.value(JsonKeys::JSONKEY_PORT).toInteger();
    CONSOLE_PRINT("Leaving Lobby Menue to join game by adress", Console::eDEBUG);
    oxygine::Stage::getStage()->addChild(spMultiplayermenu::create(slaveAddress.trimmed(), secondarySlaveAddress.trimmed(), slavePort, m_password, Multiplayermenu::NetworkMode::Client));
    oxygine::Actor::detach();
}

void LobbyMenu::updateGamesView()
{
    const auto & widths = m_gamesview->getWidths();
    ComplexTableView::Items items;

    bool hasGame = m_currentGame.get() != nullptr;
    qint64 uuid = 0;
    if (hasGame)
    {
        uuid = m_currentGame->getUuid();
    }
    qint32 itemCount = 0;
    qint32 currentItem = -1;
    for (auto & game : m_games)
    {
        if (hasGame && game->getUuid() == uuid)
        {
            currentItem = itemCount;
            m_currentGame = game;
        }
        ComplexTableView::Item item;
        item.pData = game.get();
        item.items.append(oxygine::static_pointer_cast<BaseTableItem>(spStringTableItem::create(game->getMapName(), widths[0])));
        item.items.append(oxygine::static_pointer_cast<BaseTableItem>(spXofYTableItem::create(game->getPlayers(), game->getMaxPlayers(), widths[1])));
        item.items.append(oxygine::static_pointer_cast<BaseTableItem>(spStringTableItem::create(game->getDescription(), widths[2])));
        QStringList mods = game->getMods();
        QString modString;
        for (const auto & mod : mods)
        {
            modString.append(Settings::getModName(mod) + "; ");
        }
        item.items.append(oxygine::static_pointer_cast<BaseTableItem>(spStringTableItem::create(modString, widths[3])));
        item.items.append(oxygine::static_pointer_cast<BaseTableItem>(spLockTableItem::create(game->getLocked(), widths[4])));
        items.append(item);
        ++itemCount;
    }
    m_gamesview->setItems(items);
    if (hasGame && currentItem >= 0)
    {
        m_gamesview->setCurrentItem(currentItem);
    }
    else
    {
        m_currentGame = spNetworkGameData();
    }
}

void LobbyMenu::selectGame()
{
    if (m_gamesview->getCurrentItem() >= 0)
    {
        m_currentGame = spNetworkGameData(m_gamesview->getDataItem<NetworkGameData>(m_gamesview->getCurrentItem()));
    }
}

void LobbyMenu::connected(quint64 socket)
{
    QString password = Settings::getServerPassword();
    spCustomDialog pDialog = spCustomDialog::create("userLogin", "ui/serverLogin/userLoginDialog.xml", this);
    addChild(pDialog);
    if (!password.isEmpty())
    {
        loginToServerAccount(password);
    }
}

void LobbyMenu::onLogin()
{
    enableServerButtons(true);
    m_loggedIn = true;
}

void LobbyMenu::onEnter()
{
    Interpreter* pInterpreter = Interpreter::getInstance();
    QString object = "Init";
    QString func = "lobbyMenu";
    if (pInterpreter->exists(object, func))
    {
        CONSOLE_PRINT("Executing:" + object + "." + func, Console::eDEBUG);
        QJSValueList args({pInterpreter->newQObject(this)});
        pInterpreter->doFunction(object, func, args);
    }
}

void LobbyMenu::handleAccountMessage(quint64 socketID, const QJsonObject & objData)
{
    auto action = objData.value(JsonKeys::JSONKEY_RECEIVEACTION).toInt();
    auto accountError = objData.value(JsonKeys::JSONKEY_ACCOUNT_ERROR).toInt();
    if (action >= static_cast<qint32>(NetworkCommands::PublicKeyActions::CreateAccount) &&
        action <= static_cast<qint32>(NetworkCommands::PublicKeyActions::ChangePassword))
    {
        const char * const jsScripts[] =
        {
            "CreateAccountDialog",
            "UserLoginDialog",
            "ForgotPasswordDialog",
            "ChangePasswordDialog",
        };
        QString object = jsScripts[action - static_cast<qint32>(NetworkCommands::PublicKeyActions::CreateAccount)];
        CONSOLE_PRINT("Calling function " + object + ".onAccountMessage(" + QString::number(accountError) + ")", Console::eDEBUG);
        Interpreter* pInterpreter = Interpreter::getInstance();
        QJSValueList args;
        args.append(accountError);
        pInterpreter->doFunction(object, "onAccountMessage", args);
    }
    else
    {
        CONSOLE_PRINT("Unknown account message " + QString::number(action) + " received", Console::eDEBUG);
    }
}

bool LobbyMenu::getServerRequestNewPassword() const
{
    return m_serverRequestNewPassword;
}

void LobbyMenu::setServerRequestNewPassword(bool newServerRequestNewPassword)
{
    m_serverRequestNewPassword = newServerRequestNewPassword;
}

bool LobbyMenu::isValidEmailAdress(const QString & emailAdress)
{
    static const QRegularExpression regex("(?:[a-z0-9!#$%&'*+\\/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+\\/=?^_`{|}~-]+)*|\""
                                          "(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\x7f]"
                                          "|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[a-z0-9]"
                                          "(?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|"
                                          "\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]"
                                          "|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:"
                                          "[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21-\\x5a\\x53-\\x7f]|\\\\"
                                          "[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])");
    auto match = regex.match(emailAdress);
    return match.hasMatch() && match.capturedLength() == emailAdress.length();
}

bool LobbyMenu::isValidPassword(const QString & password)
{
    static const QRegularExpression regex("^(?=.*?[A-Z])(?=.*?[a-z])(?=.*?[0-9])(?=.*?[#?!@$%^&*-]).{8,}$");
    auto match = regex.match(password);
    return match.hasMatch();
}

void LobbyMenu::createServerAccount(const QString & password, const QString & emailAdress)
{
    m_serverPassword.setPassword(password);
    m_serverEmailAdress = emailAdress;
    Mainapp* pApp = Mainapp::getInstance();
    auto & cypher = pApp->getCypher();
    emit m_pTCPClient->sig_sendData(0, cypher.getRequestKeyMessage(NetworkCommands::PublicKeyActions::CreateAccount), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::onPublicKeyCreateAccount(quint64 socketID, const QJsonObject & objData, NetworkCommands::PublicKeyActions action)
{
    auto & cypher = Mainapp::getInstance()->getCypher();
    QJsonObject data;
    data.insert(JsonKeys::JSONKEY_PASSWORD, cypher.toJsonArray(m_serverPassword.getHash()));
    data.insert(JsonKeys::JSONKEY_EMAILADRESS, m_serverEmailAdress);
    data.insert(JsonKeys::JSONKEY_USERNAME, Settings::getUsername());
    QJsonDocument doc(data);
    QString publicKey = objData.value(JsonKeys::JSONKEY_PUBLICKEY).toString();
    emit m_pTCPClient->sig_sendData(socketID, cypher.getEncryptedMessage(publicKey, action, doc.toJson()).toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::onPublicKeyLoginAccount(quint64 socketID, const QJsonObject & objData, NetworkCommands::PublicKeyActions action)
{
    auto & cypher = Mainapp::getInstance()->getCypher();
    QJsonObject data;
    data.insert(JsonKeys::JSONKEY_PASSWORD, cypher.toJsonArray(m_serverPassword.getHash()));
    data.insert(JsonKeys::JSONKEY_USERNAME, Settings::getUsername());
    QJsonDocument doc(data);
    QString publicKey = objData.value(JsonKeys::JSONKEY_PUBLICKEY).toString();
    emit m_pTCPClient->sig_sendData(socketID, cypher.getEncryptedMessage(publicKey, action, doc.toJson()).toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::onPublicKeyResetAccount(quint64 socketID, const QJsonObject & objData, NetworkCommands::PublicKeyActions action)
{
    auto & cypher = Mainapp::getInstance()->getCypher();
    QJsonObject data;
    data.insert(JsonKeys::JSONKEY_EMAILADRESS, m_serverEmailAdress);
    data.insert(JsonKeys::JSONKEY_USERNAME, Settings::getUsername());
    QJsonDocument doc(data);
    QString publicKey = objData.value(JsonKeys::JSONKEY_PUBLICKEY).toString();
    emit m_pTCPClient->sig_sendData(socketID, cypher.getEncryptedMessage(publicKey, action, doc.toJson()).toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);

}

void LobbyMenu::onPublicKeyChangePassword(quint64 socketID, const QJsonObject & objData, NetworkCommands::PublicKeyActions action)
{
    auto & cypher = Mainapp::getInstance()->getCypher();
    QJsonObject data;
    data.insert(JsonKeys::JSONKEY_PASSWORD, cypher.toJsonArray(m_serverPassword.getHash()));
    data.insert(JsonKeys::JSONKEY_OLDPASSWORD, cypher.toJsonArray(m_oldServerPassword.getHash()));
    data.insert(JsonKeys::JSONKEY_USERNAME, Settings::getUsername());
    QJsonDocument doc(data);
    QString publicKey = objData.value(JsonKeys::JSONKEY_PUBLICKEY).toString();
    emit m_pTCPClient->sig_sendData(socketID, cypher.getEncryptedMessage(publicKey, action, doc.toJson()).toJson(), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::loginToServerAccount(const QString & password)
{
    m_serverPassword.setPassword(password);
    Mainapp* pApp = Mainapp::getInstance();
    auto & cypher = pApp->getCypher();
    emit m_pTCPClient->sig_sendData(0, cypher.getRequestKeyMessage(NetworkCommands::PublicKeyActions::LoginAccount), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::resetPasswordOnServerAccount(const QString & emailAdress)
{
    m_serverEmailAdress = emailAdress;
    Mainapp* pApp = Mainapp::getInstance();
    auto & cypher = pApp->getCypher();
    emit m_pTCPClient->sig_sendData(0, cypher.getRequestKeyMessage(NetworkCommands::PublicKeyActions::ResetPassword), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}

void LobbyMenu::changePasswordOnServerAccount(const QString & oldEmailAdress, const QString & newEmailAdress)
{
    m_oldServerPassword.setPassword(oldEmailAdress);
    m_serverPassword.setPassword(newEmailAdress);
    Mainapp* pApp = Mainapp::getInstance();
    auto & cypher = pApp->getCypher();
    emit m_pTCPClient->sig_sendData(0, cypher.getRequestKeyMessage(NetworkCommands::PublicKeyActions::ChangePassword), NetworkInterface::NetworkSerives::ServerHostingJson, false);
}
