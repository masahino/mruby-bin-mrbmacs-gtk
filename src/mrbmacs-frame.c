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
#include "mrbmacs-window.h"
#include "mrbmacs-cb.h"

static const struct mrb_data_type mrb_mrbmacs_frame_data_type = {
  "mrb_mrbmacs_frame_data", mrb_free,
};

static mrb_value
scintilla_echo_window_new(mrb_state *mrb, mrb_value self)
{
  struct RClass *scintilla_gtk_class;
  int font_width, font_height;
  mrb_value echo;

  scintilla_gtk_class = mrb_class_get_under(
    mrb,
    mrb_module_get(mrb, "Scintilla"), "ScintillaGtk");

  echo = mrb_funcall(mrb, mrb_obj_value(scintilla_gtk_class), "new", 0);

  mrb_funcall(mrb, echo, "sci_style_set_font",
    2, mrb_fixnum_value(STYLE_DEFAULT),
    mrb_str_new_lit(mrb, "Monospace"));
  mrb_funcall(mrb, echo, "sci_style_set_size",
    2, mrb_fixnum_value(STYLE_DEFAULT), mrb_fixnum_value(14));

  font_width = mrb_int(mrb, mrb_funcall(mrb, echo, "sci_text_width", 2,
      mrb_fixnum_value(STYLE_DEFAULT), mrb_str_new_lit(mrb, "A")));
  font_height = mrb_int(mrb, mrb_funcall(mrb, echo, "sci_text_height", 1, mrb_fixnum_value(1)));

  gtk_widget_set_size_request((GtkWidget*)DATA_PTR(echo), font_width*80, font_height);

  mrb_funcall(mrb, echo, "sci_set_hscroll_bar", 1, mrb_false_value());
  mrb_funcall(mrb, echo, "sci_clear_cmd_key", 1, mrb_fixnum_value(SCK_RETURN));

  return echo;
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

static void mrbmacs_frame_select_buffer_activated(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *col, gpointer dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static mrb_value
mrb_mrbmacs_frame_select_buffer(mrb_state *mrb, mrb_value self)
{
  char *default_buffer_name;
  gchar *row_string;
  mrb_value buffer_name;
  mrb_value buffer_list;
  GtkWidget *dialog;
  GtkWidget *listview;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  mrb_get_args(mrb, "zA", &default_buffer_name, &buffer_list);
  dialog = gtk_dialog_new_with_buttons("select buffer",
    GTK_WINDOW(fdata->mainwin),
    (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
    "_OK", GTK_RESPONSE_OK,
    "_Cancel", GTK_RESPONSE_CANCEL,
    NULL);
  listview = gtk_tree_view_new();
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(listview),
    GTK_TREE_MODEL(store));
  for (int i = 0; i < RARRAY_LEN(buffer_list); i++) {
    mrb_value b = mrb_ary_ref(mrb, buffer_list, i);;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
      0, mrb_str_to_cstr(mrb, b), -1);
  }
  gtk_tree_view_append_column(GTK_TREE_VIEW(listview),
    gtk_tree_view_column_new_with_attributes("buffer",
      gtk_cell_renderer_text_new (), "text", 0, NULL));
  g_signal_connect(G_OBJECT(listview), "row-activated", G_CALLBACK(mrbmacs_frame_select_buffer_activated), dialog);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), listview);
  gtk_widget_show_all(dialog);

  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(result) {
    case GTK_RESPONSE_OK:
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, 0, &row_string, -1);
    buffer_name = mrb_str_new_cstr(mrb, row_string);
    break;
    case GTK_RESPONSE_CANCEL:
    default:
    buffer_name = mrb_str_new_cstr(mrb, default_buffer_name);
  }
  gtk_widget_destroy(dialog);
  return buffer_name;
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

static mrb_value
mrb_mrbmacs_frame_search_entry_get_text(mrb_state *mrb, mrb_value self)
{
  GtkWidget *isearch_entry;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  isearch_entry = fdata->search_entry;
#if GTK_CHECK_VERSION(3, 16, 0)
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(isearch_entry));
#else
  gtk_widget_grab_focus(GTK_WIDGET(isearch_entry));
#endif /* GTK_CHECK_VERSION(3, 16, 0) */

  return mrb_str_new_cstr(mrb,
    gtk_entry_get_text(GTK_ENTRY(isearch_entry)));
}

static mrb_value
mrb_mrbmacs_frame_set_mode_text(mrb_state *mrb, mrb_value self)
{
  GtkWidget *mode_win;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  char *mode_str;

  mrb_get_args(mrb, "z", &mode_str);
  mode_win = fdata->mode_win;
  gtk_label_set_text(GTK_LABEL(mode_win), mode_str);
  return self;
}

