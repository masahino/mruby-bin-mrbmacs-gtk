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
#include <Scintilla.h>

#include "mrbmacs-frame.h"

extern mrb_state *mrb;

gboolean
//mrbmacs_select_tab(GtkWidget *widget, gboolean arg1, gpointer data)
mrbmacs_select_tab(GtkWidget *notebook, GtkWidget *page, guint page_num, gpointer data)
{
  mrb_value app;
  mrb_value frame_obj;
  mrb_value buffername_obj;
  gchar *buffername;

  app = *(mrb_value *)data;
  buffername = gtk_notebook_get_tab_label_text(notebook, page);
  mrb_funcall(mrb, app, "switch_to_buffer", 1, mrb_str_new_cstr(mrb, buffername));
  return FALSE;
}

gboolean
mrbmacs_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  mrb_value send_key;
  mrb_value app;
  app = *(mrb_value *)data;

  send_key = mrb_funcall(mrb, app, "key_press", 2, mrb_fixnum_value(event->state),
    mrb_fixnum_value(event->keyval));
  if (mrb_type(send_key) == MRB_TT_TRUE) {
    return FALSE;
  } else {
    return TRUE;
  }
}

gboolean
mrbmacs_sci_notify(GtkWidget *widget, gint n, SCNotification *notification, gpointer user_data)
{
  mrb_value ret, scn;

  scn = mrb_hash_new(mrb);
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "code"), mrb_fixnum_value(notification->nmhdr.code));
  // Sci_Position position
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "position"), mrb_fixnum_value(notification->position));
  // int ch
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "ch"), mrb_fixnum_value(notification->ch));
  // int modifiers
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "modifiers"), mrb_fixnum_value(notification->modifiers));
  // int modificationType
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "modification_type"),
    mrb_fixnum_value(notification->modificationType));
  // const char *text
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "text"), mrb_str_new_cstr(mrb, notification->text));
  // Sci_Position length
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "length"), mrb_fixnum_value(notification->length));
  // Sci_Position linesAdded
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "lines_added"), mrb_fixnum_value(notification->linesAdded));
  // int message
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "message"), mrb_fixnum_value(notification->message));
  // uptr_t wParam
  // sptr_t lParam
  // Sci_Position line
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "line"), mrb_fixnum_value(notification->line));
  // int foldLevelNow
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "fold_level_now"),
    mrb_fixnum_value(notification->foldLevelNow));
  // int foldLevelPrev
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "fold_level_prev"),
    mrb_fixnum_value(notification->foldLevelPrev));
  // int margin
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "margin"), mrb_fixnum_value(notification->margin));
  // int listType
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "list_type"), mrb_fixnum_value(notification->listType));
  // int x
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "x"), mrb_fixnum_value(notification->x));
  // int y
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "y"), mrb_fixnum_value(notification->y));
  // int token
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "token"), mrb_fixnum_value(notification->token));
  // int annotationLinesAdded
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "annotation_lines_added"),
    mrb_fixnum_value(notification->annotationLinesAdded));
  // int updated
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "updated"), mrb_fixnum_value(notification->updated));
  // listCompletionMethod
  mrb_hash_set(mrb, scn, mrb_str_new_cstr(mrb, "list_completion_method"),
    mrb_fixnum_value(notification->listCompletionMethod));

  ret = mrb_funcall(mrb, *(mrb_value *)user_data, "sci_notify", 1, scn);

  return FALSE;
}
