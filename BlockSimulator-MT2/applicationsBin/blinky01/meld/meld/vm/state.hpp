
#ifndef VM_STATE_HPP
#define VM_STATE_HPP

#include <list>
#include <iostream>

#include "conf.hpp"
#include "vm/tuple.hpp"
#include "db/node.hpp"
#include "vm/program.hpp"
#include "vm/instr.hpp"
#include "db/trie.hpp"
#include "vm/all.hpp"
#include "utils/random.hpp"
#include "utils/time.hpp"
#include "queue/safe_simple_pqueue.hpp"
#include "runtime/struct.hpp"
#include "vm/stat.hpp"

// forward declaration
namespace sched {
	class base;
}

namespace vm {

  extern all* All;                // global variable that holds pointer
  // to vm all structure.  Set by
  // process/machine.cpp in constructor.

  static const size_t NUM_REGS = 32;

  class state
  {
  private:
   
    db::tuple_trie_leaf *saved_leafs[NUM_REGS];
    db::simple_tuple *saved_stuples[NUM_REGS];
    bool is_leaf[NUM_REGS];
	
    std::list<runtime::cons*, mem::allocator<runtime::cons*> > free_cons;
    std::list<runtime::rstring::ptr, mem::allocator<runtime::rstring::ptr> > free_rstring;
    std::list<runtime::struct1*, mem::allocator<runtime::struct1*> > free_struct1;
   
    typedef std::pair<db::tuple_trie_leaf *, vm::ref_count> pair_linear;
    typedef std::list<pair_linear> list_linear;

    /* execution data for when using rules */
    bool *rules;
    bool *predicates;
    queue::heap_queue<vm::rule_id> rule_queue;
	
    void purge_runtime_objects(void);
    void start_matching(void);
#ifdef CORE_STATISTICS
    void init_core_statistics(void);
#endif
    db::simple_tuple* search_for_negative_tuple_partial_agg(db::simple_tuple *);
    db::simple_tuple* search_for_negative_tuple_full_agg(db::simple_tuple *);
    db::simple_tuple* search_for_negative_tuple_normal(db::simple_tuple *);

  public:

    typedef tuple_field reg;
    reg regs[NUM_REGS];
    std::vector<tuple_field> stack;
    vm::tuple *tuple;
    db::tuple_trie_leaf *tuple_leaf;
    db::simple_tuple *tuple_queue;
    db::node *node;
    derivation_count count;
    vm::depth_t depth;
    sched::base *sched;
    bool is_linear;
    list_linear used_linear_tuples;
    utils::randgen randgen;
    size_t current_rule;
#ifdef DEBUG_MODE
    bool print_instrs;
#endif
#ifdef CORE_STATISTICS
   core_statistics stat;
#endif
    bool use_local_tuples;
    db::simple_tuple_list local_tuples; // current available tuples not yet in the database
    db::simple_tuple_list generated_tuples; // tuples generated while running the rule
    db::simple_tuple_list generated_persistent_tuples; // persistent tuples while running the rule
    db::simple_tuple_vector generated_other_level; // tuples for later computation (another stratification level or time delay)
    // leaves scheduled for deletion (for use with reused linear tuples + retraction)
    // we cannot delete them immediately because then the tuple would be deleted
    std::list< std::pair<vm::predicate*, db::tuple_trie_leaf*> > leaves_for_deletion;
    vm::strat_level current_level;
    bool persistent_only; // we are running one persistent tuple (not a rule)

#define define_get(WHAT, RET, BODY)                                     \
    inline RET get_ ## WHAT (const reg_num& num) const { BODY; }
   
    define_get(reg, reg, return regs[num]);
    define_get(int, int_val, return FIELD_INT(regs[num]));
    define_get(float, float_val, return FIELD_FLOAT(regs[num]));
    define_get(ptr, ptr_val, return FIELD_PTR(regs[num]));
    define_get(bool, bool_val, return FIELD_BOOL(regs[num]));
    define_get(string, runtime::rstring::ptr, return FIELD_STRING(regs[num]););
    define_get(cons, runtime::cons*, return FIELD_CONS(regs[num]));
    define_get(tuple, vm::tuple*, return (vm::tuple*)get_ptr(num));
    define_get(node, vm::node_val, return FIELD_NODE(regs[num]));
    define_get(struct, runtime::struct1*, return FIELD_STRUCT(regs[num]));
   
#undef define_get

#define define_set(WHAT, ARG, BODY)                                     \
    inline void set_ ## WHAT (const reg_num& num, ARG val) { BODY; };
   
