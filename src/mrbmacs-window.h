#ifndef MRBMACS_WINDOW_H
#define MRBMACS_WINDOW_H

//mrb_value scintilla_view_window_new(mrb_state *mrb, mrb_value v);
void mrb_mrbmacs_gtk_window_init(mrb_state *mrb);
int edit_win_get_height(mrb_state *mrb, mrb_value edit_win);
int edit_win_get_width(mrb_state *mrb, mrb_value edit_win);

#endif /* MRBMACS_WINDOW_H */
