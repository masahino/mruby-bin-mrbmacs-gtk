#include <locale.h>ev
#include <gtk/gtk.h>

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/variable.h"

#include <Scintilla.h>
#include <SciLexer.h>
#define PLAT_GTK 1
#include <ScintillaWidget.h>

#include "mrbmacs-frame.h"

static const struct mrb_data_type mrb_mrbmacs_frame_data_type = {
  "mrb_mrbmacs_frame_data", mrb_free,
};

static mrb_value
mrb_mrbmacs_frame_search_entry_get_text(mrb_state *mrb, mrb_value self)
{
  GtkWidget *isearch_entry;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  isearch_entry = fdata->search_entry;
//  gtk_widget_grab_focus(isearch_entry);
  return mrb_str_new_cstr(mrb, gtk_entry_get_text(GTK_ENTRY(isearch_entry)));
}

static mrb_value
mrb_mrbmacs_frame_set_mode_text(mrb_state *mrb, mrb_value self)
{
  GtkWidget *mode_win;
  struct mrb_mrbmacs_frame_data *fdata = (struct mrb_mrbmacs_frame_data *)DATA_PTR(self);
  char *mode_str;

  mrb_get_args(mrb, "z", &mode_str);
  mode_win = fdata->mode_win;
  fprintf(stderr, "mode_str = %s\n", mode_str);
  gtk_label_set_text(GTK_LABEL(mode_win), mode_str);
}

static mrb_value
mrb_mrbmacs_frame_select_file(mrb_state *mrb, mrb_value self)
{
  GtkWidget *view_win, *dialog;
  char *title;
  char *path;
  char *filename = NULL;
  
  fprintf(stderr, "select_file\n");
  view_win = (GtkWidget *)DATA_PTR(mrb_iv_get(mrb, self, mrb_intern_cstr(mrb, "@view_win")));
  mrb_get_args(mrb, "zz", &title, &path);
  dialog = gtk_file_chooser_dialog_new(title, GTK_WINDOW(view_win), GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
  if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  }
  gtk_widget_destroy(dialog);
  return mrb_str_new_cstr(mrb, filename);
}

static mrb_value
mrb_mrbmacs_frame_init(mrb_state *mrb, mrb_value self)
{
  GtkWidget *mainwin, *vbox, *mode, *search_entry, *hbox;
  GtkWidget *find_button;
  GtkWidget *notebook;
  struct RClass *scintilla_gtk_class;
  int font_width, font_height;
  mrb_value view, echo;
  struct mrb_mrbmacs_frame_data *fdata =
  (struct mrb_mrbmacs_frame_data *)malloc(sizeof(struct mrb_mrbmacs_frame_data));

  mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  vbox = gtk_vbox_new(false, 2);
  gtk_container_add(GTK_CONTAINER(mainwin), vbox);
  
  notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, FALSE, FALSE, 0);

  scintilla_gtk_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "Scintilla"), "ScintillaGtk");
  DATA_TYPE(self) = &mrb_mrbmacs_frame_data_type;
  DATA_PTR(self) = NULL;
  fdata->mainwin = mainwin;

  view = mrb_funcall(mrb, mrb_obj_value(scintilla_gtk_class), "new", 0);
//  scintilla_send_message(SCINTILLA((GtkWidget *)DATA_PTR(view)), SCI_STYLESETSIZE, STYLE_DEFAULT, "Monospace");

  mrb_funcall(mrb, view, "sci_style_set_font", 2, mrb_fixnum_value(STYLE_DEFAULT), 
    mrb_str_new_lit(mrb, "Monospace"));
  mrb_funcall(mrb, view, "sci_style_set_size", 2, mrb_fixnum_value(STYLE_DEFAULT), mrb_fixnum_value(14));
  fprintf(stderr, "sss%d\n", mrb_type(mrb_funcall(mrb, view, "sci_text_width", 2, mrb_fixnum_value(STYLE_DEFAULT),
      mrb_str_new_lit(mrb, "A"))));
  
  font_width = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_width", 2, mrb_fixnum_value(STYLE_DEFAULT),
      mrb_str_new_lit(mrb, "A")));
  fprintf(stderr, "font_width = %d\n", font_width);
  font_height = mrb_int(mrb, mrb_funcall(mrb, view, "sci_text_height", 1, mrb_fixnum_value(1)));
  fprintf(stderr, "font_height = %d\n", font_height);
  gtk_widget_set_size_request((GtkWidget *)DATA_PTR(view), font_width*(80+6), font_height*40);
//  gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget*)DATA_PTR(view), FALSE, FALSE, 0);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), (GtkWidget *)DATA_PTR(view), gtk_label_new(""));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
  mrb_funcall(mrb, view, "sci_set_caret_fore", 1, mrb_fixnum_value(0xffffff));
  mrb_funcall(mrb, view, "sci_set_caret_style", 1, mrb_fixnum_value(2));
  
  fdata->view_win = view;
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@view_win"), view);

  mode = gtk_label_new("");
  gtk_label_set_justify(GTK_LABEL(mode), GTK_JUSTIFY_LEFT);
  gtk_widget_set_halign(mode, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), mode, TRUE, TRUE, 0);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@mode_win"), mrb_cptr_value(mrb, mode));
  fdata->mode_win = mode;
  
  echo = mrb_funcall(mrb, mrb_obj_value(scintilla_gtk_class), "new", 0);
  gtk_widget_set_size_request((GtkWidget*)DATA_PTR(echo), font_width*80, font_height);
  gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget *)DATA_PTR(echo), TRUE, TRUE, 0);

  mrb_funcall(mrb, echo, "sci_set_hscroll_bar", 1, mrb_false_value());
  mrb_funcall(mrb, echo, "sci_clear_cmd_key", 1, mrb_fixnum_value(SCK_RETURN));

  fdata->echo_win = echo;
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@echo_win"), echo);
  
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
  search_entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(hbox), search_entry, FALSE, FALSE, 2);
  find_button = gtk_button_new_with_label("find");
  gtk_box_pack_start(GTK_BOX(hbox), find_button, FALSE, FALSE, 2);
  fdata->search_entry = search_entry;
  fdata->find_button = find_button;

  DATA_PTR(self) = fdata;

  mrb_funcall(mrb, self, "set_default_style", 0);
  mrb_funcall(mrb, self, "set_style_gtk", 0);
  mrb_funcall(mrb, view, "sci_set_focus", 1, mrb_true_value());
  mrb_funcall(mrb, echo, "sci_set_focus", 1, mrb_false_value());
  //set_default_style
      
  gtk_box_pack_start(GTK_BOX(vbox), gtk_statusbar_new(), TRUE, TRUE, 0);

  gtk_widget_show_all(mainwin);
  gtk_widget_grab_focus((GtkWidget *)DATA_PTR(view));
  
  return self;
}

void
mrbmacs_gtk_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");
  mrb_define_method(mrb, frame, "initialize", mrb_mrbmacs_frame_init, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "select_file", mrb_mrbmacs_frame_select_file, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, frame, "set_mode_text", mrb_mrbmacs_frame_set_mode_text, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, frame, "search_entry_get_text", mrb_mrbmacs_frame_search_entry_get_text, MRB_ARGS_NONE());
}
