#include "objects/dialogs/ingame/dialogvictoryconditions.h"
#include "objects/base/panel.h"
#include "objects/dialogs/ingame/victoryrulepopup.h"

#include "menue/gamemenue.h"
#include "menue/movementplanner.h"

#include "coreengine/mainapp.h"

#include "resource_management/objectmanager.h"

#include "resource_management/fontmanager.h"


#include "game/gamemap.h"
#include "game/gamerules.h"
#include "game/victoryrule.h"
#include "game/player.h"
#include "game/co.h"

DialogVictoryConditions::DialogVictoryConditions(GameMap* pMap)
    : m_pMap(pMap)
{
#ifdef GRAPHICSUPPORT
    setObjectName("DialogVictoryConditions");
#endif
    Mainapp* pApp = Mainapp::getInstance();
    moveToThread(pApp->getWorkerthread());
    ObjectManager* pObjectManager = ObjectManager::getInstance();
    oxygine::spBox9Sprite pSpriteBox = oxygine::spBox9Sprite::create();
    oxygine::ResAnim* pAnim = pObjectManager->getResAnim("codialog");
    pSpriteBox->setResAnim(pAnim);
    pSpriteBox->setSize(Settings::getWidth(), Settings::getHeight());
    addChild(pSpriteBox);
    pSpriteBox->setPosition(0, 0);
    pSpriteBox->setPriority(static_cast<qint32>(Mainapp::ZOrder::Objects));
    setPriority(static_cast<qint32>(Mainapp::ZOrder::Dialogs));

    // ok button
    m_OkButton = pObjectManager->createButton(tr("Ok"), 150);
    m_OkButton->setPosition(Settings::getWidth() / 2 - m_OkButton->getScaledWidth() / 2,
                            Settings::getHeight() - 30 - m_OkButton->getScaledHeight());
    pSpriteBox->addChild(m_OkButton);
    m_OkButton->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
        emit sigFinished();
    });

    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = true;

    oxygine::TextStyle headerStyle = oxygine::TextStyle(FontManager::getMainFont48());
    headerStyle.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    headerStyle.multiline = true;
    // no the fun begins create checkboxes and stuff and a panel down here
    spPanel pPanel = spPanel::create(true, QSize(Settings::getWidth() - 60, Settings::getHeight() - 110),
                                     QSize(Settings::getWidth() - 60, Settings::getHeight() - 110));
    pPanel->setPosition(30, 30);
    pSpriteBox->addChild(pPanel);

    
    GameRules* pRules = pMap->getGameRules();

    qint32 y = 10;
    oxygine::spTextField pTextfield = oxygine::spTextField::create();
    pTextfield->setStyle(headerStyle);
    pTextfield->setHtmlText(tr("Victory Info"));
    pTextfield->setPosition(Settings::getWidth() / 2 - pTextfield->getTextRect().getWidth(), y);
    pPanel->addItem(pTextfield);
    y += 60;
    QString info = pMap->getGameScript()->getVictoryInfo();

    pTextfield = oxygine::spTextField::create();
    pTextfield->setStyle(style);
    pTextfield->setHtmlText(info);
    pTextfield->setWidth(Settings::getWidth() - 90);
    pTextfield->setPosition(10, y);
    pPanel->addItem(pTextfield);
    y += 10 + pTextfield->getTextRect().getHeight();
    for (qint32 i = 0; i < pRules->getVictoryRuleSize(); i++)
    {
        VictoryRule* pVictoryRule = pRules->getVictoryRule(i);
        info = pVictoryRule->getRuleDescription();

        pTextfield = oxygine::spTextField::create();
        pTextfield->setStyle(style);
        pTextfield->setWidth(Settings::getWidth() - 90);
        pTextfield->setHtmlText(info);
        pTextfield->setPosition(10, y);
        pPanel->addItem(pTextfield);
        y += 30 + pTextfield->getTextRect().getHeight();

        qint32 x = 10;
        qint32 stepWidth = 250;
        for (qint32 i2 = 0; i2 < pMap->getPlayerCount(); i2++)
        {
            Player* pPlayer = pMap->getPlayer(i2);
            if (pPlayer->getIsDefeated() == false)
            {
                qint32 ruleValue = pVictoryRule->getRuleValue(0);
                auto ruleTypes = pVictoryRule->getRuleType();
                if (ruleTypes[0] == VictoryRule::checkbox)
                {
                    ruleValue = 0;
                }
                qint32 playerValue = pVictoryRule->getRuleProgress(pPlayer);
                info = tr("Player ") + QString::number(i2 + 1) + ": " + QString::number(playerValue) + "/" + QString::number(ruleValue);
                spBuilding building = spBuilding::create("HQ", pMap);
                building->setOwner(pPlayer);
                building->setPosition(x, y);
                pPanel->addItem(building);

                pTextfield = oxygine::spTextField::create();
                pTextfield->setStyle(style);
                pTextfield->setHtmlText(info);
                pTextfield->setPosition(x + GameMap::getImageSize() + 5, y - 15);
                pPanel->addItem(pTextfield);
                x += stepWidth;
                if (x + stepWidth > Settings::getWidth() - 90 && i2 < pMap->getPlayerCount() - 1)
                {
                    x = 10;
                    y += 60;
                }
            }
        }
        y += 40;
        oxygine::spButton pButton = pObjectManager->createButton(tr("Keep Track"), 150);
        pButton->setPosition(10, y);
        pPanel->addItem(pButton);
        pButton->addEventListener(oxygine::TouchEvent::CLICK, [this, pVictoryRule](oxygine::Event*)
        {
            emit sigShowPopup(pVictoryRule->getRuleID());
        });

        y += 40;
    }
    pPanel->setContentHeigth(y + 40);
    connect(this, &DialogVictoryConditions::sigShowPopup, this, &DialogVictoryConditions::showPopup, Qt::QueuedConnection);
    connect(this, &DialogVictoryConditions::sigFinished, this, &DialogVictoryConditions::remove, Qt::QueuedConnection);
}

void DialogVictoryConditions::remove()
{
    detach();
}

void DialogVictoryConditions::showPopup(QString rule)
{
    BaseGamemenu* pMenu = GameMenue::getInstance();
    if (pMenu != nullptr && !VictoryRulePopup::exists(rule))
    {
        spVictoryRulePopup pPopup = spVictoryRulePopup::create(m_pMap, rule, 180, 250);
        pPopup->setY(Settings::getHeight() - pPopup->getScaledHeight());
        pMenu->addChild(pPopup);
    }
}
