/**********************************************************************************************
*
*   rglfw - raylib GLFW single file compilation
*
*   This file includes latest GLFW sources (https://github.c.hom/glfw/glfw) to be compiled together
*   with raylib for all supported platforms, this way, no external dependencies are required.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2017-2023 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

//#define _GLFW_BUILD_DLL           // To build shared version
// Ref: http://www.glfw.org/docs/latest/compile.html#compile_manual

// Platform options:
// _GLFW_WIN32      to use the Win32 API
// _GLFW_X11        to use the X Window System
// _GLFW_WAYLAND    to use the Wayland API (experimental and incomplete)
// _GLFW_COCOA      to use the Cocoa frameworks
// _GLFW_OSMESA     to use the OSMesa API (headless and non-interactive)
// _GLFW_MIR        experimental, not supported at this moment

#if defined(_WIN32) || defined(__CYGWIN__)
    #define _GLFW_WIN32
#endif
#if defined(__linux__)
    #if !defined(_GLFW_WAYLAND)     // Required for Wayland windowing
        #define _GLFW_X11
    #endif
#endif
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    #define _GLFW_X11
#endif
#if defined(__APPLE__)
    #define _GLFW_COCOA
    #define _GLFW_USE_MENUBAR       // To create and populate the menu bar when the first window is created
    #define _GLFW_USE_RETINA        // To have windows use the full resolution of Retina displays
#endif
#if defined(__TINYC__)
    #define _WIN32_WINNT_WINXP      0x0501
#endif

// Common modules to all platforms
#include "init.c.h"
// #include "platform.c.h"
#include "context.c.h"
#include "monitor.c.h"
#include "window.c.h"
#include "input.c.h"
#include "vulkan.c.h"

#if defined(_WIN32) || defined(__CYGWIN__)
    #include "win32_init.c.h"
    // #include "win32_module.c.h"
    #include "win32_monitor.c.h"
    #include "win32_window.c.h"
    #include "win32_joystick.c.h"
    #include "win32_time.c.h"
    #include "win32_thread.c.h"
    #include "wgl_context.c.h"

    #include "egl_context.c.h"
    #include "osmesa_context.c.h"
#endif

#if defined(__linux__)
    // #include "posix_module.c.h"
    #include "posix_thread.c.h"
    #include "posix_time.c.h"
    #include "posix_poll.c.h"
    #include "linux_joystick.c.h"
    #include "xkb_unicode.c.h"

    #include "egl_context.c.h"
    #include "osmesa_context.c.h"

    #if defined(_GLFW_WAYLAND)
        #include "wl_init.c.h"
        #include "wl_monitor.c.h"
        #include "wl_window.c.h"
    #endif
    #if defined(_GLFW_X11)
        #include "x11_init.c.h"
        #include "x11_monitor.c.h"
        #include "x11_window.c.h"
        #include "glx_context.c.h"
    #endif
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined( __NetBSD__) || defined(__DragonFly__)
    // #include "posix_module.c.h"
    #include "posix_thread.c.h"
    #include "posix_time.c.h"
    #include "null_joystick.c.h"
    #include "xkb_unicode.c.h"

    #include "x11_init.c.h"
    #include "x11_monitor.c.h"
    #include "x11_window.c.h"
    #include "glx_context.c.h"

    #include "egl_context.c.h"
    #include "osmesa_context.c.h"
#endif

#if defined(__APPLE__)
    // #include "posix_module.c.h"
    #include "posix_thread.c.h"
    #include "cocoa_init.m"
    #include "cocoa_joystick.m"
    #include "cocoa_monitor.m"
    #include "cocoa_window.m"
    #include "cocoa_time.c.h"
    #include "nsgl_context.m"

    #include "egl_context.c.h"
    #include "osmesa_context.c.h"
#endif
