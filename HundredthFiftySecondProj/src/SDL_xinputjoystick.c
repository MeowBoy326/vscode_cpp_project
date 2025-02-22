/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2015 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#include "SDL_sysjoystick.h"

#if SDL_JOYSTICK_XINPUT

#include "SDL_assert.h"
#include "SDL_hints.h"
#include "SDL_windowsjoystick_c.h"
#include "SDL_xinputjoystick_c.h"

/*
 * Internal stuff.
 */
static SDL_bool s_bXInputEnabled = SDL_TRUE;


static SDL_bool
SDL_XInputUseOldJoystickMapping()
{
    static int s_XInputUseOldJoystickMapping = -1;
    if (s_XInputUseOldJoystickMapping < 0) {
        const char *hint = SDL_GetHint(SDL_HINT_XINPUT_USE_OLD_JOYSTICK_MAPPING);
        s_XInputUseOldJoystickMapping = (hint && *hint == '1') ? 1 : 0;
    }
    return (s_XInputUseOldJoystickMapping > 0);
}

SDL_bool SDL_XINPUT_Enabled(void)
{
    return s_bXInputEnabled;
}

int
SDL_XINPUT_JoystickInit(void)
{
    const char *env = SDL_GetHint(SDL_HINT_XINPUT_ENABLED);
    if (env && !SDL_atoi(env)) {
        s_bXInputEnabled = SDL_FALSE;
    }

    if (s_bXInputEnabled && WIN_LoadXInputDLL() < 0) {
        s_bXInputEnabled = SDL_FALSE;  /* oh well. */
    }
    return 0;
}

