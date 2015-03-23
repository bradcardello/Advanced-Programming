// $Id: bigint.cpp,v 1.52 2015-02-03 01:17:16-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg2

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"

// constructor for long data-types
bigint::bigint (long that) {
   bool isnegative = that < 0 ? true : false;
   long copy = that;
   if (isnegative) copy = -copy;
   bigint big(to_string(copy));
   big_value = big.big_value;
   negative = isnegative;
}

// constructor for const string& data-types
bigint::bigint (const string& that) {
   auto itor = that.cbegin();
   if (that == "0"){
      negative = false;
   }
   else{
      bool isnegative = false;
      if (itor != that.cend() and *itor == '_') {
         isnegative = true;
         ++itor;
      }
      int newval = 0;
      auto i = that.rbegin();
      while (*i != '_' && i != that.rend()){
         newval = *i++ - '0';
         big_value.push_back(newval);
      }
      if (isnegative == true){
         negative = true;
      }
      else{
         negative = false;
      }
      bigval_size = big_value.size();
   }
}

// trims leading zeroes
void bigint::trim_zeros (bigvalue_t& big_val){
   while (big_val.size() > 1){
      digit_t digitpos = big_val.size() - 1;
      if (big_val.at(digitpos) != 0) break;
      big_val.pop_back();
   }
}

// checks if a bigint is 0
bool is_zero(const bigint& check){
   if (check.big_value.size() > 0) return false;
   if (check.big_value.size() == 0) return true;
   // else size == 1
   if (check.big_value.at(0) == 0){
      return true;
   }
   return false;
}

/* PSEUDO-CODE FOR DO_BIGADD*/
// carry = 0
// figure out the larger
// iterate over the larger
//    get the next digit of the larger
//    if smaller.hasNext 
//       get the digit of the smaller
//    else
//       set smaller digit = 0
//    new digit = larger digit + smaller digit + carry
//    push digit onto new bigvalue
//    carry = 0
//    calculate the carry
// if carry != 0, add the carry on to the end of the final number
bigvalue_t bigint::do_bigadd (const bigvalue_t& left, 
                              const bigvalue_t& right){
   bigvalue_t result;
   digit_t carry = 0;
   bool left_is_smaller = do_bigless(left, right);
   digit_t more_digits = left_is_smaller ? right.size() : left.size();
   bigvalue_t bigger = left_is_smaller ? right : left;
   bigvalue_t smaller = bigger == right ? left : right;
   
   for (digit_t i = 0; i < more_digits; i++){
      digit_t larger_val_digit = bigger.at(i);
      digit_t smaller_val_digit;
      if (i <= (smaller.size() - 1)){
         smaller_val_digit = smaller.at(i);
      }
      else{
         smaller_val_digit = 0;
      }
      digit_t new_val = larger_val_digit + smaller_val_digit + carry;
      result.push_back(new_val % 10);
      carry = 0;
      carry = new_val/10;
   }
   if (carry != 0){
      result.push_back(carry);
   }
   return result;
}

bigvalue_t bigint::do_bigsub (const bigvalue_t& left, 
                              const bigvalue_t& right){
   bigvalue_t result;
   digit_t borrow = 0;
   bool left_is_smaller = do_bigless(left, right);
   digit_t more_digits = left_is_smaller ? right.size() : left.size();
   for (digit_t i = 0; i < more_digits; i++){
      digit_t left_digit = left.at(i);
      digit_t right_digit;
      digit_t new_val;
      if (i <= (right.size() - 1)){
         right_digit = right.at(i);
      }
      else{
         right_digit = 0;
      }
      new_val = left_digit - right_digit - borrow + 10;
      result.push_back(new_val % 10);
      borrow = 0;
      borrow = 1 - new_val/10;
   }
   trim_zeros(result);
   return result;
}

