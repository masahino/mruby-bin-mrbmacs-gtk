#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>
#ifdef MAC_INTEGRATION
#include <gtkosxapplication.h>
#endif

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
#include "mrbmacs-select.h"
#include "mrbmacs-tab.h"
#include "mrbmacs-echo.h"
#include "mrbmacs-window.h"
#include "mrbmacs-cb.h"
#include "mrbmacs-menu.h"

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

  /* Keep in step with Frame::INITIAL_COLUMNS (mrblib/frame-gtk.rb). */
  gtk_widget_set_size_request((GtkWidget*)DATA_PTR(echo), font_width*120, font_height);

  mrb_funcall(mrb, echo, "sci_set_hscroll_bar", 1, mrb_false_value());
  mrb_funcall(mrb, echo, "sci_clear_cmd_key", 1, mrb_fixnum_value(SCK_RETURN));

  /* Text changes here drive isearch / query-replace. */
  g_signal_connect(G_OBJECT((GtkWidget *)DATA_PTR(echo)),
    "sci-notify", G_CALLBACK(mrbmacs_echo_sci_notify), NULL);

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


static mrb_value
mrb_mrbmacs_frame_init(mrb_state *mrb, mrb_value self)
{
  GtkWidget *mainwin, *vbox, *mode;
  GtkWidget *notebook;
  GtkWidget *menubar;
  GtkAccelGroup *accel_group;
#ifdef MAC_INTEGRATION
  GtkosxApplication *osxapp;
#endif

  struct RClass *mrbmacs_module;
//  int font_width, font_height;
  mrb_value view, echo;
  mrb_value buffer, edit_win;
  mrb_value edit_win_list;
  mrb_int width = 0, height = 0;

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  mrb_get_args(mrb, "oii", &buffer, &width, &height);

  struct mrb_mrbmacs_frame_data *fdata =
  (struct mrb_mrbmacs_frame_data *)mrb_malloc(mrb, sizeof(struct mrb_mrbmacs_frame_data));

  mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//  gtk_widget_set_size_request(mainwin, -1, -1);
//  gtk_window_set_default_size(GTK_WINDOW(mainwin), 738, 768);

#ifdef MAC_INTEGRATION
  osxapp = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
  gtkosx_application_set_use_quartz_accelerators(osxapp, FALSE);
#endif

  DATA_TYPE(self) = &mrb_mrbmacs_frame_data_type;
  DATA_PTR(self) = NULL;
  fdata->mainwin = mainwin;

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
  gtk_container_add(GTK_CONTAINER(mainwin), vbox);

  accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group(GTK_WINDOW(mainwin), accel_group);

  // menu
  menubar = mrbmacs_create_gtk_menu();
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, TRUE, 0);
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
  gtk_widget_set_margin_start(GTK_WIDGET(mode), 8);
  gtk_widget_set_margin_top(GTK_WIDGET(mode), 2);
  gtk_widget_set_margin_bottom(GTK_WIDGET(mode), 2);
  gtk_box_pack_start(GTK_BOX(vbox), mode, FALSE, FALSE, 0);
  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@mode_win"), mrb_cptr_value(mrb, mode));
  fdata->mode_win = mode;

//  gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, TRUE, 0);

  echo = scintilla_echo_window_new(mrb, self);
  gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget *)DATA_PTR(echo), FALSE, FALSE, 0);

  mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, "@echo_win"), echo);

  DATA_PTR(self) = fdata;

//  mrb_funcall(mrb, self, "set_style_gtk", 0);
  mrb_funcall(mrb, view, "sci_set_focus", 1, mrb_true_value());
  mrb_funcall(mrb, echo, "sci_set_focus", 1, mrb_false_value());
  //set_default_style

/*
  gint w_w, w_h;
  gtk_window_get_size(GTK_WINDOW(mainwin), &w_w, &w_h);
  fprintf(stderr, "w = %d, h = %d\n", w_w, w_h);
  fprintf(stderr, "w = %d, h = %d + %d\n", 
    edit_win_get_width(mrb, edit_win),
    gtk_widget_get_allocated_height(GTK_WIDGET(vbox)),edit_win_get_height(mrb, edit_win));
*/
//  gtk_window_set_default_size(GTK_WINDOW(mainwin), 738, 768);
  gtk_window_set_default_size(GTK_WINDOW(mainwin),
    edit_win_get_width(mrb, edit_win) + 2,
    edit_win_get_height(mrb, edit_win) + 90); // 112

  /* Open on the monitor under the pointer (i.e. where it was launched from)
     rather than wherever the GdkQuartz default places it. Window stacking on
     macOS is left as-is. */
  gtk_window_set_position(GTK_WINDOW(mainwin), GTK_WIN_POS_MOUSE);
  gtk_widget_show_all(mainwin);

#ifdef MAC_INTEGRATION
  gtkosx_application_set_menu_bar(osxapp, GTK_MENU_SHELL(menubar));
  gtk_widget_hide(menubar);
#endif


  gtk_widget_grab_focus((GtkWidget *)DATA_PTR(view));

#ifdef MAC_INTEGRATION
  gtkosx_application_ready(osxapp);
#endif
  return self;
}


void
mrb_mrbmacs_gtk_frame_init(mrb_state *mrb)
{
  struct RClass *mrbmacs_module, *frame; 

  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  frame = mrb_class_get_under(mrb, mrbmacs_module, "Frame");

  mrb_define_method(mrb, frame, "frame_gtk_init",
    mrb_mrbmacs_frame_init, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, frame, "set_mode_text",
    mrb_mrbmacs_frame_set_mode_text, MRB_ARGS_REQ(1));

  mrb_mrbmacs_gtk_frame_select_init(mrb);
  mrb_mrbmacs_gtk_frame_tab_init(mrb);
  mrb_mrbmacs_gtk_frame_echo_init(mrb);
  mrb_mrbmacs_gtk_window_init(mrb);
}
