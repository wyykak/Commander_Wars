#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/actor/Actor.h"

namespace oxygine
{
    class Draggable : public IClosureOwner
    {
    public:
        enum
        {
            DragMoveEvent = sysEventID('D', 'M', 'E')
        };

        explicit Draggable() = default;
        virtual ~Draggable();

        void init(Actor* actor);

        void destroy();

        Actor* getDragClient() const
        {
            return m_dragClient;
        }
        const RectF& getDragBounds() const
        {
            return m_bounds;
        }
        const Vector2& getClientPos() const
        {
            return m_clientPos;
        }
        bool getDragEnabled() const
        {
            return m_dragEnabled;
        }
        bool isDragging() const
        {
            return m_pressed;
        }
        void setDragEnabled(bool en)
        {
            m_dragEnabled = en;
        }
        /**sets bounds position for dragged actor*/
        void setDragBounds(const RectF& bounds);
        void setIgnoreTouchUp(bool ignore)
        {
            m_ignoreTouchUp = ignore;
        }
        void snapClient2Bounds();
        bool getNoLockForMiddleButton() const;
        void setNoLockForMiddleButton(bool newNoLockForMiddleButton);

    protected:
        Actor* getClient();
        void onEvent(Event* event);
        void startDrag(const Vector2& localPos);

        void onDrag(TouchEvent* es);
        void onMove(const Vector2& position);

        Vector2 convertPosUp(Actor* src, Actor* dest, const Vector2& pos, bool direction);
        Vector2 convertPosDown(Actor* src, Actor* dest, const Vector2& pos, bool direction);
    protected:
        RectF m_bounds{0, 0, -1, -1};
        Vector2 m_dragPos;
        Vector2 m_clientPos;
        Actor* m_dragClient{nullptr};
        timeMS m_startTm{0};
        bool m_dragEnabled{true};
        bool m_noLockForMiddleButton{false};
        bool m_middleButton{false};
        bool m_pressed{false};
        bool m_ignoreTouchUp{false};
    };
}
