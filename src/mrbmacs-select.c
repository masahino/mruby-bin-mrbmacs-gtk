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
static void mrbmacs_frame_select_item_activated(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *col, gpointer dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static mrb_value
mrb_mrbmacs_frame_select_item(mrb_state *mrb, mrb_value self)
{
  char *title_str, *default_item_name;
  gchar *row_string;
  mrb_value item_name;
  mrb_value item_list;
  GtkWidget *dialog;
  GtkWidget *listview;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  mrb_get_args(mrb, "zzA", &title_str, &default_item_name, &item_list);
  dialog = gtk_dialog_new_with_buttons(title_str,
    GTK_WINDOW(fdata->mainwin),
    (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
    "_OK", GTK_RESPONSE_OK,
    "_Cancel", GTK_RESPONSE_CANCEL,
    NULL);
  listview = gtk_tree_view_new();
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(listview),
    GTK_TREE_MODEL(store));
  for (int i = 0; i < RARRAY_LEN(item_list); i++) {
    mrb_value b = mrb_ary_ref(mrb, item_list, i);;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
      0, mrb_str_to_cstr(mrb, b), -1);
  }
  gtk_tree_view_append_column(GTK_TREE_VIEW(listview),
    gtk_tree_view_column_new_with_attributes(title_str,
      gtk_cell_renderer_text_new (), "text", 0, NULL));
  g_signal_connect(G_OBJECT(listview), "row-activated", G_CALLBACK(mrbmacs_frame_select_item_activated), dialog);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), listview);
  gtk_widget_show_all(dialog);

  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(result) {
    case GTK_RESPONSE_OK:
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, 0, &row_string, -1);
    item_name = mrb_str_new_cstr(mrb, row_string);
    break;
    case GTK_RESPONSE_CANCEL:
    default:
    item_name = mrb_str_new_cstr(mrb, default_item_name);
  }
  gtk_widget_destroy(dialog);
  return item_name;
}

static mrb_value
mrb_mrbmacs_frame_select_file(mrb_state *mrb, mrb_value self)
{
  GtkWidget *dialog;
  mrb_bool is_open;
  char *title;
  char *path;
  char *filename = NULL;
  mrb_value default_name;
  char accept_button_name[6];
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  gint ret;
  mrb_value ret_value;
  mrb_get_args(mrb, "zzbo", &title, &path, &is_open, &default_name);
  if (is_open == TRUE) {
    strcpy(accept_button_name, "_Open");
  } else {
    strcpy(accept_button_name, "_Save");
  }
  dialog = gtk_file_chooser_dialog_new(title, GTK_WINDOW(fdata->mainwin),
    GTK_FILE_CHOOSER_ACTION_SAVE,
    "_Cancel", GTK_RESPONSE_CANCEL,
    accept_button_name, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
  if (mrb_nil_p(default_name)) {
  } else {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), mrb_string_cstr(mrb, default_name));
  }
  gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog), TRUE);

  ret = gtk_dialog_run (GTK_DIALOG(dialog));

  if (ret == GTK_RESPONSE_OK) {
    ret_value = mrb_str_new_cstr(mrb, path);
    ret_value = mrb_str_cat_lit(mrb, ret_value, "/Untitled");
  } else if (ret == GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    ret_value = mrb_str_new_cstr(mrb, filename);
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

  mrb_define_method(mrb, frame, "select_file",
    mrb_mrbmacs_frame_select_file, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, frame, "select_item",
    mrb_mrbmacs_frame_select_item, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, frame, "select_font",
    mrb_mrbmacs_frame_select_font, MRB_ARGS_NONE());
}
