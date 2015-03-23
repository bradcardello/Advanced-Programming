// $Id: main.cpp,v 1.18 2015-02-18 02:12:30-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg3

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string,string>;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

string trim (const string &str) {
   size_t first = str.find_first_not_of (" \t");
   if (first == string::npos){
      return "";
   }
   size_t last = str.find_last_not_of (" \t");
   string s = str.substr (first, last - first + 1);
   return str.substr (first, last - first + 1);
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   size_t EXIT_STATUS = 0;
   if(argc > 1){
      for (int j = 1; j < argc; j++){
         string name = argv[j];
         string line;
         ifstream filename;
         filename.open(name);
         if (!filename.good()){
            cout << "keyvalue: " << name 
                 << ": No such file or directory" << endl;
            EXIT_STATUS = 1;
            continue;
         }
         str_str_map *m = new str_str_map();
         while(!filename.eof()){
            for (size_t line_no = 1; ; line_no++) {
               getline (filename, line);
               if (filename.eof()) break;
               line = trim (line);
               
               // Comment or blank line
               if (line.size() == 0 or line[0] == '#'){
                  cout << name << ": " << line_no << ": " << line
                       << endl;
                  continue;
               }
               
               size_t pos = line.find_first_of ('=');
               
               // command: key =
               // finds and deletes key in map, if found
               if (pos == line.size() - 1 && pos != 0) {
                  string key = trim (line.substr (0, pos));
                  cout << name << ": " << line_no << ": " << line
                       << endl;
                  str_str_map::iterator i;
                  i = m->find(key);
                  if (i != m->end()) i = m->str_str_map::erase(i);
               }
               
               // command: =
               // AND
               // command: = value
               else if (pos == 0){
                  string value = trim (line.substr (pos + 1));
                  // command: =
                  // prints out all key(s) and their associated value(s)
                  if (value == ""){
                     cout << name << ": " << line_no << ": " << line
                          << endl;
                     str_str_map::iterator i;
                     for (i = m->begin(); i != m->end(); ++i){
                        cout << (*i).first << " = " << (*i).second
                             << endl;
                     }
                  }
                  // command: = value
                  // prints out key and value pair(s) with this value
                  else{
                     str_str_map::iterator i;
                     for (i = m->begin(); i != m->end(); ++i){
                        if ((*i).second == value)
                           cout << (*i).first << " = " << (*i).second
                                << endl;
                     }
                  }
               }
               
               // command: key
               // prints out key and value pair(s) with this key
               else if (pos == string::npos){
                  cout << name << ": " << line_no << ": " << line
                       << endl;
                  str_str_map::iterator i;
                  string key = trim (line.substr (0, pos));
                  i = m->find(key);
                  if (i != m->end())
                     cout << (*i).first << " = " << (*i).second
                          << endl;
                  else{
                     cerr << key << ": key not found" << endl;
                     EXIT_STATUS = 1;
                  }
               }
               
               // command: key = value
               // creates xpair with key_type = key, mapped_type = value
               else {
                  str_str_map::iterator i;
                  string key = trim (line.substr (0, pos));
                  string value = trim (line.substr (pos + 1));
                  cout << name << ": " << line_no << ": " << line
                       << endl;
                  str_str_pair p (key, value);
                  i = m->str_str_map::insert(p);
                  cout << key << " = " << value << endl;
               }
            }
         }
         //delete m;
         filename.close();
      }
   }
   else{
      str_str_map *m = new str_str_map();
      for (size_t line_no = 1; ; line_no++) {
         string line;
         getline (cin, line);
         if (cin.eof()) break;
         line = trim (line);
         
         // Comment or blank line
         if (line.size() == 0 or line[0] == '#'){
            cout << "-: " << line_no << ": " << line<< endl;
            continue;
         }
         size_t pos = line.find_first_of ('=');
         
         // command: key =
         // finds and deletes key in map, if found
         if (pos == line.size() - 1 && pos != 0) {
            cout << "-: " << line_no << ": " << line << endl;
            str_str_map::iterator i;
            string key = trim (line.substr (0, pos));
            i = m->find(key);
            if (i != m->end()) i = m->str_str_map::erase(i);
         }
         
         // command: =
         // prints out all key(s) and their associated value
         else if (pos == 0){
            cout << "-: " << line_no << ": " << line << endl;
            string value = trim (line.substr (pos + 1));
            if (value == ""){
               str_str_map::iterator i;
               for (i = m->begin(); i != m->end(); ++i){
                  cout << (*i).first << " = " << (*i).second << endl;
               }
            }
            // command: = value
            // prints out key and value pair(s) with this value
            else{
               str_str_map::iterator i;
               for (i = m->begin(); i != m->end(); ++i){
                  if ((*i).second == value)
                     cout << (*i).first << " = " << (*i).second << endl;
               }
            }
         }
         
         // command: key
         // prints out key and value pair with this key
         else if (pos == string::npos){
            cout << "-: " << line_no << ": " << line << endl;
            str_str_map::iterator i;
            string key = trim (line.substr (0, pos)); // uh?
            i = m->find(key);
            if (i != m->end())
               cout << (*i).first << " = " << (*i).second << endl;
            else{
               cerr << key << ": key not found" << endl;
               EXIT_STATUS = 1;
            }
         }
         
         // command: key = value
         // creates xpair with key_type = key, mapped_type = value
         else {
            str_str_map::iterator i;
            string key = trim (line.substr (0, pos));
            string value = trim (line.substr (pos + 1));
            cout << "-: " << line_no << ": " << line << endl;
            str_str_pair p (key, value);
            i = m->str_str_map::insert(p);
            cout << key << " = " << value << endl;
         }
      }
      //delete m;
   }
   return EXIT_STATUS;
}

