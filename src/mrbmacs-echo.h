#ifndef MRBMACS_ECHO_H
#define MRBMACS_ECHO_H

#include <glib.h>
#include "mruby.h"

/* Who currently owns the keyboard, checked by mrbmacs_keypress().
 *   NONE  - keys go to the editor (key_press)
 *   MODAL - a wait_echo_event / wait_confirmation_event nested loop is running
 *           (echo_gets / y_or_n); keys go to mrbmacs_echo_handle_key
 *   KEY   - a non-modal echo interaction is running (isearch / query-replace);
 *           keys go to the app's echo_key handler
 */
enum {
  MRBMACS_ECHO_MODE_NONE = 0,
  MRBMACS_ECHO_MODE_MODAL = 1,
  MRBMACS_ECHO_MODE_KEY = 2
};

extern int mrbmacs_echo_active;

/* Handle a key while a MODAL echo prompt is up. Returns TRUE if the key was
 * consumed as a prompt control key (Enter / C-g / Tab, or y / n during a
 * confirmation), in which case the nested loop has been asked to quit. Returns
 * FALSE to let the key propagate to the focused echo widget for text / IME. */
gboolean mrbmacs_echo_handle_key(guint state, guint keyval);

void mrb_mrbmacs_gtk_frame_echo_init(mrb_state *mrb);

#endif /* MRBMACS_ECHO_H */
