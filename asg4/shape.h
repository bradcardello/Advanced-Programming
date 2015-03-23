// $Id: shape.h,v 1.6 2015-03-03 01:27:36-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg4

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <cmath>
using namespace std;

#include "rgbcolor.h"

//
// Shapes constitute a single-inheritance hierarchy, summarized
// here, with the superclass listed first, and subclasses indented
// under their immediate superclass.
//
// shape
//    text
//    ellipse
//       circle
//    polygon
//       rectangle
//          square
//       diamond
//       triangle
//          right_triangle
//          isosceles
//          equilateral
//

class shape;
struct vertex {GLfloat xpos; GLfloat ypos; };
using vertex_list = vector<vertex>;
using shape_ptr = shared_ptr<shape>; 

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};


//
// Abstract base class for all shapes in this system.
//

class shape {
   friend ostream& operator<< (ostream& out, const shape&);
   private:
      shape (const shape&) = delete; // Prevent copying.
      shape& operator= (const shape&) = delete; // Prevent copying.
      shape (shape&&) = delete; // Prevent moving.
      shape& operator= (shape&&) = delete; // Prevent moving.
   protected:
      inline shape(); // Only subclass may instantiate.
   public:
      virtual ~shape() {}
      virtual void draw (const vertex&, const rgbcolor&) const = 0;
      virtual void draw_border (const vertex&, const rgbcolor&, 
                                const GLfloat) const;
      virtual void show (ostream&) const;
};


//
// Class for printing text.
//

class text: public shape {
   protected:
      void* glut_bitmap_font = nullptr;
      // GLUT_BITMAP_8_BY_13
      // GLUT_BITMAP_9_BY_15
      // GLUT_BITMAP_HELVETICA_10
      // GLUT_BITMAP_HELVETICA_12
      // GLUT_BITMAP_HELVETICA_18 
      // GLUT_BITMAP_TIMES_ROMAN_10
      // GLUT_BITMAP_TIMES_ROMAN_24
      string textdata;
   public:
      text (void* glut_bitmap_font, const string& textdata);
      virtual void draw (const vertex&, const rgbcolor&) const override;
      virtual void show (ostream&) const override;
};

//
// Classes for ellipse and circle.
//

class ellipse: public shape {
   protected:
      vertex dimension;
   public:
      ellipse (GLfloat width, GLfloat height);
      virtual void draw (const vertex&, const rgbcolor&) const override;
      virtual void draw_border (const vertex&, const rgbcolor&,
                                const GLfloat) const override;
      virtual void show (ostream&) const override;
};

class circle: public ellipse {
   public:
      circle (GLfloat diameter);
};

//
// Class polygon.
//

class polygon: public shape {
   protected:
      const vertex_list vertices;
   public:
      polygon (const vertex_list& vertices);
      virtual void draw (const vertex&, const rgbcolor&) const override;
      virtual void draw_border (const vertex&, const rgbcolor&,
                                const GLfloat) const override;
      virtual void show (ostream&) const override;
};


//
// Classes rectangle, square, etc.
//

class rectangle: public polygon {
   public:
      rectangle (const GLfloat, const GLfloat, const vertex_list&);
};

class square: public rectangle {
   public:
      square (GLfloat width, const vertex_list&);
};

class triangle: public polygon{
   public:
      triangle(const vertex_list& vertices);
};

class right_triangle: public triangle{
   public:
      right_triangle (const GLfloat, const GLfloat, const vertex_list&);
};

class isosceles:public triangle{
   public:
      isosceles(const GLfloat, const GLfloat, const vertex_list&);
};

class equilateral: public triangle{
   public:
      equilateral(const GLfloat, const vertex_list&);
};

class diamond: public polygon {
   public:
      diamond (const GLfloat, const GLfloat, const vertex_list&);
};

ostream& operator<< (ostream& out, const shape&);

#endif

