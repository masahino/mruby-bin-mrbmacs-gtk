#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/string.h"

#include <locale.h>
#include <gtk/gtk.h>
#include <Scintilla.h>

#include "mrbmacs-frame.h"

extern mrb_state *mrb;

gboolean
mrbmacs_menu_run_command(GtkWidget *widget, gpointer user_data)
{
  mrb_value app;
  mrb_value ret;
  app = mrb_gv_get(mrb, mrb_intern_lit(mrb, "$app"));
  ret = mrb_funcall(mrb, app, (char *)user_data, 0);
  return FALSE;
}

GtkWidget *create_file_menu()
{

  GtkWidget *menuitem_root, *menu;
  GtkWidget *menuitem;
  menuitem_root = gtk_menu_item_new_with_label("File");
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_root), menu);

  menuitem = gtk_menu_item_new_with_label("Open");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  g_signal_connect(menuitem, "activate", G_CALLBACK(mrbmacs_menu_run_command), (gpointer)"find_file");

  menuitem = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  g_signal_connect(menuitem, "activate", G_CALLBACK(gtk_main_quit), NULL);
  return menuitem_root;
}

GtkWidget *create_options_menu()
{

  GtkWidget *menuitem_root, *menu, *menuitem;
  menuitem_root = gtk_menu_item_new_with_label("Options");
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_root), menu);
  menuitem = gtk_menu_item_new_with_label("Change Theme");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  g_signal_connect(menuitem, "activate", G_CALLBACK(mrbmacs_menu_run_command), (gpointer)"select_theme");

  menuitem = gtk_menu_item_new_with_label("Change Font");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  g_signal_connect(menuitem, "activate", G_CALLBACK(mrbmacs_menu_run_command), (gpointer)"select_font");

  return menuitem_root;
}


GtkWidget *mrbmacs_create_gtk_menu()
{
  GtkWidget *menubar;
  menubar = gtk_menu_bar_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), create_file_menu());
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), create_options_menu());
  return menubar;
}