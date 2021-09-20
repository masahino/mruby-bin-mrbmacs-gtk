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
mrb_mrbmacs_frame_y_or_n(mrb_state *mrb, mrb_value self)
{
  GtkWidget *dialog;
  gint response;
  char *prompt;
  mrb_value ret;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  mrb_get_args(mrb, "z", &prompt);

  dialog = gtk_message_dialog_new(GTK_WINDOW(fdata->mainwin),
    GTK_DIALOG_DESTROY_WITH_PARENT,
    GTK_MESSAGE_QUESTION,
    GTK_BUTTONS_YES_NO,
    "%s",
    prompt);
  gtk_dialog_add_buttons(GTK_DIALOG(dialog),
    "_Cancel",
           GTK_RESPONSE_CANCEL,
           NULL);
  response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_YES) {
    ret = mrb_true_value();
  } else if (response == GTK_RESPONSE_NO) {
    ret = mrb_false_value();
  } else {
    ret = mrb_nil_value();
  }
  gtk_widget_destroy(dialog);
  return ret;
}

static void
mrbmacs_frame_echo_entry_callback(GtkEntry *entry, gpointer dialog)
{
  gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static mrb_value
mrb_mrbmacs_frame_echo_gets(mrb_state *mrb, mrb_value self)
{
  GtkWidget *dialog;
  GtkWidget *entry, *label;
  GtkEntryCompletion *completion;
  GtkListStore *store;
  GtkTreeIter iter;
  gint response;
  mrb_value ret;
  mrb_value block;
  mrb_int argc;
  mrb_value comp_and_len, comp_list;
  char *prompt, *pretext;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  argc = mrb_get_args(mrb, "z|z&", &prompt, &pretext, &block);
  dialog = gtk_dialog_new_with_buttons(prompt, GTK_WINDOW(fdata->mainwin),
    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
    "_Cancel", GTK_RESPONSE_CANCEL, "_OK", GTK_RESPONSE_OK, NULL);
  label = gtk_label_new(prompt);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),label);
  entry = gtk_entry_new();
  if (argc > 1) {
    gtk_entry_set_text(GTK_ENTRY(entry), pretext);
  }
  completion = gtk_entry_completion_new();
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));
  gtk_entry_completion_set_text_column(completion, 0);
  if (mrb_type(block) == MRB_TT_PROC) {
    comp_and_len = mrb_yield(mrb, block, mrb_str_new_cstr(mrb, ""));
    comp_list = mrb_funcall(mrb, mrb_ary_ref(mrb, comp_and_len, 0), "split", 1, mrb_str_new_cstr(mrb, " "));
    for (int i = 0; i < RARRAY_LEN(comp_list); i++) {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, mrb_str_to_cstr(mrb, mrb_ary_ref(mrb, comp_list, i)), -1);
    }
  }
  gtk_entry_set_completion(GTK_ENTRY(entry), completion);
  g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(mrbmacs_frame_echo_entry_callback),
      (gpointer)dialog);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);

  gtk_widget_show_all(dialog);
  response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_OK) {
    ret = mrb_str_new_cstr(mrb, gtk_entry_get_text(GTK_ENTRY(entry)));
  } else {
    ret = mrb_nil_value();
  }
  gtk_widget_destroy(dialog);
  return ret;
}

void
mrb_mrbmacs_gtk_frame_dialog_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "y_or_n",
    mrb_mrbmacs_frame_y_or_n, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "echo_gets",
    mrb_mrbmacs_frame_echo_gets, MRB_ARGS_ARG(1,2));

}
