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
  gtk_widget_set_size_request((GtkWidget *)DATA_PTR(view), font_width*(80+6), font_height*40);

  mrb_funcall(mrb, view, "sci_set_caret_fore", 1, mrb_fixnum_value(0xffffff));
  mrb_funcall(mrb, view, "sci_set_caret_style", 1, mrb_fixnum_value(2));
  //  mrb_funcall(mrb, view, "sci_set_mod_event_mask", 1, mrb_fixnum_value(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT));
  mrb_funcall(mrb, view, "sci_set_mouse_dwell_time", 1, mrb_fixnum_value(1000));
  return view;
}
