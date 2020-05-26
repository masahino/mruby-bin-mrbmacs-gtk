#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"

#include <locale.h>
#include <gtk/gtk.h>
#include <Scintilla.h>

//mrb_state *mrb;

int
main(int argc, char **argv)
{
  mrb_value mrbmacs;
  mrb_value arg_array;
  mrb_state *mrb;
  struct RClass *mrbmacs_class;
  int i;
  
  mrb = mrb_open();

  setlocale(LC_CTYPE, "");

  gtk_init(&argc, &argv);
  if (mrb == NULL) {
    fputs("Invalid mrb_state, exiting scimre\n", stderr);
    return EXIT_FAILURE;
  }

  arg_array = mrb_ary_new(mrb);
  for (i = 1; i < argc; i++) {
    mrb_ary_push(mrb, arg_array, mrb_str_new_cstr(mrb, argv[i]));
  }
  mrbmacs_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "Mrbmacs"), "ApplicationGtk");
  mrbmacs = mrb_funcall(mrb, mrb_obj_value(mrbmacs_class), "new", 1, arg_array);
  mrb_funcall(mrb, mrbmacs, "run", 0);
  return 0;
}
