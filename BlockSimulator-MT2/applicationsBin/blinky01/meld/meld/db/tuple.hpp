
#ifndef DB_TUPLE_HPP
#define DB_TUPLE_HPP

#include "conf.hpp"

#include <vector>
#include <ostream>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "vm/defs.hpp"
#include "vm/predicate.hpp"
#include "vm/tuple.hpp"
#include "mem/allocator.hpp"
#include "mem/base.hpp"
#include "utils/types.hpp"

namespace db
{
   
class simple_tuple: public mem::base
{
private:

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
     (void)version;
     ar & data;
  }

   vm::tuple *data;
   vm::derivation_count count;
   vm::depth_t depth;
   // tuple is dead and must be deleted at the end of rule execution
	bool to_delete;
   // if tuple was generated during this rule, mark as it as such so that it cannot be used
   bool generated_this_run;
   // if tuple is a final aggregate
   bool is_final_aggregate;

public:

   MEM_METHODS(simple_tuple)

   inline vm::tuple* get_tuple(void) const { return data; }
   
   inline vm::strat_level get_strat_level(void) const
   {
      return get_tuple()->get_predicate()->get_strat_level();
   }

	inline const vm::predicate* get_predicate(void) const
	{
		return get_tuple()->get_predicate();
	}
   
   inline vm::predicate_id get_predicate_id(void) const
   {
      return get_tuple()->get_predicate_id();
   }

   inline void set_generated_run(const bool v)
   {
      generated_this_run = v;
   }

   inline bool get_generated_run(void) const
   {
      return generated_this_run;
   }

   inline bool must_be_deleted(void) const
   {
      return to_delete;
   }
   
   inline void will_delete(void)
   {
	   to_delete = true;
   }
   
   inline void will_not_delete(void)
   {
	   to_delete = false;
   }

   inline bool can_be_consumed(void) const
   {
      return !to_delete && !generated_this_run;
   }

   inline bool is_aggregate(void) const
   {
      return is_final_aggregate;
   }

   inline void set_as_aggregate(void)
   {
      is_final_aggregate = true;
   }

   void print(std::ostream&) const;

   inline vm::derivation_count get_count(void) const { return count; }
   
   inline bool reached_zero(void) const { return get_count() == 0; }

   inline void inc_count(const vm::derivation_count& inc) { assert(inc > 0); count += inc; }

   inline void dec_count(const vm::derivation_count& inc) { assert(inc > 0); count -= inc; }
   
   inline void add_count(const vm::derivation_count& inc) { count += inc; }

   inline vm::depth_t get_depth(void) const { return depth; }
   
   inline size_t storage_size(void) const
   {
      return sizeof(vm::derivation_count) + sizeof(vm::depth_t) + data->get_storage_size();
   }
   
   void pack(utils::byte *, const size_t, int *) const;
   
   static simple_tuple* unpack(utils::byte *, const size_t, int *, vm::program *);

   static simple_tuple* create_new(vm::tuple *tuple, const vm::depth_t depth) { return new simple_tuple(tuple, 1, depth); }

   static simple_tuple* remove_new(vm::tuple *tuple, const vm::depth_t depth) { return new simple_tuple(tuple, -1, depth); }
   
   static void wipeout(simple_tuple *stpl) { delete stpl->get_tuple(); delete stpl; }

   explicit simple_tuple(vm::tuple *_tuple, const vm::derivation_count _count, const vm::depth_t _depth = 0):
      data(_tuple), count(_count), depth(_depth),
      to_delete(false), generated_this_run(false),
      is_final_aggregate(false)
   {}

   explicit simple_tuple(void): // for serialization purposes
      to_delete(false), generated_this_run(false),
      is_final_aggregate(false)
   {
   }

   ~simple_tuple(void);
};

std::ostream& operator<<(std::ostream&, const simple_tuple&);

typedef std::list<simple_tuple*, mem::allocator<simple_tuple*> > simple_tuple_list;
typedef std::vector<simple_tuple*, mem::allocator<simple_tuple*> > simple_tuple_vector;

}

#endif
