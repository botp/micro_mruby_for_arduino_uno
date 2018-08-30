#include "vm_config.h"
#include <string.h>

#include "value.h"
#include "micro_vm.h"
#include "class.h"
#include "alloc.h"
#include "c_object.h"
#include "c_string.h"
#include "global.h"
#include "symbol.h"
#include "console.h"
#include "debug.h"

/* Class Tree */
mrb_class *mrbc_class_object;
mrb_class *mrbc_class_proc;
mrb_class *mrbc_class_false;
mrb_class *mrbc_class_true;
mrb_class *mrbc_class_nil;
mrb_class *mrbc_class_array;
mrb_class *mrbc_class_fixnum;
mrb_class *mrbc_class_symbol;
mrb_class *mrbc_class_string;
mrb_class *mrbc_class_range;
//mrb_class *mrbc_class_hash;

/* Common */

void c_ineffect(mrb_mvm *vm, mrb_value v[], int argc)
{
  // nothing to do.
}

/* Util Function */
mrb_class *find_class_by_object(struct VM *vm, mrb_object *obj)
{
  mrb_class *cls;

  switch( obj->tt ) {
  case MRB_TT_TRUE:	cls = mrbc_class_true;		break;
  case MRB_TT_FALSE:	cls = mrbc_class_false; 	break;
  case MRB_TT_NIL:	cls = mrbc_class_nil;		break;
  case MRB_TT_FIXNUM:	cls = mrbc_class_fixnum;	break;
  //case MRB_TT_FLOAT:	cls = mrbc_class_float; 	break;
  case MRB_TT_SYMBOL:	cls = mrbc_class_symbol;	break;

  case MRB_TT_OBJECT:	cls = obj->instance->cls;       break;
  case MRB_TT_CLASS:    cls = obj->cls;                 break;
  case MRB_TT_PROC:	cls = mrbc_class_proc;		break;
  case MRB_TT_ARRAY:	cls = mrbc_class_array; 	break;
  case MRB_TT_STRING:	cls = mrbc_class_string;	break;
  case MRB_TT_RANGE:	cls = mrbc_class_range; 	break;
  //TODO
  //case MRB_TT_HASH:	cls = mrbc_class_hash;		break;

  default:		cls = mrbc_class_object;	break;
  }

  return cls;
}

mrb_proc *find_method(mrb_mvm *vm, mrb_value recv, mrb_sym sym_id)
{
  mrb_class *cls = find_class_by_object(vm, &recv);

  while( cls != 0 ) {
    mrb_proc *proc = cls->procs;
    while( proc != 0 ) {
      if( proc->sym_id == sym_id ) {
        return proc;
      }
      proc = proc->next;
    }
    cls = cls->super;
  }
  return 0;
}

mrb_class * mrbc_define_class(mrb_mvm* vm, const char *name, mrb_class *super)
{
  DEBUG_FPRINTLN("define class");
  mrb_class *cls;
  mrb_sym sym_id = str_to_symid(name);
  mrb_object obj = const_object_get(sym_id);

  // create a new class?
  if( obj.tt == MRB_TT_NIL ) {
    cls = (mrb_class*)mrbc_alloc( 0, sizeof(mrb_class) );
    if( !cls ) return cls;	// ENOMEM

    cls->sym_id = sym_id;
    cls->super = super;
    cls->procs = 0;

    // register to global constant.
    mrb_value v = {.tt = MRB_TT_CLASS};
    v.cls = cls;
    const_object_add(sym_id, &v);

    return cls;
  }

  // already?
  if( obj.tt == MRB_TT_CLASS ) {
    return obj.cls;
  }
}

void mrbc_define_method(mrb_mvm *vm, mrb_class *cls, const char *name, mrb_func_t cfunc)
{
  DEBUG_FPRINTLN("define method");
  mrb_proc *rproc = mrbc_rproc_alloc(vm, name);
  rproc->c_func = 1;  // c-func
  rproc->next = cls->procs;
  cls->procs = rproc;
  rproc->func = cfunc;
}

void mrbc_init_class(void)
{
  mrbc_init_class_object(0);
  mrbc_init_class_string(0);
  //TODO
#if 0
  mrbc_init_class_nil(0);
  mrbc_init_class_proc(0);
  mrbc_init_class_false(0);
  mrbc_init_class_true(0);

  mrbc_init_class_fixnum(0);
  mrbc_init_class_symbol(0);
  mrbc_init_class_array(0);
  mrbc_init_class_range(0);
  //mrbc_init_class_hash(0);
#endif
}

