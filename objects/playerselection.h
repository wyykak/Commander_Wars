#ifndef PLAYERSELECTION_H
#define PLAYERSELECTION_H

#include <QObject>

#include "ui_reader/createdgui.h"

#include "network/NetworkInterface.h"

#include "gameinput/basegameinputif.h"

#include "game/gamemap.h"
#include "game/campaign.h"

class DropDownmenu;
class PlayerSelection;
using spPlayerSelection = oxygine::intrusive_ptr<PlayerSelection>;

class PlayerSelection final : public CreatedGui
{
    Q_OBJECT
public:
    explicit PlayerSelection(qint32 width, qint32 heigth);
    ~PlayerSelection() = default;
    void showPlayerSelection();    
    void attachNetworkInterface(spNetworkInterface pNetworkInterface);    
    void attachCampaign(spCampaign campaign);

    /**
     * @brief sendPlayerRequest sends a request to play as a human player
     * @param socketID
     * @param player -1 for any player
     */
    void sendPlayerRequest(quint64 socketID, qint32 player, GameEnums::AiTypes aiType);
    void playerDataChanged();
    /**
     * @brief sendPlayerReady
     * @param socketID
     * @param player
     * @param value
     */
    void sendPlayerReady(quint64 socketID, const QVector<qint32> & player, bool value);
    /**
     * @brief setSaveGame
     * @param value
     */
    void setSaveGame(bool value);
    void setMap(GameMap *newPMap);
signals:
    /**
     * @brief sigDisconnect emitted when we need to leave the game cause we don't own a player
     */
    void sigDisconnect();
public slots:
    bool hasNetworkInterface() const;
    bool getIsServerNetworkInterface() const;
    bool getIsObserverNetworkInterface() const;
    bool isNotServerChangeable(Player* pPlayer) const;
    /**
     * @brief getSaveGame
     * @return
     */
    bool getSaveGame() const;
    bool isOpenPlayer(qint32 player);
    bool isClosedPlayer(qint32 player);
    bool hasOpenPlayer();
    bool hasHumanPlayer();
    QString getPlayerAiName(qint32 player);
    void setPlayerAiName(qint32 player, QString name);
    GameEnums::AiTypes getPlayerAiType(qint32 player);
    bool getIsServerGame() const;
    void setIsServerGame(bool isServerGame);
    bool getIsCampaign() const;
    bool getIsArmyCustomizationAllowed();
    void updateCOData(qint32 playerIdx);
    bool getReady(qint32 playerIdx);
    bool getPlayerReady();
    void setPlayerReady(bool value);
    QStringList getDefaultAiNames() const;
    QStringList getAiNames() const;
    QStringList getTeamNames() const;
    QStringList getDropDownColorNames() const;
    /**
     * @brief getSelectableArmies
     * @return
     */
    QStringList getSelectableArmies() const;
    /**
     * @brief getStartColorName
     * @param player
     * @return
     */
    QString getStartColorName(qint32 player);
    // slots for automation
    /**
     * @brief selectPlayerAi
     * @param player
     * @param eAiType
     */
    void selectPlayerAi(qint32 player, GameEnums::AiTypes eAiType);
    // slots for changing player data
    void allPlayerIncomeChanged(float value);
    void playerIncomeChanged(float value, qint32 playerIdx);
    void allPlayerStartFundsChanged(float value);
    void playerStartFundsChanged(float value, qint32 playerIdx);
    void playerTeamChanged(qint32 value, qint32 playerIdx);
    void playerColorChanged(QColor displayColor, qint32 playerIdx, qint32 item);
    void selectedArmyChanged(qint32 player, QString army);
    void showSelectCO(qint32 player, quint8 co);
    void playerCO1Changed(QString coid, qint32 playerIdx);
    void updateCO1Sprite(QString coid, qint32 playerIdx);
    void playerCO2Changed(QString coid, qint32 playerIdx);
    void updateCO2Sprite(QString coid, qint32 playerIdx);
    void slotCOsRandom(qint32 mode);
    void slotShowAllBuildList();
    void slotShowPlayerBuildList(qint32 player);
    void slotChangeAllBuildList(qint32, QStringList buildList);
    void slotChangePlayerBuildList(qint32 player, QStringList buildList);
    void selectAI(qint32 player);
    GameMap *getMap() const;
    /**
     * @brief changeTeams
     */
    void changeAllTeams(qint32 value);
    /**
     * @brief showSelectCOPerks
     * @param player
     */
    void showSelectCOPerks(qint32 player);
    // network stuff
    void recieveData(quint64 socketID, QByteArray data, NetworkInterface::NetworkSerives service);
    void disconnected(quint64 socketID);
    /**
     * @brief updatePlayerData updates the visual player data when data has been changed via network
     * @param player the player that has changed
     */
    void updatePlayerData(qint32 player);
    void setPlayerAi(qint32 player, GameEnums::AiTypes eAiType, QString name);
    /**
     * @brief recievePlayerReady
     * @param socketID
     * @param stream
     */
    void recievePlayerReady(quint64 socketID, QDataStream& stream);
protected:
    /**
     * @brief createAi
     * @param player
     * @param type
     */
    void createAi(qint32 player, GameEnums::AiTypes type, QString displayName);
    /**
     * @brief requestPlayer a client requested to get control of a player. We check the request and execute it if it's valid
     * @param socketID
     * @param stream
     */
    void requestPlayer(quint64 socketID, QDataStream& stream);
    /**
     * @brief changePlayer changes the player data on client sites
     * @param socketID
     * @param stream
     */
    void changePlayer(quint64 socketID, QDataStream& stream);
    /**
     * @brief recievedPlayerData
     * @param socketID
     * @param stream
     */
    void recievedPlayerData(quint64 socketID, QDataStream& stream);
    /**
     * @brief PlayerSelection::recievedCOData
     * @param stream
     */
    void recievedCOData(quint64, QDataStream& stream);
    /**
     * @brief recievedColorData
     * @param stream
     */
    void recievedColorData(quint64, QDataStream& stream);

