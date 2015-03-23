// $Id: graphics.h,v 1.4 2015-03-03 01:27:36-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg4

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   public:
      // Default copiers, movers, dtor all OK.
      shared_ptr<shape> pshape;  // how do we store info in this so that
                                 // it retains border info?
      vertex center;
      rgbcolor color;
      GLfloat thickness = 4;
      GLfloat moveby = 4.0;
      rgbcolor border_color;
      object(const shared_ptr<shape>& shape, vertex& where,
             rgbcolor color): pshape(shape), center(where),
             color(color) {}
      void draw() { pshape->draw (center, color); }
      void draw_border() { pshape->draw_border (center, border_color,
                                                thickness); }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static mouse mus;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static vector<object> objects;
      static size_t selected_obj;
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void main();
};

#endif

