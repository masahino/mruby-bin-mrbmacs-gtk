#ifndef MRBMACS_FRAME_H
#define MRBMACS_FRAME_H

struct mrb_mrbmacs_frame_data {
  GtkWidget *mainwin;
  GtkWidget *mode_win;
  GtkWidget *search_bar;
  GtkWidget *search_entry;
  GtkWidget *find_next_button;
  GtkWidget *find_prev_button;
  GtkWidget *replace_bar;
  GtkWidget *replace_entry;
  GtkWidget *replace_next_button;
  GtkWidget *replace_prev_button;
  GtkWidget *notebook;
  GtkWidget *status_bar;
  GtkWidget *menu;
};

void mrb_mrbmacs_gtk_frame_init(mrb_state *mrb);

#endif /* MRBMACS_FRAME_H */
