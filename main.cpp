#include <QObject>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QFile>

#include "3rd_party/oxygine-framework/oxygine/KeyEvent.h"

#include "coreengine/audiothread.h"
#include "coreengine/mainapp.h"
#include "coreengine/userdata.h"
#include "coreengine/console.h"
#include "coreengine/scriptvariable.h"
#include "coreengine/scriptvariables.h"
#include "coreengine/scriptvariablefile.h"
#include "coreengine/crashreporter.h"

#include "menue/ingamemenue.h"
#include "menue/victorymenue.h"
#include "menue/mainwindow.h"
#include "menue/victorymenue.h"
#include "menue/gamemenue.h"
#include "menue/mapselectionmapsmenue.h"
#include "menue/campaignmenu.h"
#include "menue/editormenue.h"

#include "game/terrain.h"
#include "game/player.h"
#include "game/building.h"
#include "game/unit.h"
#include "game/co.h"
#include "game/gameaction.h"
#include "game/gameanimation/gameanimation.h"
#include "game/gameanimation/gameanimationwalk.h"
#include "game/gameanimation/gameanimationcapture.h"
#include "game/gameanimation/gameanimationdialog.h"
#include "game/gameanimation/gameanimationpower.h"
#include "game/gameanimation/gameanimationnextday.h"
#include "game/victoryrule.h"
#include "game/gamerules.h"
#include "game/gamerule.h"
#include "game/weather.h"
#include "game/terrainfindingsystem.h"
#include "game/campaign.h"
#include "game/gamescript.h"
#include "game/GameEnums.h"

#include "wiki/wikidatabase.h"

#include "objects/base/textbox.h"
#include "objects/playerselection.h"

#include "gameinput/cursordata.h"
#include "gameinput/basegameinputif.h"

#include "ingamescriptsupport/events/scriptevent.h"
#include "ingamescriptsupport/conditions/scriptcondition.h"

#include "network/mainserver.h"

#include "ai/heavyai.h"

#include "resource_management/cospritemanager.h"
#include "resource_management/terrainmanager.h"
#include "resource_management/buildingspritemanager.h"
#include "resource_management/coperkmanager.h"
#include "resource_management/unitspritemanager.h"

