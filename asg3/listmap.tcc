// $Id: listmap.tcc,v 1.14 2015-02-18 02:12:30-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg3

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);
}

//
// listmap::empty()
//
template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() {
   return size == 0;
}

//
// listmap::iterator listmap::begin()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
   return iterator (anchor_.next);
}

//
// listmap::iterator listmap::end()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end(){
   return iterator (anchor());
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   TRACE ('l', &pair << "->" << pair);
   iterator i;
   if (empty()){
      node* n = new node(anchor(), anchor(), pair);
      begin().where->prev = n;
      begin().where->next = n;
   }
   else{
      for (i = begin(); i != end(); ++i){
         value_type cpy = pair;
         Key n_key = cpy.first;
         Key curr_key = i.where->value.first;
         bool is_less = less (n_key, curr_key);
         bool is_equal = !less(n_key,curr_key) && !less(curr_key,n_key);
         bool is_greater = not is_less and not is_equal;
         // new node's key less than iterator's key
         if (is_less) {
            if (i.where->next == anchor()){
               node *n = new node(i.where, i.where->prev, pair);
               i.where->prev->next = n;
               i.where->prev = n;
               break;
            }
            else{
               node *n = new node(i.where->next, i.where, pair);
               i.where->next->prev = n;
               i.where->next = n;
               break;
            }
            continue;
         }
         // new node's key equal to iterator's key
         else if (is_equal){
            i.where->value.second = pair.second;
            break;
         }
         // new node's greater than iterator's key
         else if (is_greater){
            if (i.where->next != anchor()){
               Key next_key = i.where->next->value.first;
               is_less = less (next_key, curr_key);
               is_equal = !less(next_key, curr_key) && 
                          !less (curr_key, next_key);
               is_greater = not is_less and not is_equal;
               if (is_less){
                  node *n = new node(i.where->next, i.where, pair);
                  i.where->next->prev = n;
                  i.where->next = n;
                  break;
               }
               else continue;
            }
            else{
               node *n = new node(i.where->next, i.where, pair);
               i.where->next->prev = n;
               i.where->next = n;
               break;
            }
         }
      }
   }
   size++;
   return i;
}

//
// iterator listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
   iterator i;
   if (empty()){
      return end();
   }
   else{
      for (i = begin(); i != end(); ++i){
         Key curr_key = i.where->value.first;
         bool is_less = less (that, curr_key);
         bool is_equal = !less(that, curr_key) && !less (curr_key,that);
         if (is_less) continue;
         else if (is_equal) return i;
      }
      i = end();
   }
   return i;
}

//
// iterator listmap::erase (iterator position)
//
// Returns an iterator pointing to the element that followed the last
// element erased by the function call
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   iterator i;
   if (empty()){
      return end();
   }
   else{
      for (i = begin(); i != end(); ++i){
         if (i == position){
            i.where->prev->next = i.where->next;
            i.where->next->prev = i.where->prev; 
            if (i.where->next == nullptr){
               i.where = i.where->prev;
            }
            else{
               i.where = i.where->next;
            }
            break;
         }
         else continue;
      }
      size--;
   }
   return i;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that){
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that){
   return this->where != that.where;
}

