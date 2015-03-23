// $Id: interp.cpp,v 1.4 2015-03-02 14:05:43-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg4

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"triangle" , &interpreter::make_polygon  },
   {"right_triangle", &interpreter::make_right_triangle},
   {"isosceles", &interpreter::make_isosceles},
   {"equilateral", &interpreter::make_equilateral},
   {"diamond" , &interpreter::make_diamond   },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

// Note: param is just a vector<string>::const_iterator
void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   object obj(itor->second, where, color);
   window::push_back(obj);
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::objects.at(window::selected_obj).border_color
          = 
   (rgbcolor)begin[0];
   window::objects.at(window::selected_obj).thickness
          =
   stof(begin[1]);
   window::objects.at(window::selected_obj).draw_border();
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::objects.at(window::selected_obj).moveby = stof(begin[0]);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto itor = fontcode.find(begin[0]);
   auto i = begin;
   string words = "";
   i++;
   while(i != end){
      words += (*i + " ");
      i++;
   }
   return make_shared<text> (itor->second, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   GLfloat height = stof(*itor);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat(stof(*begin)));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list v_list; // from shape.h
   for (auto i = begin; i != end; ++i){
      vertex vert;
      vert.xpos = stof(*i++);
      vert.ypos = stof(*i);
      v_list.push_back(vert);
   }
   return make_shared<polygon> (v_list);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   GLfloat height = stof(*itor);
   vertex_list v_list;
   vertex v1;
   v1.xpos = 0.0;        v1.ypos = 0.0;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = 0.0;        v2.ypos = height;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = width;      v3.ypos = height;
   v_list.push_back(v3);
   vertex v4;
   v4.xpos = width;      v4.ypos = 0.0;
   v_list.push_back(v4);
   return make_shared<rectangle> (width, height, v_list);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   vertex_list v_list;
   vertex v1;
   v1.xpos = 0.0;        v1.ypos = 0.0;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = 0.0;        v2.ypos = width;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = width;      v3.ypos = width;
   v_list.push_back(v3);
   vertex v4;
   v4.xpos = width;      v4.ypos = 0.0;
   v_list.push_back(v4);
   return make_shared<square> (width, v_list);
}

shape_ptr interpreter::make_right_triangle(param begin, param end){
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   GLfloat height = stof(*itor);
   vertex_list v_list;
   vertex v1;
   v1.xpos = width;        v1.ypos = width;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = height;       v2.ypos = height;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = height;       v3.ypos = width;
   v_list.push_back(v3);
   return make_shared<right_triangle> (width, height, v_list);
}

shape_ptr interpreter::make_isosceles(param begin, param end){
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   GLfloat height = stof(*itor);
   vertex_list v_list;
   vertex v1;
   v1.xpos = 0.0;        v1.ypos = 0.0;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = width;        v2.ypos = 0.0;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = width/2;      v3.ypos = height;
   v_list.push_back(v3);
   return make_shared<isosceles> (width, height, v_list); 
}

shape_ptr interpreter::make_equilateral(param begin, param end){
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   vertex_list v_list;
   vertex v1;
   v1.xpos = 0.0;        v1.ypos = 0.0;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = width;        v2.ypos = 0.0;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = width/2;      v3.ypos = width;
   v_list.push_back(v3);
   return make_shared<equilateral> (width, v_list); 
}

shape_ptr interpreter::make_diamond(param begin, param end){
   DEBUGF ('f', range (begin, end));
   auto itor = begin;
   GLfloat width = stof(*itor++);
   GLfloat height = stof(*itor);
   vertex_list v_list;
   vertex v1;
   v1.xpos = 0.0;              v1.ypos = height / 2;
   v_list.push_back(v1);
   vertex v2;
   v2.xpos = width / 2;        v2.ypos = height;
   v_list.push_back(v2);
   vertex v3;
   v3.xpos = width;            v3.ypos = height / 2;
   v_list.push_back(v3);
   vertex v4;
   v4.xpos = width / 2;        v4.ypos = 0.0;
   v_list.push_back(v4);
   return make_shared<diamond> (width, height, v_list);
}

