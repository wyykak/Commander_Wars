#include "coreengine/mainapp.h"
#include "coreengine/globalutils.h"

#include "resource_management/objectmanager.h"
#include "resource_management/terrainmanager.h"
#include "resource_management/fontmanager.h"

#include "dialogmodifyterrain.h"

#include "game/gamemap.h"

#include "objects/dialogs/filedialog.h"
#include "objects/base/label.h"
#include "objects/base/checkbox.h"

DialogModifyTerrain::DialogModifyTerrain(GameMap* pMap, Terrain* pTerrain)
    : m_pTerrain(pTerrain),
      m_pMap(pMap)
{
#ifdef GRAPHICSUPPORT
    setObjectName("DialogModifyTerrain");
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

    m_pPanel = spPanel::create(true, QSize(Settings::getWidth() - 60, Settings::getHeight() - 110),
                                     QSize(Settings::getWidth() - 60, Settings::getHeight() - 110));
    m_pPanel->setPosition(30, 30);
    pSpriteBox->addChild(m_pPanel);

    qint32 y = 20;
    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;
    oxygine::TextStyle style2 = oxygine::TextStyle(FontManager::getMainFont48());

    spLabel pLabel = spLabel::create(m_pPanel->getScaledWidth() - 50);
    pLabel->setStyle(style2);
    pLabel->setHtmlText(tr("Information"));
    pLabel->setPosition(10, y);
    m_pPanel->addItem(pLabel);
    y += 60;

    pLabel = spLabel::create(190);
    pLabel->setStyle(style);
    pLabel->setHtmlText(tr("Name:"));
    pLabel->setPosition(10, y);
    m_pPanel->addItem(pLabel);
    spTextbox pTextbox = spTextbox::create(m_pPanel->getContentWidth() - 100 - 200 - pLabel->getScaledWidth());
    pTextbox->setTooltipText(tr("Custom Name of the Terrain. Empty name equals the default name."));
    pTextbox->setPosition(200 + 20 + pLabel->getX(), y);
    pTextbox->setCurrentText(pTerrain->getTerrainName());
    connect(pTextbox.get(), &Textbox::sigTextChanged, pTerrain, &Terrain::setTerrainName, Qt::QueuedConnection);
    m_pPanel->addItem(pTextbox);
    y += 40;

    pLabel = spLabel::create(190);
    pLabel->setStyle(style);
    pLabel->setHtmlText(tr("Description:"));
    pLabel->setPosition(10, y);
    pTextbox = spTextbox::create(m_pPanel->getContentWidth() - 100 - 200 - pLabel->getScaledWidth());
    pTextbox->setTooltipText(tr("Custom Description of the Terrain. Empty description equals the default description."));
    pTextbox->setPosition(200 + 20 + pLabel->getX(), y);
    pTextbox->setCurrentText(pTerrain->getTerrainDescription());
    connect(pTextbox.get(), &Textbox::sigTextChanged, pTerrain, &Terrain::setTerrainDescription, Qt::QueuedConnection);
    m_pPanel->addItem(pTextbox);
    m_pPanel->addItem(pLabel);
    y += 60;

    loadBaseImageview(y, pTerrain);
    loadOverlayview(y, pTerrain);

    oxygine::spButton pButtonDefault = pObjectManager->createButton(tr("Default"), 150);
    pButtonDefault->setPosition(10, y);
    m_pPanel->addChild(pButtonDefault);
    pButtonDefault->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
         emit sigTerrainClicked("");
    });

    oxygine::spButton pButtonSelect = pObjectManager->createButton(tr("Select Image"), 150);
    pButtonSelect->setPosition(10 + 20 + pButtonDefault->getScaledWidth(), y);
    m_pPanel->addChild(pButtonSelect);
    pButtonSelect->addEventListener(oxygine::TouchEvent::CLICK, [this](oxygine::Event*)
    {
         emit sigShowLoadDialog();
    });

    connect(this, &DialogModifyTerrain::sigTerrainClicked, this, &DialogModifyTerrain::terrainClicked, Qt::QueuedConnection);
    connect(this, &DialogModifyTerrain::sigShowLoadDialog, this, &DialogModifyTerrain::showLoadDialog, Qt::QueuedConnection);

    m_pPanel->setContentHeigth(y + 60);
    if (pTerrain->getFixedSprite())
    {
        m_pTextbox->setCurrentText(pTerrain->getTerrainSpriteName());
    }
    else
    {
        m_pTextbox->setCurrentText("");
    }
    connect(this, &DialogModifyTerrain::sigFinished, this, &DialogModifyTerrain::remove, Qt::QueuedConnection);
}