void registerInterfaceData()
{
    // qt metatypes we need this for js and signal slot stuff
    qRegisterMetaType<NetworkInterface::NetworkSerives>("NetworkInterface::NetworkSerives");
    qRegisterMetaType<VictoryMenue::GraphModes>("VictoryMenue::GraphModes");
    qRegisterMetaType<GameEnums::Alliance>("GameEnums::Alliance");
    qRegisterMetaType<GameEnums::UnitRanks>("GameEnums::UnitRanks");
    qRegisterMetaType<GameEnums::PowerMode>("GameEnums::PowerMode");
    qRegisterMetaType<GameEnums::RocketTarget>("GameEnums::RocketTarget");
    qRegisterMetaType<GameEnums::DefeatType>("GameEnums::DefeatType");
    qRegisterMetaType<GameEnums::Directions>("GameEnums::Directions");
    qRegisterMetaType<GameEnums::AIQuestionType>("GameEnums::AIQuestionType");
    qRegisterMetaType<GameEnums::Fog>("GameEnums::Fog");
    qRegisterMetaType<GameEnums::COMood>("GameEnums::COMood");
    qRegisterMetaType<GameEnums::LuckDamageMode>("GameEnums::LuckDamageMode");
    qRegisterMetaType<GameEnums::GameRecord_SpecialEvents>("GameEnums::GameRecord_SpecialEvents");
    qRegisterMetaType<GameEnums::GameAi>("GameEnums::GameAi");
    qRegisterMetaType<GameEnums::AnimationMode>("GameEnums::AnimationMode");
    qRegisterMetaType<GameEnums::BuildingTarget>("GameEnums::BuildingTarget");
    qRegisterMetaType<GameEnums::BattleAnimationMode>("GameEnums::BattleAnimationMode");
    qRegisterMetaType<GameEnums::UnitType>("GameEnums::UnitType");
    qRegisterMetaType<GameEnums::VisionType>("GameEnums::VisionType");
    qRegisterMetaType<GameEnums::AiTypes>("GameEnums::AiTypes");
    qRegisterMetaType<GameEnums::Recoloring>("GameEnums::Recoloring");
    qRegisterMetaType<GameEnums::COInfoPosition>("GameEnums::COInfoPosition");
    qRegisterMetaType<GameEnums::WeaponType>("GameEnums::WeaponType");
    qRegisterMetaType<GameEnums::ShopItemType>("GameEnums::ShopItemType");
    qRegisterMetaType<Console::eLogLevels>("Console::eLogLevels");
    qRegisterMetaType<Mainapp::StartupPhase>("Mainapp::StartupPhase");
    qRegisterMetaType<std::shared_ptr<QTcpSocket>>("std::shared_ptr<QTcpSocket>");
    qRegisterMetaType<spScriptEvent>("spScriptEvent");
    qRegisterMetaType<spScriptCondition>("spScriptCondition");
    qRegisterMetaType<WikiDatabase::pageData>("WikiDatabase::pageData");
    qRegisterMetaType<oxygine::spActor>("oxygine::spActor");
    qRegisterMetaType<oxygine::spResAnim>("oxygine::spResAnimf");
    qRegisterMetaType<oxygine::KeyEvent>("oxygine::KeyEvent");
    qRegisterMetaType<oxygine::MouseButton>("oxygine::MouseButton");
    qRegisterMetaType<QVector<std::tuple<QString,float>>>("QVector<std::tuple<QString,float>>");
    qRegisterMetaType<QVector<float>>("QVector<float>");
    qRegisterMetaType<QList<qint32>>("QList<qint32>");
    qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
    qRegisterMetaType<spTextbox>("spTextbox");
    qRegisterMetaType<spGameAction>("spGameAction");

    qmlRegisterInterface<QmlVectorPoint>("QmlVectorPoint", 1);
    qmlRegisterInterface<Terrain>("Terrain", 1);
    qmlRegisterInterface<Player>("Player", 1);
    qmlRegisterInterface<Building>("Building", 1);
    qmlRegisterInterface<Unit>("Unit", 1);
    qmlRegisterInterface<CO>("CO", 1);
    qmlRegisterInterface<GameAction>("GameAction", 1);
    qmlRegisterInterface<GameAnimation>("GameAnimation", 1);
    qmlRegisterInterface<GameAnimationWalk>("GameAnimationWalk", 1);
    qmlRegisterInterface<GameAnimationCapture>("GameAnimationCapture", 1);
    qmlRegisterInterface<GameAnimationDialog>("GameAnimationDialog", 1);
    qmlRegisterInterface<GameAnimationPower>("GameAnimationPower", 1);
    qmlRegisterInterface<GameAnimationNextDay>("GameAnimationNextDay", 1);
    qmlRegisterInterface<QmlVectorPoint>("QmlVectorPoint", 1);
    qmlRegisterInterface<QmlVectorUnit>("QmlVectorUnit", 1);
    qmlRegisterInterface<QmlVectorBuilding>("QmlVectorBuilding", 1);
    qmlRegisterInterface<Mainapp>("Mainapp", 1);
    qmlRegisterInterface<CursorData>("CursorData", 1);
    qmlRegisterInterface<VictoryRule>("VictoryRule", 1);
    qmlRegisterInterface<GameRules>("GameRules", 1);
    qmlRegisterInterface<GameRule>("GameRule", 1);
    qmlRegisterInterface<ScriptVariable>("ScriptVariable", 1);
    qmlRegisterInterface<ScriptVariables>("ScriptVariables", 1);
    qmlRegisterInterface<ScriptVariableFile>("ScriptVariableFile", 1);
    qmlRegisterInterface<Weather>("Weather", 1);
    qmlRegisterInterface<TerrainFindingSystem>("TerrainFindingSystem", 1);
    qmlRegisterInterface<GameRecorder>("GameRecorder", 1);
    qmlRegisterInterface<GameScript>("GameScript", 1);
    qmlRegisterInterface<Campaign>("Campaign", 1);
    qmlRegisterInterface<BaseGameInputIF>("BaseGameInputIF", 1);
    qmlRegisterInterface<Settings>("Settings", 1);
    qmlRegisterInterface<Wikipage>("Wikipage", 1);
    qmlRegisterInterface<HeavyAi>("HeavyAi", 1);
    qmlRegisterInterface<NeuralNetwork>("NeuralNetwork", 1);
    qmlRegisterInterface<NetworkGame>("NetworkGame", 1);
    qmlRegisterInterface<Mainwindow>("Mainwindow", 1);
    qmlRegisterInterface<VictoryMenue>("VictoryMenue", 1);
    qmlRegisterInterface<GameMenue>("GameMenue", 1);
    qmlRegisterInterface<CampaignMenu>("CampaignMenu", 1);
    qmlRegisterInterface<EditorMenue>("EditorMenue", 1);
    qmlRegisterInterface<MapSelectionMapsMenue>("MapSelectionMapsMenue", 1);
    qmlRegisterInterface<PlayerSelection>("PlayerSelection", 1);
    qmlRegisterInterface<COSpriteManager>("COSpriteManager", 1);
    qmlRegisterInterface<UnitSpriteManager>("UnitSpriteManager", 1);
    qmlRegisterInterface<BuildingSpriteManager>("BuildingSpriteManager", 1);
    qmlRegisterInterface<TerrainManager>("TerrainManager", 1);
    qmlRegisterInterface<COPerkManager>("COPerkManager", 1);
}

