#ifndef MRBMACS_CB_H
#define MRBMACS_CB_H

//gboolean mrbmacs_select_tab(GtkWidget *widget, gboolean arg1, gpointer data);
#ifdef MAC_INTEGRATION
void open_osx(GtkosxApplication *osxapp, gchar *path, gpointer p);
#endif
gboolean mrbmacs_select_tab(GtkWidget *widget, GtkWidget *page, guint page_num, gpointer data);
gboolean mrbmacs_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean mrbmacs_sci_notify(GtkWidget *widget, gint n, SCNotification *notification, gpointer user_data);

#endif /* MRBMACS_CB_H */
