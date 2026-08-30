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
#define PLAT_GTK 1
#include <ScintillaWidget.h>

#include "mrbmacs-frame.h"

static mrb_value
mrb_mrbmacs_frame_select_font(mrb_state *mrb, mrb_value self)
{
  GtkWidget *dialog;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  gint ret;
  mrb_value ret_value;

  dialog = gtk_font_chooser_dialog_new("select font", GTK_WINDOW(fdata->mainwin));
  gtk_widget_show(dialog);
  ret = gtk_dialog_run(GTK_DIALOG(dialog));
  if (ret == GTK_RESPONSE_OK) {
    ret_value = mrb_ary_new(mrb);
    mrb_ary_push(mrb, ret_value,
      mrb_str_new_cstr(mrb,
        pango_font_description_get_family(gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(dialog)))));
    mrb_ary_push(mrb, ret_value,
      mrb_fixnum_value(gtk_font_chooser_get_font_size(GTK_FONT_CHOOSER(dialog))/PANGO_SCALE));
  } else {
    ret_value = mrb_nil_value();
  }
  gtk_widget_destroy(dialog);
  return ret_value;
}

void
mrb_mrbmacs_gtk_frame_select_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame;

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "select_font",
    mrb_mrbmacs_frame_select_font, MRB_ARGS_NONE());
}
