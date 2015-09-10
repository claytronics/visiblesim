#ifndef SCHED_NODES_SERIAL_HPP
#define SCHED_NODES_SERIAL_HPP

#include "mem/base.hpp"
#include "db/tuple.hpp"
#include "utils/spinlock.hpp"
#include "sched/base.hpp"
#include "queue/intrusive.hpp"
#include "sched/nodes/in_queue.hpp"
#include "queue/bounded_pqueue.hpp"


// Node type for sequential scheduler
namespace sched
{

  class serial_node: public in_queue_node
  {
    /*Making it compatible with simulator*/
  public:
    DECLARE_DOUBLE_QUEUE_NODE(serial_node);
    typedef queue::unsafe_bounded_pqueue<db::simple_tuple*>::type queue_type;
    queue_type queue;
   
    typedef queue_type::const_iterator queue_iterator;

    inline queue_iterator begin(void) const { return queue.begin(); }
    inline queue_iterator end(void) const { return queue.end(); }

    inline void add_work(db::simple_tuple *stpl)
    {
      queue.push(stpl, stpl->get_strat_level());
    }

    inline bool has_work(void) const { return !queue.empty(); }

    virtual void assert_end(void) const
    {
      in_queue_node::assert_end();
      assert(!has_work());
    }

    virtual void assert_end_iteration(void) const
    {
      in_queue_node::assert_end_iteration();
      assert(!has_work());
    }



    /*Changed constructor to conform to new member variables*/
    explicit serial_node(const db::node::node_id _id, const db::node::node_id _trans):
      in_queue_node(_id, _trans),
      INIT_DOUBLE_QUEUE_NODE(),
      queue(vm::All->PROGRAM->MAX_STRAT_LEVEL)
    {}

    virtual ~serial_node(void) { }
  };

}

#endif


// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
