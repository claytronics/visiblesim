
#ifndef VM_RULE_MATCHER_HPP
#define VM_RULE_MATCHER_HPP

#include <vector>
#include <set>

#include "vm/predicate.hpp"
#include "vm/tuple.hpp"

namespace vm
{

  extern program *theProgram;

  class rule_matcher
  {
  private:
	
   class rule_matcher_obj
   {
      public:
         size_t total_have;
         size_t total_needed;
         bool ignore;
   };

	typedef std::vector<rule_matcher_obj> rule_vector;

   rule_vector rules; /* availability statistics per rule */
   std::vector<ref_count> predicate_count; /* number of tuples per predicate */

	typedef std::set<rule_id, std::less<rule_id>, mem::allocator<rule_id> > set_rules;
	set_rules active_rules; /* rules that *may* be derivable */
	set_rules dropped_rules; /* any dropped rule */
	
	void register_predicate_availability(const predicate *);
	void register_predicate_unavailability(const predicate *);

  public:

   /* returns true if we did not have any tuples of this predicate */
	bool register_tuple(tuple *, const derivation_count, const bool is_new = true);

    /* returns true if now we do not have any tuples of this predicate */
    bool deregister_tuple(tuple *, const derivation_count);

    void clear_dropped_rules(void)
    {
      dropped_rules.clear();
    }

    typedef set_rules::const_iterator rule_iterator;
	
	rule_iterator begin_active_rules(void) const { return active_rules.begin(); }
	rule_iterator end_active_rules(void) const { return active_rules.end(); }
	rule_iterator begin_dropped_rules(void) const { return dropped_rules.begin(); }
	rule_iterator end_dropped_rules(void) const { return dropped_rules.end(); }

    // one per node, but no argument for the program since they all
    // share the same program.  So, the program which used to be
    // specified with 'vm::program *' will now be a global.
    rule_matcher(void);
  };

}

#endif


// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
