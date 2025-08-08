#include "my_sandbox/include/MySandBox.h"
#include OgreException.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

int main()
{
    MySandBox application;

    try
    {
        application.Run();
    }
    catch(Ogre::Exception& error)
    {
        MessageBox(
            NULL,
			(LPCWSTR)error.getFullDescription().c_str(),
            L"An exception has occurred!",
            MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    return 0;
}