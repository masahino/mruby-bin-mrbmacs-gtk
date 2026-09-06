#ifndef MRBMACS_MENU_H
#define MRBMACS_MENU_H

GtkWidget *mrbmacs_create_gtk_menu();
gboolean mrbmacs_request_exit(GtkWidget *widget, GdkEvent *event,
                              gpointer user_data);

#endif /* MRBMACS_MENU_H */
