
#ifndef PROCESS_MACHINE_HPP
#define PROCESS_MACHINE_HPP

#include "conf.hpp"

#include <stdexcept>
#include <string>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>
#if defined(TARGET_mpi)
# include <boost/mpi.hpp>
#endif
#include "db/database.hpp"
#include "db/tuple.hpp"
#include "db/node.hpp"
#include "sched/types.hpp"
#include "stat/slice_set.hpp"
#include "vm/state.hpp"
#include "sched/base.hpp"

extern vm::all* vm::All;

namespace process
{

  class machine
  {

  private:
    const std::string filename;
    const sched::scheduler_type sched_type;

    boost::thread *alarm_thread;
    statistics::slice_set slices;

    void execute_const_code(void);
    void deactivate_signals(void);
    void slice_function(void);
    void set_timer(void);

  public:


    sched::scheduler_type get_sched_type(void) const { return sched_type; }

    sched::base *get_scheduler(const vm::process_id id) { return vm::All->ALL_THREADS[id]; }

    vm::all *get_all(void) const { return vm::All; }

    void run_action(sched::base *, db::node *, vm::tuple *, const bool from_other = false);
    void route_self(sched::base *, db::node *, db::simple_tuple *, const vm::uint_val delay = 0);

    void route(const db::node *, sched::base *, const db::node::node_id, db::simple_tuple*, const vm::uint_val delay = 0);

    void init_thread(sched::base *);
    void start(void);

    explicit machine(const std::string&, const size_t, const sched::scheduler_type, const vm::machine_arguments& args = vm::machine_arguments(), const std::string& data_file = std::string());

    ~machine(void);
  };

  class machine_error : public std::runtime_error {
  public:
    explicit machine_error(const std::string& msg) :
      std::runtime_error(msg)
    {}
  };

}

#endif


// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