bigvalue_t bigint::do_bigmul (const bigvalue_t& left, 
                              const bigvalue_t& right){
   bigvalue_t result(left.size() + right.size());
   digit_t carry = 0;
   digit_t new_val = 0;
   digit_t j;
   for(digit_t i = 0; i < left.size(); i++){
      carry = 0;
      for(j = 0; j < right.size(); j++){
         new_val = result.at(i+j) + (left.at(i) * right.at(j)) + carry;
         result.at(i+j) = new_val%10;
         carry = new_val/10;
      }
      result.at(i+j) = carry;
   }
   trim_zeros(result);
   return result;
}

bigint operator+ (const bigint& left, const bigint& right) {
   bigint big;
   if (is_zero(left) && is_zero(right)) return big;
   else if (is_zero(left) && !is_zero(right)){
      big.negative = right.negative;
      big.big_value = right.big_value;
      return big;
   }
   else if (!is_zero(left) && is_zero(right)) return left;
   bool left_is_smaller = do_bigless(left.big_value,
                                  right.big_value);
   bool result_is_zero;
   if (left.negative && right.negative){
      big.big_value = big.do_bigadd(left.big_value, right.big_value);
      big.negative = true;
      result_is_zero = is_zero(big);
      if (result_is_zero){big.negative = false; big.big_value.clear();}
      return big;
   }
   else if (!left.negative && !right.negative){
      big.big_value = big.do_bigadd(left.big_value, right.big_value);
      result_is_zero = is_zero(big);
      if (result_is_zero) {big.negative = false; big.big_value.clear();}
      return big;
   }
   if (left.negative && !right.negative){
      if (left_is_smaller){
         big.big_value = big.do_bigsub(right.big_value, left.big_value);
         result_is_zero = is_zero(big);
         if (result_is_zero) {
            big.negative = false;
            big.big_value.clear();
         }
         return big;
      }
      big.big_value = big.do_bigsub(left.big_value, right.big_value);
      big.negative = true;
      result_is_zero = is_zero(big);
      if (result_is_zero || left.big_value == right.big_value) {
         big.negative = false;
         big.big_value.clear();
      }
      return big;
   }
   else if (!left.negative && right.negative){
      if (left_is_smaller){
         big.big_value = big.do_bigsub(right.big_value, left.big_value);
         big.negative = true;
         result_is_zero = is_zero(big);
         if (result_is_zero){
            big.negative = false;
            big.big_value.clear();
         }
         return big;
      }
      big.big_value = big.do_bigsub(left.big_value, right.big_value);
      result_is_zero = is_zero(big);
      if (result_is_zero) {big.negative = false; big.big_value.clear();}
      return big;
   }
   return big;
}

bigint operator- (const bigint& left, const bigint& right) {
   bigint big;
   if (is_zero(left) && is_zero(right)) return big;
   else if (is_zero(left) && !is_zero(right)){
      big.negative = right.negative ? false : true;
      big.big_value = right.big_value;
      return big;
   }
   else if (!is_zero(left) && is_zero(right)) return left;
   bool left_is_smaller = do_bigless(left.big_value,
                                  right.big_value);
   bool result_is_zero;
   if (left.negative && right.negative){
      if (left_is_smaller){
         big.big_value = big.do_bigsub(right.big_value, left.big_value);
         result_is_zero = is_zero(big);
         if (result_is_zero) {
            big.negative = false;
            big.big_value.clear();
         }
         return big;
      }
      big.big_value = big.do_bigsub(left.big_value, right.big_value);
      big.negative = true;
      result_is_zero = is_zero(big);
      if (result_is_zero || left.big_value == right.big_value) {
         big.negative = false;
         big.big_value.clear();
      }
      return big;
   }
   else if(!left.negative && !right.negative){
      if (left_is_smaller){
         big.big_value = big.do_bigsub(right.big_value, left.big_value);
         big.negative = true;
         result_is_zero = is_zero(big);
         if (result_is_zero) {
            big.negative = false;
            big.big_value.clear();
         }
         return big;
      }
      big.big_value = big.do_bigsub(left.big_value, right.big_value);
      result_is_zero = is_zero(big);
      if (result_is_zero) {big.negative = false; big.big_value.clear();}
      return big;
   }
   if (left.negative && !right.negative){
      big.big_value = big.do_bigadd(left.big_value, right.big_value);
      big.negative = true;
      result_is_zero = is_zero(big);
      if (result_is_zero) {big.negative = false; big.big_value.clear();}
      return big;
   }
   else if (!left.negative && right.negative){
      big.big_value = big.do_bigadd(right.big_value, left.big_value);
      result_is_zero = is_zero(big);
      if (result_is_zero) {big.negative = false; big.big_value.clear();}
      return big;
   }
   return big;
}

