#include "actionlistdialog.h"

#include "qdiriterator.h"

#include "coreengine/filesupport.h"

#include "resource_management/objectmanager.h"

#include "resource_management/fontmanager.h"

#include "resource_management/gamemanager.h"

#include "game/gamemap.h"

#include "objects/base/panel.h"

#include "objects/dialogs/dialogtextinput.h"
#include "objects/base/label.h"

ActionListDialog::ActionListDialog(QStringList bannlist, GameMap* pMap)
    : m_CurrentActionList(bannlist),
      m_pMap(pMap)
{
#ifdef GRAPHICSUPPORT
    setObjectName("ActionListDialog");
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
    m_OkButton->setPosition(Settings::getWidth() - m_OkButton->getScaledWidth() - 30,
                            Settings::getHeight() - 30 - m_OkButton->getScaledHeight());
    pSpriteBox->addChild(m_OkButton);
    m_OkButton->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
        emit editFinished(m_CurrentActionList);
        emit sigFinished();
    });

    // cancel button
    m_ExitButton = pObjectManager->createButton(tr("Cancel"), 150);
    m_ExitButton->setPosition(30, Settings::getHeight() - 30 - m_ExitButton->getScaledHeight());
    pSpriteBox->addChild(m_ExitButton);
    m_ExitButton->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
        emit canceled();
    });

    oxygine::spButton pSave = pObjectManager->createButton(tr("Save"), 150);
    pSave->setPosition(Settings::getWidth() / 2 - pSave->getScaledWidth() / 2,
                       Settings::getHeight() - 30 - m_ExitButton->getScaledHeight());
    pSave->addClickListener([this](oxygine::Event*)
    {
        emit sigShowSaveBannlist();
    });
    pSpriteBox->addChild(pSave);
    connect(this, &ActionListDialog::sigShowSaveBannlist, this, &ActionListDialog::showSaveBannlist, Qt::QueuedConnection);

    m_ToggleAll = pObjectManager->createButton(tr("Un/Select All"), 180);
    m_ToggleAll->setPosition(Settings::getWidth() / 2 + 60 ,
                             Settings::getHeight() - 75 - m_ToggleAll->getScaledHeight());
    pSpriteBox->addChild(m_ToggleAll);
    m_ToggleAll->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
        m_toggle = !m_toggle;
        for (qint32 i = 0; i < m_Checkboxes.size(); i++)
        {
            m_Checkboxes[i]->setChecked(m_toggle);
            emit m_Checkboxes[i]->checkChanged(m_toggle);
        }
    });
    auto items = getNameList();
    m_PredefinedLists = spDropDownmenu::create(300, items);

    m_PredefinedLists->setPosition(Settings::getWidth() / 2 + 40 - m_PredefinedLists->getScaledWidth(),
                                   Settings::getHeight() - 75 - m_ToggleAll->getScaledHeight());
    pSpriteBox->addChild(m_PredefinedLists);
    connect(m_PredefinedLists.get(), &DropDownmenu::sigItemChanged, this, &ActionListDialog::setBuildlist, Qt::QueuedConnection);



    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;
    // no the fun begins create checkboxes and stuff and a panel down here
    spPanel pPanel = spPanel::create(true, QSize(Settings::getWidth() - 60, Settings::getHeight() - 150),
                               QSize(Settings::getWidth() - 60, Settings::getHeight() - 150));
    pPanel->setPosition(30, 30);
    pSpriteBox->addChild(pPanel);

    oxygine::TextStyle headerStyle = oxygine::TextStyle(FontManager::getMainFont48());
    headerStyle.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    headerStyle.multiline = false;
    spLabel pLabel = spLabel::create(pPanel->getScaledWidth() - 60);
    pLabel->setStyle(headerStyle);
    pLabel->setHtmlText(tr("Action List"));
    pLabel->setPosition(pPanel->getScaledWidth() / 2 - pLabel->getTextRect().getWidth() / 2, 10);
    pPanel->addItem(pLabel);

    GameManager* pGameManager = GameManager::getInstance();

    QStringList actionList = pGameManager->getLoadedRessources();

    qint32 y = 30 + pLabel->getTextRect().getHeight() * 2;
    qint32 x = 10;
    
    m_CurrentActionList = m_pMap->getGameRules()->getAllowedActions();

    for (qint32 i = 0; i < actionList.size(); i++)
    {
        QString actionId = actionList[i];
        QString icon = pGameManager->getActionIcon(actionId);
        if (!icon.isEmpty())
        {
            pLabel = spLabel::create(250);
            pLabel->setStyle(style);
            pLabel->setHtmlText(pGameManager->getName(i));
            pLabel->setPosition(x + 90, y);
            QString tooltip = pGameManager->getDescription(i);

            spTooltip pTooltip = spTooltip::create();

            oxygine::spSprite pSprite = pGameManager->getIcon(pMap, icon);
            pSprite->setScale(1.25f * pSprite->getScaleX());
            pTooltip->addChild(pSprite);
            pTooltip->setPosition(x + 45, y);
            pTooltip->setTooltipText(tooltip);

            spCheckbox pCheckbox = spCheckbox::create();
            pCheckbox->setPosition(x, y);
            pCheckbox->setTooltipText(tooltip);
            m_Checkboxes.append(pCheckbox);

            if (m_CurrentActionList.contains(actionId))
            {
                pCheckbox->setChecked(true);
            }
            else
            {
                pCheckbox->setChecked(false);
            }
            connect(pCheckbox.get(), &Checkbox::checkChanged, this, [this, actionId](bool checked)
            {
                if (checked)
                {
                    m_CurrentActionList.append(actionId);
                }
                else
                {
                    m_CurrentActionList.removeAll(actionId);
                }
            });

            pPanel->addItem(pCheckbox);
            pPanel->addItem(pLabel);
            pPanel->addItem(pTooltip);

            x += 350;
            if (x + 350 > pPanel->getContentWidth())
            {
                y += 40;
                x = 10;
            }
        }
    }
    pPanel->setContentHeigth(y + 50);
    connect(this, &ActionListDialog::canceled, this, &ActionListDialog::remove, Qt::QueuedConnection);
    connect(this, &ActionListDialog::sigFinished, this, &ActionListDialog::remove, Qt::QueuedConnection);
}

