// $Id: scanner.h,v 1.1 2015-01-24 22:21:22-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg1

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <iostream>
#include <utility>
using namespace std;

#include "debug.h"

enum terminal_symbol {NUMBER, OPERATOR, SCANEOF};
struct token_t {
   terminal_symbol symbol;
   string lexinfo;
};

class scanner {
   private:
      bool seen_eof;
      char lookahead;
      void advance();
   public:
      scanner();
      token_t scan();
};

ostream& operator<< (ostream&, const terminal_symbol&);
ostream& operator<< (ostream&, const token_t&);

#endif

