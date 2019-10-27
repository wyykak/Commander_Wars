#pragma once
#include "oxygine-include.h"
#include "Material.h"
#include "actor/Actor.h"

namespace oxygine
{
    class VisualStyle
    {
    public:

        VisualStyle();

        const Color&        getColor() const {return _color;}
        blend_mode          getBlendMode() const {return _blend;}

        void setColor(const Color& color);
        void setBlendMode(blend_mode mode);

    protected:
        Color _color;
        blend_mode _blend;
    };

    DECLARE_SMART(VStyleActor, spVStyleActor);
    class VStyleActor : public Actor
    {
    public:
        DECLARE_COPYCLONE_NEW(VStyleActor)
        VStyleActor();

        blend_mode              getBlendMode() const {return _vstyle.getBlendMode();}
        const Color&            getColor() const;
        const Color&            getAddColor() const;

        void                    setColor(const Color& color);
        void                    setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void                    setBlendMode(blend_mode mode);
        void                    setAddColor(const Color& color);
        void                    setAddColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0);

        virtual void setEnabled(bool value) override;

        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getColor, &VStyleActor::setColor> TweenColor;
        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getAddColor, &VStyleActor::setAddColor> TweenAddColor;

        void                    setMaterial(spSTDMaterial mat);
        void                    resetMaterial();
        spSTDMaterial _mat;

        Color getDisableColor() const;
        void setDisableColor(const Color &value);

    protected:
        virtual void matChanged() {}
        VisualStyle _vstyle;
        Color disableColor{75, 75, 75, 0};

    };

}