    define_set(float, const float_val&, SET_FIELD_FLOAT(regs[num], val));
    define_set(int, const int_val&, SET_FIELD_INT(regs[num], val));
    define_set(ptr, const ptr_val&, SET_FIELD_PTR(regs[num], val));
    define_set(bool, const bool_val&, SET_FIELD_BOOL(regs[num], val));
    define_set(string, const runtime::rstring::ptr, SET_FIELD_STRING(regs[num], val));
    define_set(cons, runtime::cons*, SET_FIELD_CONS(regs[num], val));
    define_set(tuple, vm::tuple*, set_ptr(num, (ptr_val)val));
    define_set(node, const node_val, SET_FIELD_NODE(regs[num], val));
    define_set(struct, runtime::struct1*, SET_FIELD_STRUCT(regs[num], val));
   
#undef define_set
   
    inline void set_reg(const reg_num& num, const reg val) { regs[num] = val; }
    inline void set_nil(const reg_num& num) { set_ptr(num, null_ptr_val); }
    inline reg get_reg(const reg_num& num) { return regs[num]; }
   
    inline void set_leaf(const reg_num& num, db::tuple_trie_leaf* leaf) { is_leaf[num] = true; saved_leafs[num] = leaf; }
    inline db::tuple_trie_leaf* get_leaf(const reg_num& num) const { return saved_leafs[num]; }
    inline void set_tuple_queue(const reg_num& num, db::simple_tuple *stpl) { is_leaf[num] = false; saved_stuples[num] = stpl; }
    inline db::simple_tuple* get_tuple_queue(const reg_num& num) const { return saved_stuples[num]; }
    inline bool is_it_a_leaf(const reg_num& num) const { return is_leaf[num]; }

    inline tuple_field* get_stack_at(const offset_num& off) { return &stack[stack.size() - 1 - off]; }
   
    inline void copy_reg(const reg_num& reg_from, const reg_num& reg_to) {
      regs[reg_to] = regs[reg_from];
    }

    void copy_reg2const(const reg_num&, const const_id&);
   
    inline void add_cons(runtime::cons *ls) { ls->inc_refs();
      free_cons.push_back(ls); }
    inline void add_string(runtime::rstring::ptr str) { str->inc_refs();
      free_rstring.push_back(str); }
    inline void add_struct(runtime::struct1 *s) { s->inc_refs();
      free_struct1.push_back(s); }
    inline void add_generated_tuple(db::simple_tuple *tpl) { tpl->set_generated_run(true); generated_tuples.push_back(tpl); }
   
    bool add_fact_to_node(vm::tuple *, const vm::derivation_count count = 1, const vm::depth_t depth = 0);
	
	bool check_if_rule_predicate_activated(vm::rule *);
	
    void mark_predicate_to_run(const vm::predicate *);
    void mark_active_rules(void);
    void add_to_aggregate(db::simple_tuple *);
    bool do_persistent_tuples(void);
    void process_persistent_tuple(db::simple_tuple *, vm::tuple *);
    void process_consumed_local_tuples(void);
    void print_local_tuples(std::ostream& cout);
    void print_generated_tuples(std::ostream& cout);

    void process_others(void);
    vm::strat_level mark_rules_using_local_tuples(db::simple_tuple_list&);

    void run_node(db::node *);

    void setup(vm::tuple*, db::node*, const vm::derivation_count, const vm::depth_t);

    void cleanup(void);
    bool linear_tuple_can_be_used(db::tuple_trie_leaf *) const;
    void using_new_linear_tuple(db::tuple_trie_leaf *);
    void no_longer_using_linear_tuple(db::tuple_trie_leaf *);
    void unmark_generated_tuples(void);

    explicit state(sched::base *);
    explicit state(void);
    ~state(void);
  };

}

#endif


// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
