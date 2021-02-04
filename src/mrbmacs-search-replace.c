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

static mrb_value
mrb_mrbmacs_frame_search_entry_get_text(mrb_state *mrb, mrb_value self)
{
  GtkWidget *isearch_entry;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  isearch_entry = fdata->search_entry;
  gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(fdata->search_bar), TRUE);
#if GTK_CHECK_VERSION(3, 16, 0)
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(isearch_entry));
#else
  gtk_widget_grab_focus(GTK_WIDGET(isearch_entry));
#endif /* GTK_CHECK_VERSION(3, 16, 0) */

  return mrb_str_new_cstr(mrb,
    gtk_entry_get_text(GTK_ENTRY(isearch_entry)));
}

static mrb_value
mrb_mrbmacs_frame_query_replace(mrb_state *mrb, mrb_value self)
{
  GtkWidget *isearch_entry;
  GtkWidget *replace_entry;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  isearch_entry = fdata->search_entry;
  replace_entry = fdata->replace_entry;
  gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(fdata->search_bar), TRUE);
  gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(fdata->replace_bar), TRUE);
#if GTK_CHECK_VERSION(3, 16, 0)
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(isearch_entry));
#else
  gtk_widget_grab_focus(GTK_WIDGET(isearch_entry));
#endif /* GTK_CHECK_VERSION(3, 16, 0) */

  return mrb_nil_value();
}

void
mrb_mrbmacs_gtk_frame_search_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "search_entry_get_text",
    mrb_mrbmacs_frame_search_entry_get_text, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "query_replace",
    mrb_mrbmacs_frame_query_replace, MRB_ARGS_NONE());
}
