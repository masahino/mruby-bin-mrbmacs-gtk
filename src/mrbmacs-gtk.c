#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"

#include <locale.h>
#include <gtk/gtk.h>
#ifdef MAC_INTEGRATION
#include <gtkosxapplication.h>
#endif
#include <Scintilla.h>

#include "mrbmacs-frame.h"
#include "mrbmacs-cb.h"
#include "mrbmacs-menu.h"

mrb_state *mrb;

static gboolean
mrbmacs_io_read_cb(GIOChannel *source, GIOCondition condition, gpointer data)
{
  gint fd;
  mrb_value read_io_a, io_handler, cb;
  mrb_value app = mrb_gv_get(mrb, mrb_intern_lit(mrb, "$app"));
  int i;
  if (condition & G_IO_IN) {
    fd = g_io_channel_unix_get_fd(source);
    read_io_a = mrb_iv_get(mrb, app, mrb_intern_lit(mrb, "@readings"));
    io_handler = mrb_iv_get(mrb, app, mrb_intern_lit(mrb, "@io_handler"));
    for (i = 0; i < RARRAY_LEN(read_io_a); i++) {
      mrb_value io_obj = mrb_ary_ref(mrb, read_io_a, i);
      mrb_value fd_obj = mrb_funcall(mrb, io_obj, "fileno", 0);
      if (mrb_fixnum(fd_obj) == fd) {
        mrb_value args[2];
        args[0] = app;
        args[1] = io_obj;
        cb = mrb_hash_get(mrb, io_handler, io_obj);
        mrb_yield_argv(mrb, cb, 2, args);
        break;
      }
    }
  }
  return TRUE;
}

static gboolean
mrbmacs_replace_next_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  mrb_value ret;
  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "replace_forward", 0);
  return FALSE;
}

static gboolean
mrbmacs_replace_prev_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  mrb_value ret;
  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "replace_backward", 0);
  return FALSE;
}

static gboolean
mrbmacs_find_next_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  mrb_value ret;
  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "isearch_forward", 0);
  return FALSE;
}

static gboolean
mrbmacs_find_prev_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  mrb_value ret;
  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "isearch_backward", 0);
  return FALSE;
}

static gboolean
mrbmacs_search_entry_changed(GtkSearchEntry *widget, gpointer user_data)
{
  mrb_value ret;
  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "isearch", 0);
  return FALSE;
}

static mrb_value
mrb_mrbmacs_editloop(mrb_state *mrb, mrb_value self)
{
  mrb_value frame_obj;
  mrb_value prefix_key;
  mrb_value edit_win;
  struct mrb_mrbmacs_frame_data *frame;

  prefix_key = mrb_str_new_lit(mrb, "");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@prefix_key"), prefix_key);
  
  frame_obj = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@frame"));
  frame = (struct mrb_mrbmacs_frame_data *)DATA_PTR(frame_obj);
  edit_win = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@edit_win"));
//  g_signal_connect(G_OBJECT((GtkWidget *)DATA_PTR(frame->view_win)),
  g_signal_connect(G_OBJECT(frame->mainwin),
    "key-press-event", G_CALLBACK(mrbmacs_keypress), &self);
//  g_signal_connect(G_OBJECT((GtkWidget *)DATA_PTR(frame->view_win)),
//    "sci-notify", G_CALLBACK(mrbmacs_sci_notify), &self);

  // find button
  g_signal_connect(G_OBJECT(frame->find_next_button),
    "button-press-event", G_CALLBACK(mrbmacs_find_next_button_press), &self);
  g_signal_connect(G_OBJECT(frame->find_prev_button),
    "button-press-event", G_CALLBACK(mrbmacs_find_prev_button_press), &self);
  // search entry
  g_signal_connect(G_OBJECT(frame->search_entry),
    "search-changed", G_CALLBACK(mrbmacs_search_entry_changed), &self);

  // replace button
  g_signal_connect(G_OBJECT(frame->replace_next_button),
    "button-press-event", G_CALLBACK(mrbmacs_replace_next_button_press), &self);
  g_signal_connect(G_OBJECT(frame->replace_prev_button),
    "button-press-event", G_CALLBACK(mrbmacs_replace_prev_button_press), &self);
  // notebook
  g_signal_connect(G_OBJECT(frame->notebook),
//    "select-page", G_CALLBACK(mrbmacs_select_tab), &self);
    "switch-page", G_CALLBACK(mrbmacs_select_tab), &self);

  // IO events
//  read_io_a = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@readings"));
//  for (i = 0; i < RARRAY_LEN(read_io_a); i++) {
//    mrb_value fd = mrb_funcall(mrb, mrb_ary_ref(mrb, read_io_a, i), "fileno", 0);
//    GIOChannel *channel = g_io_channel_unix_new(mrb_fixnum(fd));
//    g_io_add_watch(channel, G_IO_IN, mrbmacs_io_read_cb, &self);
//  }
#ifdef MAC_INTEGRATION
  GtkosxApplication *osxapp;
  osxapp = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
  g_signal_connect(osxapp, "NSApplicationOpenFile", G_CALLBACK(open_osx), &self);
#endif
  gtk_main();

  return self;
}

mrb_value
mrb_mrbmacs_add_gtk_io_callback(mrb_state *mrb, mrb_value self)
{
  mrb_value io;
  mrb_value fd;
  GIOChannel *channel;
  mrb_get_args(mrb, "o", &io);
  fd = mrb_funcall(mrb, io, "fileno", 0);
  channel = g_io_channel_unix_new(mrb_fixnum(fd));
  g_io_add_watch(channel, G_IO_IN, mrbmacs_io_read_cb, &self);
  return self;
}

mrb_value
mrb_mrbmacs_del_gtk_io_callback(mrb_state *mrb, mrb_value self)
{
  return self;
}

void
mrb_mruby_bin_mrbmacs_gtk_gem_init(mrb_state *mrb_in)
{
  struct RClass *mrbmacs_module;
  struct RClass *mrbmacs_class;

  mrb = mrb_in;
  mrbmacs_module = mrb_module_get(mrb, "Mrbmacs");
  mrbmacs_class = mrb_define_class_under(mrb, mrbmacs_module, "ApplicationGtk",
    mrb_class_get_under(mrb, mrbmacs_module, "Application"));

  mrb_define_method(mrb, mrbmacs_class, "editloop", mrb_mrbmacs_editloop, MRB_ARGS_NONE());
  mrb_define_method(mrb, mrbmacs_class, "add_gtk_io_callback", mrb_mrbmacs_add_gtk_io_callback, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrbmacs_class, "del_gtk_io_callback", mrb_mrbmacs_del_gtk_io_callback, MRB_ARGS_REQ(1));

  mrb_mrbmacs_gtk_frame_init(mrb);
}

void
mrb_mruby_bin_mrbmacs_gtk_gem_final(mrb_state* mrb)
{
}
