#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/actor/Sprite.h"

namespace oxygine
{
    class ColorRectSprite;
    using spColorRectSprite = intrusive_ptr<ColorRectSprite>;
    class ColorRectSprite final : public Sprite
    {
    public:
        explicit ColorRectSprite();
        ~ColorRectSprite() = default;
        virtual void doRender(const RenderState&) override;
        virtual bool isOn(const Vector2& localPosition, float localScale) override;
    protected:
        virtual void sizeChanged(const Vector2& size) override;
    };
}
