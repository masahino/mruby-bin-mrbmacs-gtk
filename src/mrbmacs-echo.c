/*
 * Echo-area minibuffer input for the GTK frontend.
 *
 * A prompt (echo_gets / y_or_n) is a blocking call from Ruby's point of view,
 * but GTK owns the main loop, so it is implemented with a nested gtk_main()
 * that a key handler ends with gtk_main_quit() - the same shape as the cocoa
 * frontend's wait_echo_event / -[NSApp runModalForWindow:].
 */

#include <locale.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "mruby.h"
#include "mruby/variable.h"

#include "mrbmacs-echo.h"

typedef enum {
  MRBMACS_ECHO_NONE = 0,
  MRBMACS_ECHO_ENTER,
  MRBMACS_ECHO_CANCEL,
  MRBMACS_ECHO_TAB,
  MRBMACS_ECHO_YES,
  MRBMACS_ECHO_NO
} mrbmacs_echo_response_t;

int mrbmacs_echo_active = 0;
static int mrbmacs_echo_confirmation = 0;
static int mrbmacs_echo_choice = 0;
static guint mrbmacs_echo_choice_keyval = 0;
static mrbmacs_echo_response_t mrbmacs_echo_response = MRBMACS_ECHO_NONE;

gboolean
mrbmacs_echo_handle_key(guint state, guint keyval)
{
  const gboolean ctrl = (state & GDK_CONTROL_MASK) != 0;

  if (mrbmacs_echo_choice) {
    if (ctrl && keyval == GDK_KEY_g) {
      mrbmacs_echo_choice_keyval = 0;
    } else if (ctrl) {
      return TRUE;
    } else if (gdk_keyval_to_unicode(keyval) < 128) {
      mrbmacs_echo_choice_keyval = keyval;
    } else {
      return TRUE;
    }
    gtk_main_quit();
    return TRUE;
  }

  if (mrbmacs_echo_confirmation) {
    if (keyval == GDK_KEY_y || keyval == GDK_KEY_Y) {
      mrbmacs_echo_response = MRBMACS_ECHO_YES;
    } else if (keyval == GDK_KEY_n || keyval == GDK_KEY_N ||
               (ctrl && keyval == GDK_KEY_g)) {
      mrbmacs_echo_response = MRBMACS_ECHO_NO;
    } else {
      return TRUE; /* ignore any other key during a y/n prompt */
    }
    gtk_main_quit();
    return TRUE;
  }

  if (ctrl && keyval == GDK_KEY_g) {
    mrbmacs_echo_response = MRBMACS_ECHO_CANCEL;
  } else if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
    mrbmacs_echo_response = MRBMACS_ECHO_ENTER;
  } else if (keyval == GDK_KEY_Tab) {
    mrbmacs_echo_response = MRBMACS_ECHO_TAB;
  } else {
    return FALSE; /* propagate to the focused echo widget */
  }
  gtk_main_quit();
  return TRUE;
}

static mrb_value
mrb_mrbmacs_frame_wait_echo_event(mrb_state *mrb, mrb_value self)
{
  mrb_value echo_win = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@echo_win"));

  mrb_funcall(mrb, echo_win, "sci_grab_focus", 0);

  mrbmacs_echo_response = MRBMACS_ECHO_NONE;
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_MODAL;
  gtk_main();
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_NONE;

  switch (mrbmacs_echo_response) {
  case MRBMACS_ECHO_ENTER:
    return mrb_symbol_value(mrb_intern_lit(mrb, "enter"));
  case MRBMACS_ECHO_TAB:
    return mrb_symbol_value(mrb_intern_lit(mrb, "tab"));
  default:
    return mrb_symbol_value(mrb_intern_lit(mrb, "cancel"));
  }
}

static mrb_value
mrb_mrbmacs_frame_wait_confirmation_event(mrb_state *mrb, mrb_value self)
{
  mrb_value echo_win = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@echo_win"));

  mrb_funcall(mrb, echo_win, "sci_grab_focus", 0);

  mrbmacs_echo_response = MRBMACS_ECHO_NONE;
  mrbmacs_echo_confirmation = 1;
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_MODAL;
  gtk_main();
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_NONE;
  mrbmacs_echo_confirmation = 0;

  return mrb_symbol_value(mrb_intern_lit(
    mrb, mrbmacs_echo_response == MRBMACS_ECHO_YES ? "yes" : "no"));
}

static mrb_value
mrb_mrbmacs_frame_wait_choice_event(mrb_state *mrb, mrb_value self)
{
  mrb_value echo_win = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@echo_win"));
  gunichar character;
  gchar text[7];
  gint length;

  mrb_funcall(mrb, echo_win, "sci_grab_focus", 0);

  mrbmacs_echo_choice_keyval = 0;
  mrbmacs_echo_choice = 1;
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_MODAL;
  gtk_main();
  mrbmacs_echo_active = MRBMACS_ECHO_MODE_NONE;
  mrbmacs_echo_choice = 0;

  if (mrbmacs_echo_choice_keyval == 0) {
    return mrb_nil_value();
  }
  character = gdk_keyval_to_unicode(mrbmacs_echo_choice_keyval);
  length = g_unichar_to_utf8(character, text);
  return mrb_str_new(mrb, text, length);
}

/* Enter / leave the non-modal echo key mode (isearch / query-replace).
 * Called from the frame's start_isearch / finish_isearch etc. */
static mrb_value
mrb_mrbmacs_frame_echo_key_mode(mrb_state *mrb, mrb_value self)
{
  mrb_bool on;
  mrb_get_args(mrb, "b", &on);
  if (mrbmacs_echo_active != MRBMACS_ECHO_MODE_MODAL) {
    mrbmacs_echo_active = on ? MRBMACS_ECHO_MODE_KEY : MRBMACS_ECHO_MODE_NONE;
  }
  return self;
}

void
mrb_mrbmacs_gtk_frame_echo_init(mrb_state *mrb)
{
  struct RClass *frame = mrb_class_get_under(
    mrb, mrb_module_get(mrb, "Mrbmacs"), "Frame");

  mrb_define_method(mrb, frame, "wait_echo_event",
    mrb_mrbmacs_frame_wait_echo_event, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "wait_confirmation_event",
    mrb_mrbmacs_frame_wait_confirmation_event, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "wait_choice_event",
    mrb_mrbmacs_frame_wait_choice_event, MRB_ARGS_NONE());
  mrb_define_method(mrb, frame, "echo_key_mode",
    mrb_mrbmacs_frame_echo_key_mode, MRB_ARGS_REQ(1));
}