void ActionListDialog::remove()
{
    m_Checkboxes.clear();
    detach();
}

QStringList ActionListDialog::getNameList()
{
    QStringList items;
    QStringList filters;
    filters << "*.bl";
    QDirIterator dirIter("data/actionbannlist/", filters, QDir::Files, QDirIterator::IteratorFlag::NoIteratorFlags);
    while (dirIter.hasNext())
    {
        dirIter.next();
        QString file = dirIter.fileInfo().canonicalFilePath();
        auto data = Filesupport::readList(file);
        items.append(data.name);
    }
    return items;
}

void ActionListDialog::setBuildlist(qint32)
{
    QStringList data;
    QString file = m_PredefinedLists->getCurrentItemText();
    auto fileData = Filesupport::readList(file + ".bl", "data/actionbannlist/");
    data = fileData.items;
    m_CurrentActionList = data;
}

void ActionListDialog::showSaveBannlist()
{    
    spDialogTextInput pSaveInput = spDialogTextInput::create(tr("Bannlist Name"), true, "");
    connect(pSaveInput.get(), &DialogTextInput::sigTextChanged, this, &ActionListDialog::saveBannlist, Qt::QueuedConnection);
    addChild(pSaveInput);    
}

void ActionListDialog::saveBannlist(QString filename)
{    
    Filesupport::storeList(filename, m_CurrentActionList, "data/actionbannlist/");
    auto items = getNameList();
    m_PredefinedLists->changeList(items);
}