bigint operator- (const bigint& right) {
   bigint copy = right;
   if (is_zero(right)) return copy;
   if (right.negative) copy.negative = false;
   else copy.negative = true;
   return copy;
}

bool do_bigless (const bigvalue_t& left, const bigvalue_t& right) {
   //checking if left or right is smaller
   if (left.size() < right.size())
      return true;
   else if (left.size() > right.size() ||
           ((left.size() == 0) && (right.size() == 0)))
   {
      return false;
   }
   // left and right are same size
   for (digit_t i = 0; i <= left.size() - 1; i++) {
      digit_t index = left.size() - 1 - i;
      if (left.at(index) > right.at(index)) return false;
      else if (left.at(index) < right.at(index))
         return true;
   }
   return false;
}

//
// Multiplication algorithm.
//
bigint operator* (const bigint& left, const bigint& right) {
   bigint big;
   if ((is_zero(left) && is_zero(right))  ||
       (is_zero(left) && !is_zero(right)) ||
       (!is_zero(left) && is_zero(right)))
   {
      big.negative = false;
      return big;
   }
   
   bool result_is_zero;
   //if both are negative
   if (left.negative && right.negative){
      big.big_value = big.do_bigmul(left.big_value, right.big_value);
   }
   //if both are positive
   else if (!left.negative && !right.negative){
      big.big_value = big.do_bigmul(left.big_value, right.big_value);
   }
   //if left is neg and right is pos
   if (left.negative && !right.negative){
      big.big_value = big.do_bigmul(right.big_value, left.big_value);
      big.negative = true;
   }
   //if left is pos and right is neg
   else if (!left.negative && right.negative){
      big.big_value = big.do_bigmul(right.big_value, left.big_value);
      big.negative = true;
   }
   result_is_zero = is_zero(big);
   if (result_is_zero) big.negative = false;
   return big;
}

void bigint::multiply_by_2 (bigvalue_t& big_val) {
   bigint zero_check;
   zero_check.big_value = big_val;
   if (is_zero(zero_check)) return;
   bigint big;
   big_val = big.do_bigadd(big_val, big_val);
}

void bigint::divide_by_2 (bigvalue_t& big_val) {
   bigvalue_t copy = big_val;
   big_val.clear();
   if (!copy.empty()){
      if ((copy.size() == 1) && copy[0] < 2) return;
   }
   else return;
   digit_t length = copy.size();
   for (digit_t i = 0; i <= length - 1; i++){
      digit_t add;
      if (i == (length - 1)) add = 0;
      else if (i < (length - 1)){
         if (copy.at(i + 1) % 2 == 0) add = 0;
         else if (copy.at(i + 1) % 2 == 1) add = 5;
      }
      digit_t new_val = copy.at(i)/2 + add;
      big_val.push_back(new_val);
   }
   bigint big;
   big.trim_zeros(big_val);
}

//
// Division algorithm.
//

bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (is_zero(right)) throw domain_error ("divide by 0");
   static bigint zero(0);
   if (is_zero(right)) throw domain_error ("bigint::divide");
   bigint divisor = right;
   divisor.negative = false;
   bigint quotient = zero;
   bigint remainder = left;
   remainder.negative = false;
   bigint power_of_2(1);
   // zero divided by anything is clearly zero
   if (is_zero(left)){
      return {zero, zero};
   }
   
   while (do_bigless(divisor.big_value, remainder.big_value)){
      bigint::multiply_by_2 (divisor.big_value);
      bigint::multiply_by_2 (power_of_2.big_value);
   } 
   while (do_bigless(zero.big_value, power_of_2.big_value)) {
      if (not do_bigless(remainder.big_value, divisor.big_value)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      bigint::divide_by_2 (divisor.big_value);
      bigint::divide_by_2 (power_of_2.big_value);
   }
   return {quotient, remainder};
}

bigint operator/ (const bigint& left, const bigint& right) {
   bigint big;
   if ((!is_zero(left) && is_zero(right)) ||
       (is_zero(left) && is_zero(right))){
      throw domain_error ("divide by 0");
   }
   else if (is_zero(left) && !is_zero(right)){
      big.negative = false;
      return big;
   }
   
   bool result_is_zero;
   //if both are negative
   if (left.negative && right.negative){
      big = divide(left, right).first;
   }
   //if both are positive
   else if (!left.negative && !right.negative){
      big = divide(left, right).first;
   }
   //if left is neg and right is pos
   if (left.negative && !right.negative){
      big = divide(left, right).first;
      big.negative = true;
   }
   //if left is pos and right is neg
   else if (!left.negative && right.negative){
      big = divide(left, right).first;
      big.negative = true;
   }
   result_is_zero = is_zero(big);
   if (result_is_zero) big.negative = false;
   return big;
}

bigint operator% (const bigint& left, const bigint& right) {
   bigint big;
   if ((!is_zero(left) && is_zero(right)) ||
       (is_zero(left) && is_zero(right))){
      throw domain_error ("divide by 0");
   }
   else if (is_zero(left) && !is_zero(right)){
      big.negative = false;
      return big;
   }
   
   big = divide(left, right).second;
   bool result_is_zero;
   //if both are negative
   if (left.negative && right.negative){
      big = divide(left, right).second;
   }
   //if both are positive
   else if (!left.negative && !right.negative){
      big = divide(left, right).second;
   }
   //if left is neg and right is pos
   if (left.negative && !right.negative){
      big = divide(left, right).second;
      big.negative = true;
   }
   //if left is pos and right is neg
   else if (!left.negative && right.negative){
      big = divide(left, right).second;
      big.negative = true;
   }
   result_is_zero = is_zero(big);
   if (result_is_zero) big.negative = false;
   return big;
}

bool operator== (const bigint& left, const bigint& right) {
   return ((left.big_value == right.big_value) &&
           (left.negative == right.negative));
}

bool operator< (const bigint& left, const bigint& right) {
   if (is_zero(left) && !is_zero(right)){
      return (right.negative ? false : true);
   }
   
   if (!is_zero(left) && is_zero(right)){
      return (right.negative ? true : false);
   }
   
   if ((left.negative && right.negative) ||
       (left.negative && !right.negative))
   {
      return (not do_bigless(left.big_value, right.big_value));
   }
   
   if ((!left.negative && !right.negative) ||
       (!left.negative && right.negative))
   {
      return (do_bigless(left.big_value, right.big_value));
   }
   return false;
}

ostream& operator<< (ostream& out, const bigint& that) {
   auto itor = that.big_value.rbegin();
   if (that.big_value.size() == 0){
      out << "0";
      return out;
   }
   if (that.negative == true){
      out << "-";
   }
   for (int i = 1; itor != that.big_value.rend(); itor++, i++){
      if (i == 70){
         out << "\\" << endl;
         i = 1;
      } 
      out << *itor + 0;
   }
   return out;
}

// doesn't fully work with big numbers, might look into when I have time
bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   bigint big;
   if (base == 0) return big;
   bigint base_copy = base;
   bigint expt = exponent;
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt % 2 == 1) { //odd
         result = result * base_copy;
         cout << "result = " << result << endl;
         expt = expt - 1;
      }else { //even
         base_copy = base_copy * base_copy;
         expt = expt / 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}