static char *
GetXInputName(const Uint8 userid, BYTE SubType)
{
    char name[32];

    if (SDL_XInputUseOldJoystickMapping()) {
        SDL_snprintf(name, sizeof(name), "X360 Controller #%u", 1 + userid);
    } else {
        switch (SubType) {
        case XINPUT_DEVSUBTYPE_GAMEPAD:
            SDL_snprintf(name, sizeof(name), "XInput Controller #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_WHEEL:
            SDL_snprintf(name, sizeof(name), "XInput Wheel #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_ARCADE_STICK:
            SDL_snprintf(name, sizeof(name), "XInput ArcadeStick #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
            SDL_snprintf(name, sizeof(name), "XInput FlightStick #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_DANCE_PAD:
            SDL_snprintf(name, sizeof(name), "XInput DancePad #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_GUITAR:
        case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:
        case XINPUT_DEVSUBTYPE_GUITAR_BASS:
            SDL_snprintf(name, sizeof(name), "XInput Guitar #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_DRUM_KIT:
            SDL_snprintf(name, sizeof(name), "XInput DrumKit #%u", 1 + userid);
            break;
        case XINPUT_DEVSUBTYPE_ARCADE_PAD:
            SDL_snprintf(name, sizeof(name), "XInput ArcadePad #%u", 1 + userid);
            break;
        default:
            SDL_snprintf(name, sizeof(name), "XInput Device #%u", 1 + userid);
            break;
        }
    }
    return SDL_strdup(name);
}

static void
AddXInputDevice(const Uint8 userid, BYTE SubType, JoyStick_DeviceData **pContext)
{
    JoyStick_DeviceData *pPrevJoystick = NULL;
    JoyStick_DeviceData *pNewJoystick = *pContext;

    if (SDL_XInputUseOldJoystickMapping() && SubType != XINPUT_DEVSUBTYPE_GAMEPAD)
        return;

    if (SubType == XINPUT_DEVSUBTYPE_UNKNOWN)
        return;

    while (pNewJoystick) {
        if (pNewJoystick->bXInputDevice && (pNewJoystick->XInputUserId == userid) && (pNewJoystick->SubType == SubType)) {
            /* if we are replacing the front of the list then update it */
            if (pNewJoystick == *pContext) {
                *pContext = pNewJoystick->pNext;
            } else if (pPrevJoystick) {
                pPrevJoystick->pNext = pNewJoystick->pNext;
            }

            pNewJoystick->pNext = SYS_Joystick;
            SYS_Joystick = pNewJoystick;
            return;   /* already in the list. */
        }

        pPrevJoystick = pNewJoystick;
        pNewJoystick = pNewJoystick->pNext;
    }

    pNewJoystick = (JoyStick_DeviceData *)SDL_malloc(sizeof(JoyStick_DeviceData));
    if (!pNewJoystick) {
        return; /* better luck next time? */
    }
    SDL_zerop(pNewJoystick);

    pNewJoystick->joystickname = GetXInputName(userid, SubType);
    if (!pNewJoystick->joystickname) {
        SDL_free(pNewJoystick);
        return; /* better luck next time? */
    }

    pNewJoystick->bXInputDevice = SDL_TRUE;
    if (SDL_XInputUseOldJoystickMapping()) {
        SDL_zero(pNewJoystick->guid);
    } else {
        pNewJoystick->guid.data[0] = 'x';
        pNewJoystick->guid.data[1] = 'i';
        pNewJoystick->guid.data[2] = 'n';
        pNewJoystick->guid.data[3] = 'p';
        pNewJoystick->guid.data[4] = 'u';
        pNewJoystick->guid.data[5] = 't';
        pNewJoystick->guid.data[6] = SubType;
    }
    pNewJoystick->SubType = SubType;
    pNewJoystick->XInputUserId = userid;
    SDL_SYS_AddJoystickDevice(pNewJoystick);
}

void
SDL_XINPUT_JoystickDetect(JoyStick_DeviceData **pContext)
{
    int iuserid;

    if (!s_bXInputEnabled) {
        return;
    }

    /* iterate in reverse, so these are in the final list in ascending numeric order. */
    for (iuserid = XUSER_MAX_COUNT - 1; iuserid >= 0; iuserid--) {
        const Uint8 userid = (Uint8)iuserid;
        XINPUT_CAPABILITIES capabilities;
        if (XINPUTGETCAPABILITIES(userid, XINPUT_FLAG_GAMEPAD, &capabilities) == ERROR_SUCCESS) {
            AddXInputDevice(userid, capabilities.SubType, pContext);
        }
    }
}

int
SDL_XINPUT_JoystickOpen(SDL_Joystick * joystick, JoyStick_DeviceData *joystickdevice)
{
    const Uint8 userId = joystickdevice->XInputUserId;
    XINPUT_CAPABILITIES capabilities;
    XINPUT_VIBRATION state;

    SDL_assert(s_bXInputEnabled);
    SDL_assert(XINPUTGETCAPABILITIES);
    SDL_assert(XINPUTSETSTATE);
    SDL_assert(userId < XUSER_MAX_COUNT);

    joystick->hwdata->bXInputDevice = SDL_TRUE;

    if (XINPUTGETCAPABILITIES(userId, XINPUT_FLAG_GAMEPAD, &capabilities) != ERROR_SUCCESS) {
        SDL_free(joystick->hwdata);
        joystick->hwdata = NULL;
        return SDL_SetError("Failed to obtain XInput device capabilities. Device disconnected?");
    }
    SDL_zero(state);
    joystick->hwdata->bXInputHaptic = (XINPUTSETSTATE(userId, &state) == ERROR_SUCCESS);
    joystick->hwdata->userid = userId;

    /* The XInput API has a hard coded button/axis mapping, so we just match it */
    if (SDL_XInputUseOldJoystickMapping()) {
        joystick->naxes = 6;
        joystick->nbuttons = 15;
    } else {
        joystick->naxes = 6;
        joystick->nbuttons = 11;
        joystick->nhats = 1;
    }
    return 0;
}

static void
UpdateXInputJoystickState_OLD(SDL_Joystick * joystick, XINPUT_STATE_EX *pXInputState)
{
    static WORD s_XInputButtons[] = {
        XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_GUIDE
    };
    WORD wButtons = pXInputState->Gamepad.wButtons;
    Uint8 button;

    SDL_PrivateJoystickAxis(joystick, 0, (Sint16)pXInputState->Gamepad.sThumbLX);
    SDL_PrivateJoystickAxis(joystick, 1, (Sint16)(-SDL_max(-32767, pXInputState->Gamepad.sThumbLY)));
    SDL_PrivateJoystickAxis(joystick, 2, (Sint16)pXInputState->Gamepad.sThumbRX);
    SDL_PrivateJoystickAxis(joystick, 3, (Sint16)(-SDL_max(-32767, pXInputState->Gamepad.sThumbRY)));
    SDL_PrivateJoystickAxis(joystick, 4, (Sint16)(((int)pXInputState->Gamepad.bLeftTrigger * 65535 / 255) - 32768));
    SDL_PrivateJoystickAxis(joystick, 5, (Sint16)(((int)pXInputState->Gamepad.bRightTrigger * 65535 / 255) - 32768));

    for (button = 0; button < SDL_arraysize(s_XInputButtons); ++button) {
        SDL_PrivateJoystickButton(joystick, button, (wButtons & s_XInputButtons[button]) ? SDL_PRESSED : SDL_RELEASED);
    }
}

static void
UpdateXInputJoystickState(SDL_Joystick * joystick, XINPUT_STATE_EX *pXInputState)
{
    static WORD s_XInputButtons[] = {
        XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_GUIDE
    };
    WORD wButtons = pXInputState->Gamepad.wButtons;
    Uint8 button;
    Uint8 hat = SDL_HAT_CENTERED;

    SDL_PrivateJoystickAxis(joystick, 0, (Sint16)pXInputState->Gamepad.sThumbLX);
    SDL_PrivateJoystickAxis(joystick, 1, (Sint16)(-SDL_max(-32767, pXInputState->Gamepad.sThumbLY)));
    SDL_PrivateJoystickAxis(joystick, 2, (Sint16)(((int)pXInputState->Gamepad.bLeftTrigger * 65535 / 255) - 32768));
    SDL_PrivateJoystickAxis(joystick, 3, (Sint16)pXInputState->Gamepad.sThumbRX);
    SDL_PrivateJoystickAxis(joystick, 4, (Sint16)(-SDL_max(-32767, pXInputState->Gamepad.sThumbRY)));
    SDL_PrivateJoystickAxis(joystick, 5, (Sint16)(((int)pXInputState->Gamepad.bRightTrigger * 65535 / 255) - 32768));

    for (button = 0; button < SDL_arraysize(s_XInputButtons); ++button) {
        SDL_PrivateJoystickButton(joystick, button, (wButtons & s_XInputButtons[button]) ? SDL_PRESSED : SDL_RELEASED);
    }

    if (wButtons & XINPUT_GAMEPAD_DPAD_UP) {
        hat |= SDL_HAT_UP;
    }
    if (wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
        hat |= SDL_HAT_DOWN;
    }
    if (wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
        hat |= SDL_HAT_LEFT;
    }
    if (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
        hat |= SDL_HAT_RIGHT;
    }
    SDL_PrivateJoystickHat(joystick, 0, hat);
}

void
SDL_XINPUT_JoystickUpdate(SDL_Joystick * joystick)
{
    HRESULT result;
    XINPUT_STATE_EX XInputState;

    if (!XINPUTGETSTATE)
        return;

    result = XINPUTGETSTATE(joystick->hwdata->userid, &XInputState);
    if (result == ERROR_DEVICE_NOT_CONNECTED) {
        joystick->hwdata->send_remove_event = SDL_TRUE;
        joystick->hwdata->removed = SDL_TRUE;
        return;
    }

    /* only fire events if the data changed from last time */
    if (XInputState.dwPacketNumber && XInputState.dwPacketNumber != joystick->hwdata->dwPacketNumber) {
        if (SDL_XInputUseOldJoystickMapping()) {
            UpdateXInputJoystickState_OLD(joystick, &XInputState);
        } else {
            UpdateXInputJoystickState(joystick, &XInputState);
        }
        joystick->hwdata->dwPacketNumber = XInputState.dwPacketNumber;
    }
}

void
SDL_XINPUT_JoystickClose(SDL_Joystick * joystick)
{
}

void
SDL_XINPUT_JoystickQuit(void)
{
    if (s_bXInputEnabled) {
        WIN_UnloadXInputDLL();
    }
}

SDL_bool
SDL_SYS_IsXInputGamepad_DeviceIndex(int device_index)
{
    JoyStick_DeviceData *device = SYS_Joystick;
    int index;

    for (index = device_index; index > 0; index--)
        device = device->pNext;

    return (device->SubType == XINPUT_DEVSUBTYPE_GAMEPAD);
}

#else /* !SDL_JOYSTICK_XINPUT */

typedef struct JoyStick_DeviceData JoyStick_DeviceData;

SDL_bool SDL_XINPUT_Enabled(void)
{
    return SDL_FALSE;
}

int
SDL_XINPUT_JoystickInit(void)
{
    return 0;
}

void
SDL_XINPUT_JoystickDetect(JoyStick_DeviceData **pContext)
{
}

int
SDL_XINPUT_JoystickOpen(SDL_Joystick * joystick, JoyStick_DeviceData *joystickdevice)
{
    return SDL_Unsupported();
}

void
SDL_XINPUT_JoystickUpdate(SDL_Joystick * joystick)
{
}

void
SDL_XINPUT_JoystickClose(SDL_Joystick * joystick)
{
}

void
SDL_XINPUT_JoystickQuit(void)
{
}

#endif /* SDL_JOYSTICK_XINPUT */

/* vi: set ts=4 sw=4 expandtab: */
