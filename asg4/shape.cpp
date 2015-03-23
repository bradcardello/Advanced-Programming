// $Id: shape.cpp,v 1.6 2015-03-03 01:27:36-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg4

#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (const GLfloat width, const GLfloat height,
                      const vertex_list& vertices):
         polygon(vertices)
{
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width, const vertex_list &vertices):
         rectangle (width, width, vertices)
{
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& vertices): polygon(vertices){
   DEBUGF ('c', this);
}

right_triangle::right_triangle (const GLfloat width, 
                                const GLfloat height,
                                const vertex_list& vertices):
         triangle(vertices){
      DEBUGF ('c', this << "(" << width << "," << height << ")");
}

isosceles::isosceles(const GLfloat width, GLfloat height,
                     const vertex_list& vertices):
         triangle(vertices){
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

equilateral::equilateral(const GLfloat width,
                         const vertex_list& vertices):
         triangle(vertices){
   DEBUGF ('c', this << "(" << width << ")"); 
}

diamond::diamond (const GLfloat width, const GLfloat height,
                  const vertex_list& vertices):
         polygon(vertices){
      DEBUGF ('c', this << "(" << width << "," << height << ")");
}

void shape::draw_border (const vertex& center, const rgbcolor& color,
                         const GLfloat thickness) const {
   vertex disregard_errors = center;
   rgbcolor acquire_money = color;
   GLfloat i_hate_borders = thickness;
   
   disregard_errors.xpos++;
   acquire_money = (rgbcolor)"black";
   i_hate_borders++;
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glEnable(GL_LINE_SMOOTH); 
   glColor3ubv(color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   for (auto ch: textdata) glutBitmapCharacter (glut_bitmap_font, ch);
   glEnd();
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3d (color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float x = dimension.xpos * cos (theta) + center.xpos / 2;
      float y = dimension.ypos * sin (theta) + center.ypos / 2;
      glVertex2f (x + center.xpos / 2, y + center.ypos / 2);
   }
   glEnd();
}

void ellipse::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat thickness) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glPointSize(thickness);
   glBegin (GL_LINE_LOOP);
   glColor3d (color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float x = dimension.xpos * cos (theta) + center.xpos / 2;
      float y = dimension.ypos * sin (theta) + center.ypos / 2;
      glVertex2f (x + center.xpos / 2, y + center.ypos / 2);
   }
   glEnd();
   glLineWidth(1.f); // reset
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   float x_sum = 0.0;
   float y_sum = 0.0;
   size_t total_count = 0;
   
   for (auto i = vertices.begin(); i != vertices.end(); ++i){
      x_sum += i->xpos;
      y_sum += i->ypos;
      total_count++;
   }
   float x_center = x_sum / total_count;
   float y_center = y_sum / total_count;
   
   vertex_list new_list;
   for (auto itor = vertices.begin(); itor != vertices.end(); ++itor){
      vertex v_adjusted;
      v_adjusted.xpos = itor->xpos - x_center;
      v_adjusted.ypos = itor->ypos - y_center;
      new_list.push_back(v_adjusted);
   }
   
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3d (color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   
   for (auto i = new_list.begin(); i != new_list.end(); ++i) {
      float x = i->xpos + center.xpos;
      float y = i->ypos + center.ypos;
      glVertex2f (x, y);
   }
   glEnd();
}

void polygon::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat thickness) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   float x_sum = 0.0;
   float y_sum = 0.0;
   size_t total_count = 0;
   
   for (auto i = vertices.begin(); i != vertices.end(); ++i){
      x_sum += i->xpos;
      y_sum += i->ypos;
      total_count++;
   }
   float x_center = x_sum / total_count;
   float y_center = y_sum / total_count;
   
   vertex_list new_list;
   for (auto itor = vertices.begin(); itor != vertices.end(); ++itor){
      vertex v_adjusted;
      v_adjusted.xpos = itor->xpos - x_center;
      v_adjusted.ypos = itor->ypos - y_center;
      new_list.push_back(v_adjusted);
   }
   
   glBegin (GL_LINE_LOOP);
   glLineWidth(thickness);
   glEnable (GL_LINE_SMOOTH);
   glColor3d (color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   
   for (auto i = new_list.begin(); i != new_list.end(); ++i) {
      float x = i->xpos + center.xpos;
      float y = i->ypos + center.ypos;
      glVertex2f (x, y);
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}
