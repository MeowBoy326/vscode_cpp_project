#ifndef __CHANGEPAGEACTION_H__
#define __CHANGEPAGEACTION_H__

#include "cocos_fairyguiControllerAction.h"

NS_FGUI_BEGIN

class ChangePageAction : public ControllerAction
{
public:
    virtual void setup(ByteBuffer * buffer) override;

    std::string objectId;
    std::string controllerName;
    std::string targetPage;

protected:
    virtual void enter(GController* controller) override;
    virtual void leave(GController* controller) override;
};

NS_FGUI_END

#endif
