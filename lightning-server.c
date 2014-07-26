#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "kit.h"

static void
callback(GtkWidget *widget,
         gpointer data) {
    g_print("Hello again - %s was pressed\n", (gchar *) data);
}

static gboolean
delete_event(GtkWidget *widget,
             GdkEvent *event,
             gpointer data) {
    g_print("delete event occurred\n");
    return FALSE;
}

// Exit gtk_main()
static void
destroy(GtkWidget *widget,
        gpointer data) {
    gtk_main_quit();
}

int main(int argc, char **argv) {

    /* int i; */

    // setup gtk widgets

    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;
	
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hello, Buttons!");

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    box1 = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), box1);

    // Button 1
    
    button = gtk_button_new_with_label("Button 1");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer) "button 1");
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_widget_show(button);

    // Button 2

    button = gtk_button_new_with_label("Button 2");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer) "button 2");
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_widget_show(button);

    // Quit button
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect(button, "clicked", G_CALLBACK(destroy), NULL);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_widget_show(button);

    gtk_widget_show(box1);
    gtk_widget_show(window);

    // setup kit

    const char * default_kit = "kits/default";

    Kit kit = Kit_load(default_kit);

    gtk_main();

    Kit_free(&kit);

    return 0;
}
