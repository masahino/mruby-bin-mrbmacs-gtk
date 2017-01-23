#ifndef MRBMACS_GTK_H
#define MRBMACS_GTK_H

struct mrb_mrbmacs_frame_data {
  GtkWidget *mainwin;
  mrb_value view_win;
  GtkWidget *mode_win;
  mrb_value echo_win;
};

void mrbmacs_gtk_init(mrb_state *mrb);

#endif /* MRBMACS_GTK_H */