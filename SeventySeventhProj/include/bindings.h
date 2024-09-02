// Copyright (c) 2015 Sergio Gonzalez. All rights reserved.
// License: https://github.com/serge-rgb/milton#license

#pragma once

#include "common.h"

enum BindableAction
{
    Action_NONE,

    Action_FIRST,

    Action_DECREASE_BRUSH_SIZE = Action_FIRST,
    Action_INCREASE_BRUSH_SIZE,
    Action_ZOOM_IN,
    Action_ZOOM_OUT,
    Action_REDO,
    Action_UNDO,
    Action_EXPORT,
    Action_QUIT,
    Action_NEW,
    Action_SAVE,
    Action_SAVE_AS,
    Action_OPEN,
    Action_TOGGLE_MENU,
    Action_TOGGLE_GUI,
    Action_MODE_ERASER,
    Action_MODE_PEN,
    Action_MODE_EYEDROPPER,
    Action_MODE_PRIMITIVE_LINE,
    Action_MODE_PRIMITIVE_RECTANGLE,
    Action_MODE_PRIMITIVE_GRID,
    Action_SET_BRUSH_ALPHA_10,
    Action_SET_BRUSH_ALPHA_20,
    Action_SET_BRUSH_ALPHA_30,
    Action_SET_BRUSH_ALPHA_40,
    Action_SET_BRUSH_ALPHA_50,
    Action_SET_BRUSH_ALPHA_60,
    Action_SET_BRUSH_ALPHA_70,
    Action_SET_BRUSH_ALPHA_80,
    Action_SET_BRUSH_ALPHA_90,
    Action_SET_BRUSH_ALPHA_100,
    Action_HELP,
    Action_PEEK_OUT,
    Action_DRAG_BRUSH_SIZE,
    Action_DRAG_ZOOM,
    Action_TRANSFORM,

    #if MILTON_ENABLE_PROFILING
        // Debug bindings
        Action_TOGGLE_DEBUG_WINDOW,
    #endif

    Action_COUNT,

    // Press-and-release actions.
    ActionRelease_PEEK_OUT,
    ActionRelease_DRAG_BRUSH_SIZE,
    ActionRelease_DRAG_ZOOM,
    ActionRelease_TRANSFORM,

    Action_COUNT_WITH_RELEASE,
};

enum ModifierFlags
{
    Modifier_NONE = 0,

    Modifier_CTRL = 1<<0,
    Modifier_WIN = 1<<1,
    Modifier_ALT = 1<<2,
    Modifier_SPACE = 1<<3, // Spaaaaaace
    Modifier_SHIFT = 1<<4,
};

struct Binding
{
    u8 accepts_repeats;
    u8 on_release;

    ModifierFlags modifiers;

    i8 bound_key;  // Positive values are ascii keys.
    // Zero/Negative values:
    enum Key
    {
        UNBOUND = 0,
        TAB = '\t',
        ESC = 27,

        F1 = -2,
        F2 = -3,
        F3 = -4,
        F4 = -5,
        F5 = -6,
        F6 = -7,
        F7 = -8,
        F8 = -9,
        F9 = -10,
        F10 = -11,
        F11 = -12,
        F12 = -13,

        KP_DIVIDE = '/',
        KP_MULTIPLY = '*',
        KP_MINUS = '-',
        KP_PLUS = '+',
        KP_PERIOD = '.',
        KP_0 = '0',
        KP_1 = '1',
        KP_2 = '2',
        KP_3 = '3',
        KP_4 = '4',
        KP_5 = '5',
        KP_6 = '6',
        KP_7 = '7',
        KP_8 = '8',
        KP_9 = '9',
    };

    BindableAction action;
};

struct MiltonBindings
{
    // NOTE: On key-down, we have to loop over this array to find a matching
    // binding. Action_COUNT is a small number so this is not really a problem.
    // It's unlikely to ever be a problem, but if we ever get to a point where
    // there are enough actions that looping over this array feels too wrong,
    // we can add a map here.
    Binding bindings[Action_COUNT_WITH_RELEASE];
};

