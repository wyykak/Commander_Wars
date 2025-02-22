#include "3rd_party/oxygine-framework/oxygine/RenderDelegate.h"
#include "3rd_party/oxygine-framework/oxygine/RenderState.h"
#include "3rd_party/oxygine-framework/oxygine/STDRenderer.h"

#include "3rd_party/oxygine-framework/oxygine/core/gamewindow.h"
#include "3rd_party/oxygine-framework/oxygine/core/UberShaderProgram.h"

#include "3rd_party/oxygine-framework/oxygine/actor/Actor.h"
#include "3rd_party/oxygine-framework/oxygine/actor/ClipRectActor.h"
#include "3rd_party/oxygine-framework/oxygine/actor/TextField.h"
#include "3rd_party/oxygine-framework/oxygine/actor/ColorRectSprite.h"

#include "3rd_party/oxygine-framework/oxygine/text_utils/Node.h"

#include <QPainter>

namespace oxygine
{
    spRenderDelegate RenderDelegate::instance;

    void RenderDelegate::render(Actor* parent, const RenderState& parentRS)
    {
#ifdef GRAPHICSUPPORT
        RenderState rs;
        if (!parent->internalRender(rs, parentRS))
        {
            return;
        }
        auto & children = parent->getChildren();
        for (auto & child : children)
        {
            child->render(rs);
        }
#endif
    }

    void RenderDelegate::render(ClipRectActor* actor, const RenderState& parentRS)
    {
#ifdef GRAPHICSUPPORT
        STDRenderer* renderer = STDRenderer::getCurrent();
        VideoDriver* driver = renderer->getDriver();

        RenderState rs = parentRS;

        const RectF* parentClippedRect = parentRS.clip;
        RectF clippedRect = *parentClippedRect;
        rs.clip = &clippedRect;

        Rect scissorRect(0, 0, 0, 0);
        bool scissorEnabled = driver->getScissorRect(scissorRect);
        bool vis = true;
        if (actor->getClipping())
        {
            renderer->flush();
            RectF ss_rect = Actor::getActorTransformedDestRect(actor, actor->getTransform() * parentRS.transform);

            clippedRect.clip(ss_rect);
            if (!clippedRect.isEmpty())
            {
                Rect gl_rect = Rect(static_cast<qint32>(clippedRect.pos.x + 0.01f),
                                    static_cast<qint32>(clippedRect.pos.y + 0.01f),
                                    static_cast<qint32>(clippedRect.size.x + 0.01f),
                                    static_cast<qint32>(clippedRect.size.y + 0.01f));

                if (!driver->getRenderTarget()->getHandle())
                {
                    GameWindow* window = oxygine::GameWindow::getWindow();
                    QSize size = window->size();
                    gl_rect.pos.y = size.height() - gl_rect.getBottom();
                }

                driver->setScissorRect(&gl_rect);
            }
            else
            {
                vis = false;
            }
        }

        if (vis)
        {
            actor->Actor::render(rs);
        }

        if (actor->getClipping())
        {
            renderer->flush();
            driver->setScissorRect(scissorEnabled ? &scissorRect : nullptr);
        }
#endif
    }

    void RenderDelegate::doRender(Sprite* sprite, const RenderState& rs)
    {
#ifdef GRAPHICSUPPORT
        if (!sprite->getAnimFrame().getTexture())
        {
            return;
        }
        QColor color = rs.getFinalColor(sprite->getColor());
        sprite->getMaterial()->apply();
        sprite->getMaterial()->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
#endif
    }

    void RenderDelegate::doRender(TextField* tf, const RenderState& rs)
    {
#ifdef GRAPHICSUPPORT
        text::Node* root = tf->getRootNode();
        if (!root)
        {
            return;
        }

        STDRenderer* renderer = STDRenderer::getCurrent();
        VideoDriver* driver = renderer->getDriver();
        Rect scissorRect(0, 0, 0, 0);
        bool scissorEnabled = driver->getScissorRect(scissorRect);

        spMaterial cur = Material::current;
        Material::null->apply();

        //---------------------------------------------------------
        // qt painter usage
        GameWindow* window = oxygine::GameWindow::getWindow();
        QPainter painter(window);
        if (scissorEnabled)
        {
            QSize size = window->size();
            QRect clipRect(scissorRect.getX(), size.height() - scissorRect.getY() - scissorRect.getHeight(), scissorRect.getWidth(), scissorRect.getHeight());
            painter.setClipRect(clipRect);
        }
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, tf->getFont().antialiasing);
        root->draw(rs, tf->getStyle(), tf->getStyle().color, painter);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);
        painter.end();
        //---------------------------------------------------------

        rsCache().restoreAfterPainterUse();
        cur->apply();
        driver->setScissorRect(scissorEnabled ? &scissorRect : nullptr);
#endif
    }

    void RenderDelegate::doRender(ColorRectSprite* sprite, const RenderState& rs)
    {
#ifdef GRAPHICSUPPORT
        QColor color = rs.getFinalColor(sprite->getColor());
        sprite->getMaterial()->apply();
        sprite->getMaterial()->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
#endif
    }
}
