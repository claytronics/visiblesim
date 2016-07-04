
#ifndef VM_MATCH_HPP
#define VM_MATCH_HPP

#include <vector>
#include <stack>
#include <ostream>

#include "vm/types.hpp"
#include "vm/defs.hpp"
#include "mem/allocator.hpp"
#include "vm/predicate.hpp"
#include "utils/stack.hpp"
#include "vm/types.hpp"

namespace vm
{

struct match_field {
   bool exact;
   type *ty;
};

typedef utils::stack<vm::tuple_field> val_stack;
typedef utils::stack<vm::type*> type_stack;
typedef utils::stack<match_field> match_type_stack;
typedef val_stack match_val_stack;

class match: public mem::base
{
private:
   
   bool any_exact;
   std::vector<match_field, mem::allocator<match_field> > types;
   std::vector<tuple_field, mem::allocator<tuple_field> > vals;
   
   inline void set_any_all(const predicate *pred) {
      for(size_t i(0); i < types.size(); ++i) {
         types[i].exact = false;
         types[i].ty = pred->get_field_type(i);
      }
   }
   
public:

   MEM_METHODS(match)
   
   inline size_t size(void) const { return types.size(); }
   
   inline bool has_any_exact(void) const { return any_exact; }
   
   inline void get_val_stack(match_val_stack& stk) const {
      for(int i(vals.size()-1); i >= 0; --i)
         stk.push(vals[i]);
   }
   
   inline void get_type_stack(match_type_stack& stk) const {
      for(int i(types.size()-1); i >= 0; --i)
         stk.push(types[i]);
   }

   inline void match_int(const field_num& num, const int_val val) {
      assert(num < types.size());
      types[num].exact = true;
      types[num].ty = TYPE_INT;
      SET_FIELD_INT(vals[num], val);
      any_exact = true;
   }
   
   inline void match_float(const field_num& num, const float_val val) {
      assert(num < types.size());
      types[num].exact = true;
      types[num].ty = TYPE_FLOAT;
      SET_FIELD_FLOAT(vals[num], val);
      any_exact = true;
   }
   
   inline void match_node(const field_num& num, const node_val val) {
      assert(num < types.size());
      types[num].exact = true;
      types[num].ty = TYPE_NODE;
      SET_FIELD_NODE(vals[num], val);
      any_exact = true;
   }
   
   inline void print(std::ostream& cout) const {
      for(size_t i(0); i < types.size(); ++i) {
         if(types[i].exact) {
            switch(types[i].ty->get_type()) {
               case FIELD_INT: cout << (int)i << " -> " << FIELD_INT(vals[i]) << std::endl; break;
               case FIELD_FLOAT: cout << (int)i << " -> " << FIELD_FLOAT(vals[i]) << std::endl; break;
               case FIELD_NODE: cout << (int)i << " -> " << FIELD_NODE(vals[i]) << std::endl; break;
               default: assert(false);
            }
         }
      }
   }
   
   explicit match(const predicate *pred):
      any_exact(false)
   {
      types.resize(pred->num_fields());
      vals.resize(pred->num_fields());
      set_any_all(pred);
   }
};
   
}

#endif
