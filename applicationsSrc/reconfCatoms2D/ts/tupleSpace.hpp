#ifndef TUPLE_SPACE_H_
#define TUPLE_SPACE_H_

#include "tuple.hpp"

class TupleSpace {
public:
  TupleSpace();
  ~TupleSpace();

  virtual void out(Tuple *t) = 0;
  virtual Tuple* in(const Tuple &q) = 0;
  virtual Tuple* inp(const Tuple &q) = 0;
};

#endif