static void
mrbmacs_frame_new_file_callback(GtkWidget *new_button, gpointer dialog)
{
  gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static mrb_value
mrb_mrbmacs_frame_select_file(mrb_state *mrb, mrb_value self)
{
  GtkWidget *dialog, *new_button;
  char *title;
  char *path;
  char *filename = NULL;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  gint ret;
  mrb_value ret_value;
  
  mrb_get_args(mrb, "zz", &title, &path);
  dialog = gtk_file_chooser_dialog_new(title, GTK_WINDOW(fdata->mainwin),
    GTK_FILE_CHOOSER_ACTION_OPEN,
    "_Cancel", GTK_RESPONSE_CANCEL,
    "_Open", GTK_RESPONSE_ACCEPT, NULL);
  new_button = gtk_button_new_with_label("New");
  g_signal_connect(G_OBJECT(new_button), "clicked", G_CALLBACK(mrbmacs_frame_new_file_callback),
    (gpointer)dialog);
  gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog), new_button);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);

  ret = gtk_dialog_run (GTK_DIALOG(dialog));

  if (ret == GTK_RESPONSE_OK) {
    ret_value = mrb_str_new_cstr(mrb, path);
    ret_value = mrb_str_cat_lit(mrb, ret_value, "/Untitled");
  }
  if (ret == GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    ret_value = mrb_str_new_cstr(mrb, filename);
  }
  gtk_widget_destroy(dialog);
  return ret_value;
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

static mrb_value
add_new_edit_win_with_tab(mrb_state *mrb, mrb_value frame, mrb_value buffer, GtkWidget *notebook)
{
  mrb_value view, edit_win;
  mrb_value buffer_name;
  int i;

  buffer_name = mrb_funcall(mrb, buffer, "name", 0);

  /* edit window */
  /* initial buffer "*scratch*" */
  edit_win = mrb_funcall(mrb,
    mrb_obj_value(mrb_class_get_under(mrb, mrb_module_get(mrb, "Mrbmacs"), "EditWindowGtk")),
    "new", 6, frame, buffer, mrb_fixnum_value(0), mrb_fixnum_value(0),
    mrb_fixnum_value(80+6), mrb_fixnum_value(40));
  view = mrb_funcall(mrb, edit_win, "sci", 0);
  gtk_widget_show((GtkWidget *)DATA_PTR(view));
  i = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), (GtkWidget *)DATA_PTR(view),
    gtk_label_new(mrb_str_to_cstr(mrb, buffer_name)));

  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), i);

  return edit_win;
}

static GtkWidget*
create_search_box(struct mrb_mrbmacs_frame_data *fdata)
{
  GtkWidget *grid;
  GtkWidget *search_entry, *replace_entry, *hbox1, *hbox2;
  GtkWidget *find_next_button, *find_prev_button;
  GtkWidget *replace_next_button, *replace_prev_button;
 // search box
  grid = gtk_grid_new();
  hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
//  gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 2);

  search_entry = gtk_search_entry_new();
  gtk_widget_set_size_request(GTK_WIDGET(search_entry), 300, -1);
  gtk_grid_attach(GTK_GRID(grid), search_entry, 0, 0, 1, 1);
  find_next_button = gtk_button_new_with_label("Find Next");
  gtk_grid_attach_next_to(GTK_GRID(grid), find_next_button, search_entry, GTK_POS_RIGHT, 1, 1);
  find_prev_button = gtk_button_new_with_label("Find Prev");
  gtk_grid_attach_next_to(GTK_GRID(grid), find_prev_button, find_next_button, GTK_POS_RIGHT, 1, 1);
  fdata->search_entry = search_entry;
  fdata->find_next_button = find_next_button;
  fdata->find_prev_button = find_prev_button;

  // replace box
  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  replace_entry = gtk_search_entry_new();
  gtk_widget_set_size_request(GTK_WIDGET(replace_entry), 240, -1);
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(replace_entry), GTK_ENTRY_ICON_PRIMARY,"edit-find-replace");
  gtk_grid_attach_next_to(GTK_GRID(grid), replace_entry, search_entry, GTK_POS_BOTTOM, 1, 1);
  replace_next_button = gtk_button_new_with_label("Replace Next");
  gtk_grid_attach_next_to(GTK_GRID(grid), replace_next_button, replace_entry, GTK_POS_RIGHT, 1, 1);
  replace_prev_button = gtk_button_new_with_label("Replace Prev");
  gtk_grid_attach_next_to(GTK_GRID(grid), replace_prev_button, replace_next_button, GTK_POS_RIGHT, 1, 1);
  fdata->replace_entry = replace_entry;
  fdata->replace_next_button = replace_next_button;
  fdata->replace_prev_button = replace_prev_button;

  return grid;
}

