#ifndef MRBMACS_ECHO_H
#define MRBMACS_ECHO_H

#include <glib.h>
#include "mruby.h"

/* Non-zero while a wait_echo_event / wait_confirmation_event nested loop is
 * running. mrbmacs_keypress() checks this to route keys to the echo prompt. */
extern int mrbmacs_echo_active;

/* Handle a key while echo is active. Returns TRUE if the key was consumed as a
 * prompt control key (Enter / C-g / Tab, or y / n during a confirmation), in
 * which case the nested loop has been asked to quit. Returns FALSE to let the
 * key propagate to the focused echo widget for text input / IME. */
gboolean mrbmacs_echo_handle_key(guint state, guint keyval);

void mrb_mrbmacs_gtk_frame_echo_init(mrb_state *mrb);

#endif /* MRBMACS_ECHO_H */
