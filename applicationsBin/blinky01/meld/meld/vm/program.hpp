
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "conf.hpp"

#include <string>
#include <vector>
#include <stdexcept>
#include <ostream>

#include "vm/external.hpp"
#include "vm/types.hpp"
#include "utils/types.hpp"
#include "vm/predicate.hpp"
#include "vm/defs.hpp"
#include "vm/tuple.hpp"
#include "vm/rule.hpp"
#include "vm/function.hpp"
#include "runtime/string.hpp"
#include "queue/heap_implementation.hpp"
#include "vm/import.hpp"


namespace process {
   class router;
}

namespace vm {

typedef enum {
   PRIORITY_ASC,
   PRIORITY_DESC
} priority_type;

const size_t INIT_PREDICATE_ID(0);
const size_t SET_PRIORITY_PREDICATE_ID(1);
const size_t SETCOLOR_PREDICATE_ID(2);
const size_t SETEDGELABEL_PREDICATE_ID(3);
const size_t WRITE_STRING_PREDICATE_ID(4);
const size_t ADD_PRIORITY_PREDICATE_ID(5);
const size_t SCHEDULE_NEXT_PREDICATE_ID(6);
const size_t SETCOLOR2_PREDICATE_ID(7);

#define VERSION_AT_LEAST(MAJ, MIN) (major_version > (MAJ) || (major_version == (MAJ) && minor_version >= (MIN)))

class program
{
private:

   const std::string filename;
   uint32_t major_version, minor_version;

   std::vector<type*> types;

   std::vector<import*> imported_predicates;
   std::vector<std::string> exported_predicates;

	size_t num_args;
   size_t number_rules;

   std::vector<rule*> rules;

   std::vector<function*> functions;
   
   std::vector<predicate*> predicates;
  
   std::vector<byte_code> code;
   std::vector<code_size_t> code_size;

	code_size_t const_code_size;
	byte_code const_code;
	std::vector<type*> const_types;
   
   std::vector<predicate*> route_predicates;
   
   bool safe;
   bool is_data_file;

   rule *data_rule;

   mutable predicate *init;

	typedef std::vector<runtime::rstring::ptr> string_store;
	
	string_store default_strings;
	
   strat_level priority_strat_level;
   field_type priority_type;
   vm::priority_type priority_order;
   bool priority_static;
   heap_priority initial_priority;

   void print_predicate_code(std::ostream&, predicate*) const;
   
public:

   strat_level MAX_STRAT_LEVEL;

   inline size_t num_types(void) const { return types.size(); }

   inline type* get_type(const size_t i) const { assert(i < types.size()); return types[i]; }

   inline size_t num_rules(void) const { return number_rules; }
	inline size_t num_args_needed(void) const { return num_args; }

   inline std::string get_name(void) const { return filename; }

   inline rule *get_rule(const rule_id id) const {
      assert(id < number_rules);
      return rules[id];
   }

   inline function *get_function(const size_t id) const {
      assert(id < functions.size());
      return functions[id];
   }

   inline field_type get_priority_type(void) const { return priority_type; }
   inline strat_level get_priority_strat_level(void) const { return priority_strat_level; }
	inline bool is_priority_asc(void) const { return priority_order == PRIORITY_ASC; }
	inline bool is_priority_desc(void) const { return priority_order == PRIORITY_DESC; }

   inline heap_priority get_initial_priority(void) const { return initial_priority; }
   inline bool is_static_priority(void) const { return priority_static; }
	
   predicate *get_predicate_by_name(const std::string&) const;
   
   predicate *get_init_predicate(void) const;
   
   predicate *get_edge_predicate(void) const;
   
   void print_bytecode(std::ostream&) const;
   void print_predicates(std::ostream&) const;
   void print_rules(std::ostream&) const;
   void print_program(std::ostream&) const;
   void print_bytecode_by_predicate(std::ostream&, const std::string&) const;

   
   predicate* get_predicate(const predicate_id&) const;
   predicate* get_route_predicate(const size_t&) const;
   
   byte_code get_predicate_bytecode(const predicate_id id) const {
      assert(id < num_predicates());
      return code[id];
   }
	inline byte_code get_const_bytecode(void) const { return const_code; }
	inline type* get_const_type(const const_id& id) const { return const_types[id]; }
   
   size_t num_predicates(void) const { return predicates.size(); }
   size_t num_route_predicates(void) const { return route_predicates.size(); }

	inline runtime::rstring::ptr get_default_string(const size_t i) const
	{
      assert(i < default_strings.size());
		return default_strings[i];
	}
   
   tuple* new_tuple(const predicate_id&) const;
   
   inline bool is_safe(void) const { return safe; }
   inline bool is_data(void) const { return is_data_file; }

   bool add_data_file(vm::program&);

    void add_external_function(external_function_ptr ptr, size_t num_args, type *ret_type, type **arg_type);
    ptr_val get_function_pointer(char *lib_path,char* func_name);
   
   explicit program(const std::string&);
   
   ~program(void);
};

}

#endif
