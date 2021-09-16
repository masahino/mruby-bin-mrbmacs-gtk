#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>
#ifdef MAC_INTEGRATION
#include <gtkosxapplication.h>
#endif

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/variable.h"
#include "mruby/array.h"
#include "mruby/string.h"

#include <Scintilla.h>
#include <SciLexer.h>
#define PLAT_GTK 1
#include <ScintillaWidget.h>

#include "mrbmacs-frame.h"
#include "mrbmacs-cb.h"

int
edit_win_get_height(mrb_state *mrb, mrb_value edit_win)
{
  int font_height;
  mrb_value view;
  mrb_int height;

  height = mrb_fixnum(mrb_iv_get(mrb, edit_win, mrb_intern_cstr(mrb, "@height")));
  view = mrb_iv_get(mrb, edit_win, mrb_intern_cstr(mrb, "@sci"));
  font_height = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_height", 1, mrb_fixnum_value(1)));

  return font_height*height;
}

int edit_win_get_width(mrb_state *mrb, mrb_value edit_win)
{
  int font_width;
  mrb_value view;
  mrb_int width;

  width = mrb_fixnum(mrb_iv_get(mrb, edit_win, mrb_intern_cstr(mrb, "@width")));
  view = mrb_iv_get(mrb, edit_win, mrb_intern_cstr(mrb, "@sci"));
  font_width = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_width", 2,
      mrb_fixnum_value(STYLE_DEFAULT), mrb_str_new_lit(mrb, "A")));

  return font_width*(width+6);
}
mrb_value
mrb_mrbmacs_window_set_callback(mrb_state *mrb, mrb_value self)
{
  mrb_value view;

  view = mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@sci"));
  g_signal_connect(G_OBJECT((GtkWidget *)DATA_PTR(view)),
    "sci-notify", G_CALLBACK(mrbmacs_sci_notify), NULL);

  return self;
}

mrb_value
mrb_mrbmacs_window_compute_area(mrb_state *mrb, mrb_value self)
{
  int font_width, font_height;
  mrb_value view;
  mrb_int width, height;
  width = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@width")));
  height = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@height")));

  view = mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@sci"));

  font_width = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_width", 2,
      mrb_fixnum_value(STYLE_DEFAULT), mrb_str_new_lit(mrb, "A")));
  font_height = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_height", 1, mrb_fixnum_value(1)));
//  gtk_widget_set_size_request((GtkWidget *)DATA_PTR(view), font_width*(width + 6), font_height*height);

  return view;
}

void
mrb_mrbmacs_gtk_window_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *edit_win; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");

  edit_win = mrb_define_class_under(mrb, mrbmacs_module, "EditWindowGtk",
    mrb_class_get_under(mrb, mrbmacs_module, "EditWindow"));

  mrb_define_method(mrb, edit_win, "set_callback",
    mrb_mrbmacs_window_set_callback, MRB_ARGS_NONE());
  mrb_define_method(mrb, edit_win, "compute_area",
    mrb_mrbmacs_window_compute_area, MRB_ARGS_NONE());
}