#ifndef __GEARANIMATION_H__
#define __GEARANIMATION_H__

#include "cocos_FairyGUIMacros.h"
#include "cocos_cocos2d.h"
#include "cocos_fairyguiGearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearAnimation : public GearBase
{
public:
    GearAnimation(GObject* owner);
    virtual ~GearAnimation();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    class GearAnimationValue
    {
    public:
        bool playing;
        int frame;

        GearAnimationValue();
        GearAnimationValue(bool playing, int frame);
    };
    std::unordered_map<std::string, GearAnimationValue> _storage;
    GearAnimationValue _default;
};

NS_FGUI_END

#endif
