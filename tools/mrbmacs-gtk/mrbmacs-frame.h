#ifndef MRBMACS_FRAME_H
#define MRBMACS_FRAME_H

struct mrb_mrbmacs_frame_data {
  GtkWidget *mainwin;
  mrb_value view_win;
  GtkWidget *mode_win;
  mrb_value echo_win;
  GtkWidget *search_entry;
  GtkWidget *find_button;
};

void mrbmacs_gtk_init(mrb_state *mrb);

#endif /* MRBMACS_FRAME_H */
