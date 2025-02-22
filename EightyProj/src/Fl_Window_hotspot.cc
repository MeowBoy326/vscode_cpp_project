//
// "$Id$"
//
// Common hotspot routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#include <Fl.H>
#include <Fl_Window.H>
#include <fl_x.H>
#include <stdio.h>

void Fl_Window::hotspot(int X, int Y, int offscreen) {
  int mx,my;

  // Update the screen position based on the mouse position.
  Fl::get_mouse(mx,my);
  X = mx-X; Y = my-Y;

  // If offscreen is 0 (the default), make sure that the window
  // stays on the screen, if possible.
  if (!offscreen) {
    int scr_x, scr_y, scr_w, scr_h;
    Fl::screen_work_area(scr_x, scr_y, scr_w, scr_h);

    int top = 0;
    int left = 0;
    int right = 0;
    int bottom = 0;

    if (border()) {
#ifdef WIN32
      if (size_range_set && (maxw != minw || maxh != minh)) {
        left = right = GetSystemMetrics(SM_CXSIZEFRAME);
        top = bottom = GetSystemMetrics(SM_CYSIZEFRAME);
      } else {
        left = right = GetSystemMetrics(SM_CXFIXEDFRAME); 
        top = bottom = GetSystemMetrics(SM_CYFIXEDFRAME);
      }
      top += GetSystemMetrics(SM_CYCAPTION);
#elif defined(__APPLE__)
      top = 24;
      left = 2;
      right = 2;
      bottom = 2;
#else
      // Ensure border is on screen; these values are generic enough
      // to work with many window managers, and are based on KDE defaults.
      top = 20;
      left = 4;
      right = 4;
      bottom = 8;
#endif
    }
    // now insure contents are on-screen (more important than border):
    if (X+w()+right > scr_w+scr_x) X = scr_w+scr_x-right-w();
    if (X-left < scr_x) X = left + scr_x;
    if (Y+h()+bottom > scr_h+scr_y) Y = scr_h+scr_y-bottom-h();
    if (Y-top < scr_y) Y = top + scr_y;
    // make sure that we will force this position
    if (X==x()) x(X-1);
  }

  position(X,Y);
}

void Fl_Window::hotspot(const Fl_Widget *o, int offscreen) {
  int X = o->w()/2;
  int Y = o->h()/2;
  while (o != this && o) {
    X += o->x(); Y += o->y();
    o = o->window();
  }
  hotspot(X,Y,offscreen);
}


//
// End of "$Id$".
//