int main(qint32 argc, char* argv[])
{
    qInstallMessageHandler(Console::messageOutput);
    srand(static_cast<unsigned>(time(nullptr)));
    QThread::currentThread()->setPriority(QThread::TimeCriticalPriority);

    QApplication app(argc, argv);
    app.setApplicationName("Commander Wars");
    app.setApplicationVersion(Mainapp::getGameVersion());

    Settings::loadSettings();

    NeuralNetwork network;
    QMap<QString, double> parameters;
    parameters.insert(Layer::LAYER_PARAMETER_TYPE, static_cast<double>(Layer::LayerType::INPUT));
    parameters.insert(Layer::LAYER_PARAMETER_ACTIVATION, static_cast<double>(Neuron::ActivationFunction::Step));
    parameters.insert(Layer::LAYER_PARAMETER_SIZE, static_cast<double>(10));
    network.addLayer(parameters);
    parameters.insert(Layer::LAYER_PARAMETER_TYPE, static_cast<double>(Layer::LayerType::STANDARD));
    for (qint32 i = 0; i < 4; ++i)
    {
        network.addLayer(parameters);
    }
    parameters.insert(Layer::LAYER_PARAMETER_TYPE, static_cast<double>(Layer::LayerType::OUTPUT));
    parameters[Layer::LAYER_PARAMETER_SIZE] = 1;
    network.addLayer(parameters);
    network.autogenerate();

    Mainapp window;
    window.setTitle("Commander Wars");
    QStringList args = app.arguments();
    window.loadArgs(args);
    // start crash report handler
    crashReporter::setSignalHandler(&Mainapp::showCrashReport);
    registerInterfaceData();
    /*************************************************************************************************/
    // show window according to window mode
    window.changeScreenMode(window.getScreenMode());

    window.setBrightness(Settings::getBrightness());
    window.setGamma(Settings::getGamma());
    window.setPosition(Settings::getX(), Settings::getY());
    qint32 returncode = app.exec();
    /*************************************************************************************************/
    // shutting down
    Settings::setX(window.x());
    Settings::setY(window.y());
    crashReporter::setSignalHandler(nullptr);
    window.setShuttingDown(true);
    Player::releaseStaticData();
    if (GameMenue::getInstance() != nullptr)
    {
        GameMenue::getInstance()->deleteMenu();
    }
    if (EditorMenue::getInstance() != nullptr)
    {
        EditorMenue::getInstance()->deleteMenu();
    }
    if (GameMap::getInstance() != nullptr)
    {
        GameMap::getInstance()->deleteMap();
    }
    window.getWorkerthread()->quit();
    window.getWorkerthread()->wait();
    Console::print("Shutting down main window", Console::eDEBUG);
    window.shutdown();
    Console::print("Saving settings and shutting them down", Console::eDEBUG);
    Settings::saveSettings();
    Settings::shutdown();

    if (MainServer::getInstance() != nullptr)
    {
        Console::print("Shutting game server", Console::eDEBUG);
        MainServer::getInstance()->deleteLater();
        window.getGameServerThread()->quit();
        window.getGameServerThread()->wait();
    }
    //end
    if (returncode == 1)
    {
#ifdef Q_OS_ANDROID
        Console::print("No automatic restart on android", Console::eDEBUG);
#else
        Console::print("Restarting application", Console::eDEBUG);
        QProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList());
#endif
    }
    return returncode;
}
