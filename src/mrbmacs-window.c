#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>

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

mrb_value
scintilla_view_window_new(mrb_state *mrb, mrb_value self)
{
  struct RClass *scintilla_gtk_class;
  int font_width, font_height;
  mrb_value view;
  mrb_int width, height;
  width = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@width")));
  height = mrb_fixnum(mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@height")));
  scintilla_gtk_class = mrb_class_get_under(
    mrb,
    mrb_module_get(mrb, "Scintilla"), "ScintillaGtk");

  view = mrb_funcall(mrb, mrb_obj_value(scintilla_gtk_class), "new", 0);

  mrb_funcall(mrb, view, "sci_style_set_font",
    2, mrb_fixnum_value(STYLE_DEFAULT),
    mrb_str_new_lit(mrb, "Monospace"));
  mrb_funcall(mrb, view, "sci_style_set_size",
    2, mrb_fixnum_value(STYLE_DEFAULT), mrb_fixnum_value(14));

  font_width = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_width", 2,
      mrb_fixnum_value(STYLE_DEFAULT), mrb_str_new_lit(mrb, "A")));
  font_height = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_height", 1, mrb_fixnum_value(1)));
  gtk_widget_set_size_request((GtkWidget *)DATA_PTR(view), font_width*(width + 6), font_height*height);

  mrb_funcall(mrb, view, "sci_set_caret_fore", 1, mrb_fixnum_value(0xffffff));
  mrb_funcall(mrb, view, "sci_set_caret_style", 1, mrb_fixnum_value(2));
  //  mrb_funcall(mrb, view, "sci_set_mod_event_mask", 1, mrb_fixnum_value(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT));
  mrb_funcall(mrb, view, "sci_set_mouse_dwell_time", 1, mrb_fixnum_value(1000));
  return view;
}

void
mrb_mrbmacs_gtk_window_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *edit_win; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
//  edit_win = mrb_get_class_under(mrb, mrbmacs_module, "EditWindowGtk");
  edit_win = mrb_define_class_under(mrb, mrbmacs_module, "EditWindowGtk",
    mrb_class_get_under(mrb, mrbmacs_module, "EditWindow"));

  mrb_define_method(mrb, edit_win, "new_view_win_gtk",
    scintilla_view_window_new, MRB_ARGS_NONE());
}