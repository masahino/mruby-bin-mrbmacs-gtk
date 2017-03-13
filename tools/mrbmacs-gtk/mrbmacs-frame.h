#ifndef MRBMACS_FRAME_H
#define MRBMACS_FRAME_H

struct mrb_mrbmacs_frame_data {
  GtkWidget *mainwin;
  mrb_value view_win;
  GtkWidget *mode_win;
  mrb_value echo_win;
  GtkWidget *search_entry;
  GtkWidget *find_next_button;
  GtkWidget *find_prev_button;
  GtkWidget *replace_entry;
  GtkWidget *replace_next_button;
  GtkWidget *replace_prev_button;
  GtkWidget *notebook;
};

void mrbmacs_gtk_init(mrb_state *mrb);

#endif /* MRBMACS_FRAME_H */
