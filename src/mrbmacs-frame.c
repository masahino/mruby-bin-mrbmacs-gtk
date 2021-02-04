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
#include "mrbmacs-search-replace.h"
#include "mrbmacs-select.h"
#include "mrbmacs-tab.h"
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


static GtkWidget*
create_search_bar(struct mrb_mrbmacs_frame_data *fdata, gboolean replace)
{
  GtkWidget *search_bar;
  GtkWidget *entry;
  GtkWidget *next_button, *prev_button;
  GtkWidget *box;

  entry = gtk_search_entry_new();
  if (replace == TRUE) {
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY,"edit-find-replace");
  }

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_halign(box, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);
  search_bar = gtk_search_bar_new();
  gtk_container_set_border_width(GTK_CONTAINER(search_bar), 0);
  gtk_search_bar_connect_entry(GTK_SEARCH_BAR(search_bar), GTK_ENTRY(entry));
  gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(search_bar), FALSE);
  gtk_search_bar_set_show_close_button(GTK_SEARCH_BAR(search_bar), TRUE);

  next_button = gtk_button_new_from_icon_name("go-down-symbolic", GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(GTK_BOX(box), next_button, FALSE, FALSE, 0);
  prev_button = gtk_button_new_from_icon_name("go-up-symbolic", GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(GTK_BOX(box), prev_button, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(search_bar), box);

  if (replace == FALSE) {
    fdata->search_bar = search_bar;
    fdata->search_entry = entry;
    fdata->find_next_button = next_button;
    fdata->find_prev_button = prev_button;
  } else {
    fdata->replace_bar = search_bar;
    fdata->replace_entry = entry;
    fdata->replace_next_button = next_button;
    fdata->replace_prev_button = prev_button;
  }
  return search_bar;
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
//  gtk_widget_set_size_request(mainwin, -1, -1);
  gtk_window_set_default_size(GTK_WINDOW(mainwin), 738, 764);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
  gtk_container_add(GTK_CONTAINER(mainwin), vbox);

  DATA_TYPE(self) = &mrb_mrbmacs_frame_data_type;
  DATA_PTR(self) = NULL;
  fdata->mainwin = mainwin;

  notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

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
  gtk_box_pack_start(GTK_BOX(vbox), mode, FALSE, FALSE, 0);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@mode_win"), mrb_cptr_value(mrb, mode));
  fdata->mode_win = mode;

//  gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, TRUE, 0);

  echo = scintilla_echo_window_new(mrb, self);

  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@echo_win"), echo);

  // search & replace
  gtk_box_pack_start(GTK_BOX(vbox), create_search_bar(fdata, FALSE), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), create_search_bar(fdata, TRUE), FALSE, FALSE, 0);
//  gtk_box_pack_start(GTK_BOX(vbox),   create_search_box(fdata), FALSE, FALSE, 0);

  DATA_PTR(self) = fdata;

//  mrb_funcall(mrb, self, "set_style_gtk", 0);
  mrb_funcall(mrb, view, "sci_set_focus", 1, mrb_true_value());
  mrb_funcall(mrb, echo, "sci_set_focus", 1, mrb_false_value());
  //set_default_style

  gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(vbox), gtk_statusbar_new(), FALSE, FALSE, 0);
  gtk_widget_show_all(mainwin);
//gtk_widget_hide(GTK_WIDGET(grid));
  gtk_widget_grab_focus((GtkWidget *)DATA_PTR(view));

  /*
  gint w_w, w_h;
  gtk_window_get_size(GTK_WINDOW(mainwin), &w_w, &w_h);
  fprintf(stderr, "w = %d, h = %d\n", w_w, w_h);
  */
  return self;
}


void
mrb_mrbmacs_gtk_frame_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "frame_gtk_init",
    mrb_mrbmacs_frame_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "set_mode_text",
    mrb_mrbmacs_frame_set_mode_text, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "echo_gets",
    mrb_mrbmacs_frame_echo_gets, MRB_ARGS_ARG(1,2));

  mrb_mrbmacs_gtk_frame_search_init(mrb);
  mrb_mrbmacs_gtk_frame_select_init(mrb);
  mrb_mrbmacs_gtk_frame_tab_init(mrb);
  mrb_mrbmacs_gtk_window_init(mrb);
}
