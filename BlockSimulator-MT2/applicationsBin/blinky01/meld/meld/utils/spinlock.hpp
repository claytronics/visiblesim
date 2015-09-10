
#ifndef UTILS_SPINLOCK_HPP
#define UTILS_SPINLOCK_HPP

#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>


namespace utils
{

class spinlock {
private:
   volatile unsigned int data;

    boost::detail::spinlock *s;
   inline bool is_locked(void) const { return data != 0; }

public:

   inline bool try_lock(void)
   {
      char tmp = 1;
          __asm__ __volatile__(
             "xchgb %b0, %1"
             : "=q"(tmp), "=m"(data)
             : "0"(tmp) : "memory");
      return tmp == 0;
   }

   inline void unlock(void)
   {
     /* To unlock we move 0 to the lock.
      * On i386 this needs to be a locked operation
      * to avoid Pentium Pro errata 66 and 92.
      */
       s->unlock();
   }

   inline void lock(void)
   {
       s->lock();
   }

   typedef boost::lock_guard<spinlock> scoped_lock;

   explicit spinlock(void):
      data(0)
   {
       s = new boost::detail::spinlock();
   }

    ~spinlock(void) {delete s;}

};

}
#endif
