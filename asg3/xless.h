// $Id: xless.h,v 1.1 2015-02-10 17:44:06-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg3

#ifndef __XLESS_H__
#define __XLESS_H__

//
// We assume that the type type_t has an operator< function.
//

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

#endif

