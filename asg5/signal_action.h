// $Id: signal_action.h,v 1.1 2015-03-12 23:42:21-07 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg5

#ifndef __SIGNAL_ACTION_H__
#define __SIGNAL_ACTION_H__

#include <stdexcept>
using namespace std;

#include <signal.h>

class cix_exit: public exception{};

class signal_action {
   private:
      struct sigaction action;
   public:
      signal_action (int signal, void (*handler) (int));
};

class signal_error: runtime_error {
   public:
      int signal;
      explicit signal_error (int signal);
};

#endif

