
#ifndef VM_ALL_HPP
#define VM_ALL_HPP

#if defined(TARGET_mpi)
# include <boost/mpi.hpp>
#endif
#include "vm/program.hpp"

#define MAX_CONSTS 32

// forward declaration
namespace db {
   class database;
}

namespace process {
   class remote;
   class machine;
}

namespace sched {
	class base;
}

namespace vm
{

  typedef std::vector<std::string> machine_arguments;

  /* state of the virtual machine */
  class all
  {
  private:

    tuple_field consts[MAX_CONSTS];
	
  public:

    vm::program *PROGRAM;
   
    db::database *DATABASE;
    process::machine *MACHINE;
    size_t NUM_THREADS;
    size_t NUM_NODES_PER_PROCESS;
    machine_arguments ARGUMENTS;
    std::vector<sched::base*> ALL_THREADS; /* schedulers */

    inline void set_const(const const_id& id, const tuple_field d) { consts[id] = d; }

#define define_get_const(WHAT, TYPE, CODE) TYPE get_const_ ## WHAT (const const_id& id) { return CODE; }
	
    define_get_const(int, int_val, FIELD_INT(consts[id]))
    define_get_const(float, float_val, FIELD_FLOAT(consts[id]))
    define_get_const(ptr, ptr_val, FIELD_PTR(consts[id]));
    define_get_const(cons, runtime::cons*, FIELD_CONS(consts[id]))
    define_get_const(string, runtime::rstring*, FIELD_STRING(consts[id]))
    define_get_const(node, node_val, FIELD_NODE(consts[id]))
    define_get_const(struct, runtime::struct1*, FIELD_STRUCT(consts[id]))
	
#undef define_get_const

    inline tuple_field get_const(const const_id& id) { return consts[id]; }
	
#define define_set_const(WHAT, TYPE, CODE) void set_const_ ## WHAT (const const_id& id, const TYPE val) { CODE;}
	
    define_set_const(int, int_val&, SET_FIELD_INT(consts[id], val))
    define_set_const(float, float_val&, SET_FIELD_FLOAT(consts[id], val))
    define_set_const(node, node_val&, SET_FIELD_NODE(consts[id], val))
    define_set_const(ptr, ptr_val&, SET_FIELD_PTR(consts[id], val))
    define_set_const(string, runtime::rstring*, SET_FIELD_STRING(consts[id], val))
	
#undef define_set_const

    inline runtime::rstring::ptr get_argument(const argument_id id)
    {
      assert(id <= ARGUMENTS.size());
      return runtime::rstring::make_string(ARGUMENTS[id-1]);
    }
	
    explicit all(void) {}
  };

  extern all* All;

}

#endif


// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:

