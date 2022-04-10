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

mrb_value
add_new_edit_win_with_tab(mrb_state *mrb, mrb_value frame, mrb_value buffer, GtkWidget *notebook)
{
  mrb_value view, edit_win;
  int i;

  /* edit window */
  /* initial buffer "*scratch*" */
  edit_win = mrb_funcall(mrb,
    mrb_obj_value(mrb_class_get_under(mrb, mrb_module_get(mrb, "Mrbmacs"), "EditWindowGtk")),
    "new", 6, frame, buffer, mrb_fixnum_value(0), mrb_fixnum_value(0),
    mrb_fixnum_value(80+6), mrb_fixnum_value(40));
  view = mrb_funcall(mrb, edit_win, "sci", 0);
  gtk_widget_show((GtkWidget *)DATA_PTR(view));
  i = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), (GtkWidget *)DATA_PTR(view),
    NULL);
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), i);

  return edit_win;
}

static mrb_value
mrb_mrbmacs_frame_add_new_tab(mrb_state *mrb, mrb_value self)
{
  mrb_value buffer, edit_win, edit_win_list;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  mrb_get_args(mrb, "o", &buffer);
  /* edit window */
  /* initial buffer "*scratch*" */
  edit_win = add_new_edit_win_with_tab(mrb, self, buffer, fdata->notebook);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@view_win"), mrb_funcall(mrb, edit_win, "sci", 0));
  edit_win_list = mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@edit_win_list"));
  mrb_funcall(mrb, edit_win_list, "push", 1, edit_win);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@edit_win"), edit_win);
  return mrb_nil_value();
}

static mrb_value
mrb_mrbmacs_frame_sync_tab(mrb_state *mrb, mrb_value self)
{
  int i;
  gint max_n;
  char *selected_buffer_name;
  const gchar *label_text;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  mrb_get_args(mrb, "z", &selected_buffer_name);
  label_text = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(fdata->notebook),
    gtk_notebook_get_nth_page(GTK_NOTEBOOK(fdata->notebook),
      gtk_notebook_get_current_page(GTK_NOTEBOOK(fdata->notebook))));
  if (strcmp(selected_buffer_name, label_text) == 0) {
    return mrb_nil_value();
  }
  max_n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(fdata->notebook));
  for (i = 0; i < max_n; i++) {
    label_text = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(fdata->notebook),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(fdata->notebook), i));
    if (label_text == NULL) {
      continue;
    }
    if (strcmp(selected_buffer_name, label_text) == 0) {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(fdata->notebook), i);
      break;
    }
  }

  return mrb_nil_value();
}


static mrb_value
mrb_mrbmacs_frame_set_buffer_name(mrb_state *mrb, mrb_value self)
{
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  char *buffer_name;

  mrb_get_args(mrb, "z", &buffer_name);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(fdata->notebook),
    gtk_notebook_get_nth_page(GTK_NOTEBOOK(fdata->notebook),
      gtk_notebook_get_current_page(GTK_NOTEBOOK(fdata->notebook))),
    buffer_name);
  return self;
}

void
mrb_mrbmacs_gtk_frame_tab_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "add_new_tab",
    mrb_mrbmacs_frame_add_new_tab, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "sync_tab",
    mrb_mrbmacs_frame_sync_tab, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "set_buffer_name",
    mrb_mrbmacs_frame_set_buffer_name, MRB_ARGS_REQ(1));

}
