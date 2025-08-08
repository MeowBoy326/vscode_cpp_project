#ifndef __GIMAGE_H__
#define __GIMAGE_H__

#include "cocos_FairyGUIMacros.h"
#include "cocos_fairyguiGObject.h"
#include "cocos_cocos2d.h"
#include "cocos_UIScale9Sprite.h"

NS_FGUI_BEGIN

class FUISprite;

class GImage : public GObject
{
public:
    GImage();
    virtual ~GImage();

    CREATE_FUNC(GImage);

    FlipType getFlip() const;
    void setFlip(FlipType value);

    cocos2d::Color3B getColor() const;
    void setColor(const cocos2d::Color3B& value);

    FillMethod getFillMethod() const;
    void setFillMethod(FillMethod value);

    FillOrigin getFillOrigin() const;
    void setFillOrigin(FillOrigin value);

    bool isFillClockwise() const;
    void setFillClockwise(bool value);

    float getFillAmount() const;
    void setFillAmount(float value);

    virtual void constructFromResource() override;

    virtual cocos2d::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const cocos2d::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;

private:
    FUISprite* _content;
};

NS_FGUI_END

#endif