static mrb_value
mrb_mrbmacs_frame_init(mrb_state *mrb, mrb_value self)
{
  GtkWidget *mainwin, *vbox, *mode;
  GtkWidget *notebook;
  struct RClass *mrbmacs_module;
//  int font_width, font_height;
  mrb_value view, echo;
  mrb_value buffer, edit_win;
  mrb_value edit_win_list;

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");

  mrb_get_args(mrb, "o", &buffer);

  struct mrb_mrbmacs_frame_data *fdata =
  (struct mrb_mrbmacs_frame_data *)mrb_malloc(mrb, sizeof(struct mrb_mrbmacs_frame_data));

  mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add(GTK_CONTAINER(mainwin), vbox);

  DATA_TYPE(self) = &mrb_mrbmacs_frame_data_type;
  DATA_PTR(self) = NULL;
  fdata->mainwin = mainwin;

  notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, FALSE, 0);
//  gtk_widget_show(GTK_WIDGET(notebook));
  fdata->notebook = notebook;

  /* edit window */
  /* initial buffer "*scratch*" */
  edit_win = add_new_edit_win_with_tab(mrb, self, buffer, notebook);
  view = mrb_funcall(mrb, edit_win, "sci", 0);

  edit_win_list = mrb_ary_new(mrb);
  mrb_ary_push(mrb, edit_win_list, edit_win);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@edit_win_list"), edit_win_list);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@edit_win"), edit_win);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@view_win"), view);

  mode = gtk_label_new("");
  gtk_label_set_justify(GTK_LABEL(mode), GTK_JUSTIFY_LEFT);
  gtk_widget_set_halign(mode, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), mode, TRUE, TRUE, 0);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@mode_win"), mrb_cptr_value(mrb, mode));
  fdata->mode_win = mode;

  gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), TRUE, TRUE, 0);

  echo = scintilla_echo_window_new(mrb, self);

  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@echo_win"), echo);

  // search & replace
  gtk_box_pack_start(GTK_BOX(vbox),   create_search_box(fdata), FALSE, FALSE, 2);

  DATA_PTR(self) = fdata;

//  mrb_funcall(mrb, self, "set_style_gtk", 0);
  mrb_funcall(mrb, view, "sci_set_focus", 1, mrb_true_value());
  mrb_funcall(mrb, echo, "sci_set_focus", 1, mrb_false_value());
  //set_default_style

  gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), gtk_statusbar_new(), TRUE, TRUE, 0);
  gtk_widget_show_all(mainwin);
//gtk_widget_hide(GTK_WIDGET(grid));
  gtk_widget_grab_focus((GtkWidget *)DATA_PTR(view));

  return self;
}

static mrb_value
mrb_mrbmacs_frame_add_new_tab(mrb_state *mrb, mrb_value self)
{
  mrb_value buffer, edit_win, buffer_name, edit_win_list;
  mrb_value view;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);

  mrb_get_args(mrb, "o", &buffer);
  buffer_name = mrb_funcall(mrb, buffer, "name", 0);

  /* edit window */
  /* initial buffer "*scratch*" */
  edit_win = add_new_edit_win_with_tab(mrb, self, buffer, fdata->notebook);
  view = mrb_funcall(mrb, edit_win, "sci", 0);

  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@view_win"), mrb_funcall(mrb, edit_win, "sci", 0));

  edit_win_list = mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@edit_win_list"));
  mrb_funcall(mrb, edit_win_list, "push", 1, edit_win);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@edit_win"), edit_win);

  return mrb_nil_value();
}

void
mrb_mrbmacs_gtk_frame_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "frame_gtk_init",
    mrb_mrbmacs_frame_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "select_file",
    mrb_mrbmacs_frame_select_file, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, frame, "set_mode_text",
    mrb_mrbmacs_frame_set_mode_text, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "search_entry_get_text",
    mrb_mrbmacs_frame_search_entry_get_text, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "set_buffer_name",
    mrb_mrbmacs_frame_set_buffer_name, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "select_buffer",
    mrb_mrbmacs_frame_select_buffer, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, frame, "echo_gets",
    mrb_mrbmacs_frame_echo_gets, MRB_ARGS_ARG(1,2));
  mrb_define_method(mrb, frame, "add_new_tab",
    mrb_mrbmacs_frame_add_new_tab, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "sync_tab",
    mrb_mrbmacs_frame_sync_tab, MRB_ARGS_REQ(1));

  mrb_mrbmacs_gtk_window_init(mrb);
}