    /**
     * @brief recievePlayerServerReady
     * @param socketID
     * @param stream
     */
    void recievePlayerServerReady(quint64 socketID, QDataStream& stream);
    /**
     * @brief recievePlayerArmy
     * @param socketID
     * @param stream
     */
    void recievePlayerArmy(quint64 socketID, QDataStream& stream);
    /**
     * @brief joinAllowed
     * @param socketId
     * @param username
     * @param eAiType
     * @return
     */
    bool joinAllowed(quint64 socketId, QString username, GameEnums::AiTypes eAiType);
    /**
     * @brief sendOpenPlayerCount
     */
    void sendOpenPlayerCount();

    void createPlayerChangedData(QByteArray & data, quint64 socketId, QString name, qint32 aiType, qint32 player, bool clientRequest);
    /**
     * @brief getDefaultColorCount
     * @return
     */
    qint32 getDefaultColorCount() const;
    /**
     * @brief getDefaultColor
     * @param index
     * @return
     */
    QColor getDefaultColor(qint32 index);
    /**
     * @brief getDisplayColor
     * @param index
     * @param exists
     * @return
     */
    QColor getDisplayColor(qint32 index, bool & exists) const;
    /**
     * @brief tableColorToDisplayColor
     * @param tableColor
     * @return
     */
    QColor tableColorToDisplayColor(QColor tableColor);
    /**
     * @brief displayColorToTableColor
     * @param displayColor
     * @return
     */
    QColor displayColorToTableColor(QColor displayColor);
    /**
     * @brief autoSelectPlayerColors
     */
    void autoSelectPlayerColors();
    /**
     * @brief playerAccessDenied
     */
    void playerAccessDenied();
    /**
     * @brief joinObserver
     */
    void joinObserver(quint64 socketID);
    /**
     * @brief initializeMap sets predefined stuff and fixes old maps
     */
    void initializeMap();
    /**
     * @brief updateInitialState
     */
    void updateInitialState();
    /**
     * @brief selectInitialCos
     * @param player
     */
    void selectInitialCos(qint32 player);
    /**
     * @brief selectInitialAi
     * @param pPlayerAi
     * @param ai
     * @param aiList
     */
    void selectInitialAi(qint32 player, DropDownmenu* pPlayerAi, qint32 & ai, const QStringList & aiList, const QStringList & defaultAiList);
    /**
     * @brief createInitialAi
     * @param pPlayerAi
     * @param ai
     * @param player
     */
    void createInitialAi(DropDownmenu* pPlayerAi, qint32 ai, qint32 player);
private:
    /**
     * @brief m_PlayerSockets holds which socket owns which player
     * For clients and local games this always contains 0
     * For the host this contains 0 for owned by the server or the socket id for client owned
     */
    QVector<quint64> m_playerSockets;
    QVector<bool> m_playerReadyFlags;

    spNetworkInterface m_pNetworkInterface{nullptr};
    spCampaign m_pCampaign;

    bool m_saveGame{false};
    bool m_playerReady{false};
    bool m_isServerGame{false};
    GameMap* m_pMap{nullptr};
};

Q_DECLARE_INTERFACE(PlayerSelection, "PlayerSelection");

#endif // PLAYERSELECTION_H
