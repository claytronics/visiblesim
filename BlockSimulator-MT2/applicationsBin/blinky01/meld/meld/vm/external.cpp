
#include <tr1/unordered_map>
#include <assert.h>
#include <stdlib.h>
#include<iostream>

#include "vm/external.hpp"
#include "external/math.hpp"
#include "external/utils.hpp"
#include "external/lists.hpp"
#include "external/strings.hpp"
#include "external/others.hpp"
#include "external/core.hpp"

using namespace std;
using namespace std::tr1;

namespace vm
{
   
using namespace external;
using namespace std;

typedef unordered_map<external_function_id, external_function*> hash_external_type;

//static bool init_external_functions(void);
static external_function_id external_counter(0);   
static hash_external_type hash_external;
//static bool dummy(init_external_functions());
static bool external_functions_initialised(false);

void
external_function::set_arg_type(const size_t arg, type *typ)
{
   assert(arg < num_args);
   
   spec[arg] = typ;
}

external_function::external_function(external_function_ptr _ptr,
         const size_t _num_args,
         type *_ret):
   num_args(_num_args),
   ptr(_ptr), ret(_ret),
   spec(new type*[_num_args])
{
   assert(num_args <= EXTERNAL_ARG_LIMIT);
}

external_function::~external_function(void)
{
   delete []spec;
}

external_function_id
register_external_function(external_function *ex)
{
   hash_external[external_counter] = ex;
   return external_counter++;
}

external_function*
lookup_external_function(const external_function_id id)
{
   external_function *ret(hash_external[id]);
   cout<<"lookup_external_function : id : "<<id<<endl; 
   assert(ret != NULL);
   
   return ret;
}

external_function*
external0(external_function_ptr ptr, type *ret)
{
   return new external_function(ptr, 0, ret);
}

external_function*
external1(external_function_ptr ptr, type *ret, type *arg1)
{
   external_function *f(new external_function(ptr, 1, ret));
   
   f->set_arg_type(0, arg1);
   
   return f;
}

external_function*
external2(external_function_ptr ptr, type *ret, type *arg1, type *arg2)
{
   external_function *f(new external_function(ptr, 2, ret));
   
   f->set_arg_type(0, arg1);
   f->set_arg_type(1, arg2);
   
   return f;
}

external_function*
external3(external_function_ptr ptr, type *ret, type *arg1, type *arg2, type *arg3)
{
   external_function *f(new external_function(ptr, 3, ret));
   
   f->set_arg_type(0, arg1);
   f->set_arg_type(1, arg2);
   f->set_arg_type(2, arg3);
   
   return f;
}

static void
cleanup_externals(void)
{
   for(hash_external_type::iterator it(hash_external.begin()), end(hash_external.end()); it != end; it++)
      delete it->second;
}

bool
init_external_functions(void)
{
   if(external_functions_initialised) 
    return true;
   else external_functions_initialised = true; 

#define EXTERN(NAME) (external_function_ptr) external :: NAME
#define EXTERNAL0(NAME, RET) external0(EXTERN(NAME), RET)
#define EXTERNAL1(NAME, RET, ARG1) external1(EXTERN(NAME), RET, ARG1)
#define EXTERNAL2(NAME, RET, ARG1, ARG2) external2(EXTERN(NAME), RET, ARG1, ARG2)
#define EXTERNAL3(NAME, RET, ARG1, ARG2, ARG3) external3(EXTERN(NAME), RET, ARG1, ARG2, ARG3)
   static type *i(TYPE_INT);
   static type *f(TYPE_FLOAT);
   static type *n(TYPE_NODE);
   static type *s(TYPE_STRING);
   static list_type *li(TYPE_LIST_INT);
   static list_type *lf(TYPE_LIST_FLOAT);
   static list_type *ln(TYPE_LIST_NODE);

   register_external_function(EXTERNAL1(sigmoid, f, f));
   register_external_function(EXTERNAL1(randint, i, i));
   register_external_function(EXTERNAL1(normalize, lf, lf));
   register_external_function(EXTERNAL3(damp, lf, lf, lf, f));
   register_external_function(EXTERNAL2(divide, lf, lf, lf));
   register_external_function(EXTERNAL2(convolve, lf, lf, lf));
   register_external_function(EXTERNAL2(addfloatlists, lf, lf, lf));
   register_external_function(EXTERNAL1(listlength, i, li));
   register_external_function(EXTERNAL2(intlistdiff, li, li, li));
   register_external_function(EXTERNAL2(intlistnth, i, li, i));
	register_external_function(EXTERNAL2(concatenate, s, s, s));
	register_external_function(EXTERNAL1(str2float, f, s));
	register_external_function(EXTERNAL1(str2int, i, s));
	register_external_function(EXTERNAL2(nodelistremove, ln, ln, n));
	register_external_function(EXTERNAL1(wastetime, i, i));
	register_external_function(EXTERNAL2(truncate, f, f, i));
	register_external_function(EXTERNAL1(float2int, i, f));
	register_external_function(EXTERNAL1(int2str, s, i));
	register_external_function(EXTERNAL1(float2str, s, f));
   register_external_function(EXTERNAL3(intlistsub, li, li, i, i));
   register_external_function(EXTERNAL2(listappend, li, li, li));
   register_external_function(EXTERNAL1(str2intlist, li, s));
   register_external_function(EXTERNAL2(filecountwords, i, s, i));
   register_external_function(EXTERNAL2(residual, f, lf, lf));
   register_external_function(EXTERNAL1(listlength, i, ln));
   register_external_function(EXTERNAL2(nodelistcount, i, ln, n));
   register_external_function(EXTERNAL2(listappend, ln, ln, ln));
   register_external_function(EXTERNAL1(node_priority, f, n));
   register_external_function(EXTERNAL1(listreverse, ln, ln));
   register_external_function(EXTERNAL1(listlast, n, ln));
   register_external_function(EXTERNAL1(cpu_id, i, n));
   register_external_function(EXTERNAL1(node2int, i, n));
   register_external_function(EXTERNAL2(intpower, i, i, i));
   register_external_function(EXTERNAL1(intlistsort, li, li));
   register_external_function(EXTERNAL1(intlistremoveduplicates, li, li));
   register_external_function(EXTERNAL2(degeneratevector, li, i, i));
   register_external_function(EXTERNAL2(demergemessages, li, li, li));
   register_external_function(EXTERNAL2(intlistequal, i, i, i));

   atexit(cleanup_externals);
   
   return true;
}

}
