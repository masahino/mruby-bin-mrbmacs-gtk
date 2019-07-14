#ifndef MRBMACS_CB_H
#define MRBMACS_CB_H

gboolean mrbmacs_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean mrbmacs_keypress2(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean mrbmacs_sci_notify(GtkWidget *widget, gint n, SCNotification *notification, gpointer user_data);

#endif /* MRBMACS_CB_H */
