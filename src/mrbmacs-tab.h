#ifndef MRBMACS_FRAME_TAB_H
#define MRBMACS_FRAME_TAB_H

void mrb_mrbmacs_gtk_frame_tab_init(mrb_state *mrb);
mrb_value add_new_edit_win_with_tab(mrb_state *mrb, mrb_value frame, mrb_value buffer, GtkWidget *notebook);

#endif /* MRBMACS_FRAME_TAB_H */
