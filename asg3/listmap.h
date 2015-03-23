// $Id: listmap.h,v 1.6 2015-02-17 18:09:38-08 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg3

#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "xless.h"
#include "xpair.h"

template <typename Key, typename Value, class Less=xless<Key>>
class listmap {
   public:
      using key_type = Key;  // Key from template typename
      using mapped_type = Value;  // Value from template typename
      using value_type = xpair<const key_type, mapped_type>;// Pair of 
                                                           // Key, Value
   private:
      Less less; //  xless<Key>
      struct node;
      // Link represents the list itself and is contained in every node.
      // Is a circular doubly linked list
      struct link {
         node* next{};
         node* prev{};
         link (node* next, node* prev): next(next), prev(prev){}
      };
      struct node: link {
         value_type value{};  // again, the xpair of key, value
         node (node* next, node* prev, const value_type&);
      };
      // begin is anchor->next
      // end is anchor
      node* anchor() { return static_cast<node*> (&anchor_); }
      link anchor_ {anchor(), anchor()};
      size_t size = 0;
   public:
      class iterator;
      listmap(){};
      listmap (const listmap&);
      listmap& operator= (const listmap&);
      ~listmap();
      iterator insert (const value_type&);
      iterator find (const key_type&);
      iterator erase (iterator position);
      iterator begin();
      iterator end();
      bool empty();
};


template <typename Key, typename Value, class Less=xless<Key>>
class listmap<Key,Value,Less>::iterator {
   private:
      friend class listmap<Key,Value>;
      listmap<Key,Value,Less>::node* where {nullptr};
      iterator (node* where): where(where){};
   public:
      iterator(){}
      value_type& operator*();
      value_type* operator->();
      iterator& operator++(); //++itor
      iterator& operator--(); //--itor
      void erase();
      bool operator== (const iterator&);
      bool operator!= (const iterator&);
};

#include "listmap.tcc"
#endif