void DialogModifyTerrain::loadBaseImageview(qint32 & y, Terrain* pTerrain)
{
    qint32 x = 20;
    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;
    oxygine::TextStyle style2 = oxygine::TextStyle(FontManager::getMainFont48());

    QStringList pTerrainStyles = pTerrain->getTerrainSprites();
    spLabel pLabel = spLabel::create(m_pPanel->getScaledWidth() - 50);
    pLabel->setStyle(style2);
    pLabel->setHtmlText(tr("Base image"));
    pLabel->setPosition(10, y);
    m_pPanel->addItem(pLabel);
    y += 60;

    TerrainManager* pTerrainManager = TerrainManager::getInstance();
    for (qint32 i = 0; i < pTerrainStyles.size(); i++)
    {
        QString id = pTerrainStyles[i];
        oxygine::ResAnim* pAnim = pTerrainManager->getResAnim(id, oxygine::error_policy::ep_ignore_error);
        if (pAnim != nullptr)
        {
            spTerrain pSprite = Terrain::createTerrain(pTerrain->getBaseTerrainID(), -2, -2, "", m_pMap);
            pSprite->setFixedSprite(true);
            pSprite->setTerrainSpriteName(id);
            pSprite->loadSprites();
            pSprite->setScale(2);
            pSprite->setPosition(x, y);
            pSprite->addClickListener([this, id](oxygine::Event*)
            {
                emit sigTerrainClicked(id);
            });
            m_pPanel->addItem(pSprite);

            x += GameMap::getImageSize() * 3 ;
            if (x > m_pPanel->getContentWidth() - 100 - pSprite->getScaledWidth() && i < pTerrainStyles.size() - 1)
            {
                x = 20;
                y += GameMap::getImageSize() * 3;
            }
        }
    }
    y += GameMap::getImageSize() * 3;

    spLabel pTextfield = spLabel::create(180);
    pTextfield->setStyle(style);
    pTextfield->setHtmlText(tr("Terrain Style"));
    pTextfield->setPosition(10, y);
    m_pPanel->addItem(pTextfield);
    m_pTextbox = spTextbox::create(m_pPanel->getContentWidth() - 100 - 200 - pTextfield->getScaledWidth());
    m_pTextbox->setTooltipText(tr("Current select terrain image or terrain path or empty for default selection."));
    m_pTextbox->setPosition(200 + 20 + pTextfield->getX(), y);
    m_pPanel->addItem(m_pTextbox);
    y += 80;
}

void DialogModifyTerrain::loadOverlayview(qint32 & y, Terrain* pTerrain)
{
    QStringList overlayTerrainStyles = pTerrain->getOverlayTerrainSprites();
    if (overlayTerrainStyles.size() > 0)
    {
        connect(this, &DialogModifyTerrain::sigOverlayChanged, this, &DialogModifyTerrain::overlayChanged, Qt::QueuedConnection);
        qint32 x = 20;
        oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
        style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
        style.multiline = false;
        oxygine::TextStyle style2 = oxygine::TextStyle(FontManager::getMainFont48());

        QStringList selectedOverlayTerrainStyles = pTerrain->getCustomOverlays();
        spLabel pLabel = spLabel::create(m_pPanel->getScaledWidth() - 50);
        pLabel->setStyle(style2);
        pLabel->setHtmlText(tr("Overlays"));
        pLabel->setPosition(10, y);
        m_pPanel->addItem(pLabel);
        y += 60;

        TerrainManager* pTerrainManager = TerrainManager::getInstance();
        for (qint32 i = 0; i < overlayTerrainStyles.size(); i++)
        {
            QString id = overlayTerrainStyles[i];
            oxygine::ResAnim* pAnim = pTerrainManager->getResAnim(id, oxygine::error_policy::ep_ignore_error);
            if (pAnim != nullptr)
            {
                constexpr qint32 scale = 2;
                oxygine::spColorRectSprite pRect = oxygine::spColorRectSprite::create();
                pRect->setPosition(x, y);
                pRect->setSize(GameMap::getImageSize() * scale, GameMap::getImageSize() * scale);
                pRect->setColor(QColor(100, 100, 100, 100));

                oxygine::spSprite pSprite = oxygine::spSprite::create();
                pSprite->setResAnim(pAnim);
                pSprite->setSize(pAnim->getSize());
                pSprite->setScale(GameMap::getImageSize() * scale / pAnim->getWidth());
                pRect->addChild(pSprite);
                m_pPanel->addItem(pRect);

                bool selected = selectedOverlayTerrainStyles.contains(id);
                spCheckbox pCheckbox = spCheckbox::create();
                pCheckbox->setPosition(x + pRect->getScaledWidth() + 10, y + pRect->getScaledHeight() / 2 - pCheckbox->getScaledHeight() / 2);
                pCheckbox->setChecked(selected);
                connect(pCheckbox.get(), &Checkbox::checkChanged, this, [this, id](bool checked)
                {
                    emit sigOverlayChanged(id, checked);
                }, Qt::QueuedConnection);
                m_pPanel->addItem(pCheckbox);

                x += GameMap::getImageSize() * 3 + pCheckbox->getScaledWidth();
                if (x > m_pPanel->getContentWidth() - 100 - pSprite->getScaledWidth() && i < overlayTerrainStyles.size() - 1)
                {
                    x = 20;
                    y += GameMap::getImageSize() * 3;
                }
            }
        }
        y += GameMap::getImageSize() * 3;
    }
}

void DialogModifyTerrain::remove()
{
    detach();
}

void DialogModifyTerrain::terrainClicked(QString id)
{    
    m_pTerrain->setFixedSprite(!id.isEmpty());
    m_pTerrain->setTerrainSpriteName(id);
    m_pTerrain->loadSprites();
    m_pTextbox->setCurrentText(id);
}

void DialogModifyTerrain::showLoadDialog()
{    
    QStringList wildcards;
    wildcards.append("*.png");
    QString path = Settings::getUserPath() + "customTerrainImages";
    spFileDialog fileDialog = spFileDialog::create(path, wildcards, m_pMap->getMapName(), true, tr("Load"));
    connect(fileDialog.get(),  &FileDialog::sigFileSelected, this, &DialogModifyTerrain::loadCustomSprite, Qt::QueuedConnection);
    addChild(fileDialog);    
}

void DialogModifyTerrain::loadCustomSprite(QString id)
{
    QString imageId = GlobalUtils::makePathRelative(id);
    terrainClicked(imageId);
}

void DialogModifyTerrain::overlayChanged(QString id, bool selected)
{
    if (selected)
    {
        m_pTerrain->addCustomOverlay(id);
    }
    else
    {
        m_pTerrain->removeCustomOverlay(id);
    }
    m_pTerrain->setFixedOverlaySprites(m_pTerrain->getCustomOverlays().size() > 0);
    m_pTerrain->loadSprites();
}
