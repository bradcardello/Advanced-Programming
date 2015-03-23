// $Id: bigint.h,v 1.16 2015-02-02 21:16:30-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg1

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <memory>
#include <utility>
using namespace std;

#include "debug.h"

class bigint;
using bigint_ptr = shared_ptr<bigint>;
using digit_t = unsigned char;
using bigvalue_t = vector<digit_t>;
using itor = bigvalue_t::iterator;

//
// Define class bigint
//
class bigint {
      friend ostream& operator<< (ostream&, const bigint&);
   private:
      bool negative = false;
      bigvalue_t big_value; // bigvalue_t is a vector of unsigned chars
      // quotient and remainder
      digit_t bigval_size;
      using quot_rem = pair<bigint,bigint>;
      // why private for mult_by_2 and div_by_2 but not others?
      friend quot_rem divide (const bigint&, const bigint&);
      static void multiply_by_2 (bigvalue_t&);
      static void divide_by_2 (bigvalue_t&);
      bigvalue_t do_bigadd (const bigvalue_t&, const bigvalue_t&);
      bigvalue_t do_bigsub (const bigvalue_t&, const bigvalue_t&);
      bigvalue_t do_bigmul (const bigvalue_t&, const bigvalue_t&);
      void trim_zeros(bigvalue_t&);
   public:

      //
      // Ensure synthesized members are genrated.
      //
      bigint() = default;
      bigint (const bigint&) = default;
      bigint (bigint&&) = default;
      bigint& operator= (const bigint&) = default;
      bigint& operator= (bigint&&) = default;
      ~bigint() = default;

      //
      // Extra ctors to make bigints.
      //
      bigint (const long);
      bigint (const string&);

      //
      // Basic add/sub operators.
      //
      friend bool is_zero(const bigint&);
      friend bigint operator+ (const bigint&, const bigint&);
      friend bigint operator- (const bigint&, const bigint&);
      friend bigint operator+ (const bigint&);
      friend bigint operator- (const bigint&);
      long to_long() const;

      //
      // Extended operators implemented with add/sub.
      //
      friend bigint operator* (const bigint&, const bigint&);
      friend bigint operator/ (const bigint&, const bigint&);
      friend bigint operator% (const bigint&, const bigint&);

      //
      // Comparison operators.
      //
      friend bool operator== (const bigint&, const bigint&);
      friend bool operator<  (const bigint&, const bigint&);
};

//
// The rest of the operators do not need to be friends.
// Make the comparisons inline for efficiency.
//

//  what does it mean for comparisons to be inline?

bool do_bigless (const bigvalue_t&, const bigvalue_t&);

bigint pow (const bigint& base, const bigint& exponent);

inline bool operator!= (const bigint &left, const bigint &right) {
   return not (left == right);
}
inline bool operator>  (const bigint &left, const bigint &right) {
   return right < left;
}
inline bool operator<= (const bigint &left, const bigint &right) {
   return not (right < left);
}
inline bool operator>= (const bigint &left, const bigint &right) {
   return not (left < right);
}

#endif

