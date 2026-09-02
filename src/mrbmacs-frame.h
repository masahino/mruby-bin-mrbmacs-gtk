#ifndef MRBMACS_FRAME_H
#define MRBMACS_FRAME_H

struct mrb_mrbmacs_frame_data {
  GtkWidget *mainwin;
  GtkWidget *mode_win;
  /* The GtkSearchBar search/replace UI was removed in favour of the echo
     area; these 8 slots are kept unused so the struct layout is unchanged. */
  GtkWidget *unused_search_1;
  GtkWidget *unused_search_2;
  GtkWidget *unused_search_3;
  GtkWidget *unused_search_4;
  GtkWidget *unused_search_5;
  GtkWidget *unused_search_6;
  GtkWidget *unused_search_7;
  GtkWidget *unused_search_8;
  GtkWidget *notebook;
  GtkWidget *menu;
};

void mrb_mrbmacs_gtk_frame_init(mrb_state *mrb);

#endif /* MRBMACS_FRAME_H */
