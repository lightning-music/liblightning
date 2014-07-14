#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

static gboolean
delete_event(GtkWidget *widget,
             GdkEvent *event,
             gpointer data) {
    gtk_main_quit();
    return FALSE;
}

static GtkWidget *
make_box(gboolean homogeneous,
         gint spacing,
         gboolean expand,
         gboolean fill,
         guint padding) {
    GtkWidget *box;
    GtkWidget *button;
    char padstr[80];

    box = gtk_hbox_new(homogeneous, spacing);
    button = gtk_button_new_with_label("gtk_box_pack");
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("(box,");
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("(button,");
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    if (expand == TRUE) {
        button = gtk_button_new_with_label("TRUE,");
    } else {
        button = gtk_button_new_with_label("FALSE,");
    }

    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    button = gtk_button_new_with_label(fill ? "TRUE," : "FALSE,");
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    sprintf(padstr, "%d);", padding);

    button = gtk_button_new_with_label(padstr);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    return box;
}

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *separator;
    GtkWidget *label;
    GtkWidget *quitbox;
    int which;
    
    /* Our init, don't forget this! :) */
    gtk_init(&argc, &argv);
    
    if (argc != 2) {
        fprintf(stderr, "usage: packbox num, where num is 1, 2, or 3.\n");
        /* This just does cleanup in GTK and exits with an exit status of 1. */
        exit(1);
    }

    which = atoi(argv[1]);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    box1 = gtk_vbox_new(FALSE, 0);

    switch(which) {
    case 1:
        label = gtk_label_new("gtk_hbox_new(FALSE, 0)");
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        gtk_box_pack_start(GTK_BOX(box1), label, FALSE, FALSE, 0);
        gtk_widget_show(label);

        /* Call our make box function - homogeneous = FALSE, spacing = 0,
         * expand = FALSE, fill = FALSE, padding = 0 */
        box2 = make_box(FALSE, 0, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(box1), box2, FALSE, FALSE, 0);
        gtk_widget_show(box2);

        /* Call our make box function - homogeneous = FALSE, spacing = 0,
         * expand = TRUE, fill = FALSE, padding = 0 */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* Creates a separator, we'll learn more about these later, 
         * but they are quite simple. */
        separator = gtk_hseparator_new ();

        /* Pack the separator into the vbox. Remember each of these
         * widgets is being packed into a vbox, so they'll be stacked
         * vertically. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        /* Create another new label, and show it. */
        label = gtk_label_new ("gtk_hbox_new (TRUE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (TRUE, 0, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* Another new separator. */
        separator = gtk_hseparator_new ();
        /* The last 3 arguments to gtk_box_pack_start are:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        break;
    case 2:
        /* Create a new label, remember box1 is a vbox as created 
         * near the beginning of main() */
        label = gtk_label_new ("gtk_hbox_new (FALSE, 10);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 10, TRUE, TRUE, 0);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        separator = gtk_hseparator_new ();
        /* The last 3 arguments to gtk_box_pack_start are:
         * expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
        
        label = gtk_label_new ("gtk_hbox_new (FALSE, 0);");
        gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
        
        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        /* Args are: homogeneous, spacing, expand, fill, padding */
        box2 = make_box (FALSE, 0, TRUE, TRUE, 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);
        
        separator = gtk_hseparator_new ();
        /* The last 3 arguments to gtk_box_pack_start are: expand, fill, padding. */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);

        break;
    case 3:

        /* This demonstrates the ability to use gtk_box_pack_end() to
         * right justify widgets. First, we create a new box as before. */
        box2 = make_box (FALSE, 0, FALSE, FALSE, 0);

        /* Create the label that will be put at the end. */
        label = gtk_label_new ("end");
        /* Pack it using gtk_box_pack_end(), so it is put on the right
         * side of the hbox created in the make_box() call. */
        gtk_box_pack_end (GTK_BOX (box2), label, FALSE, FALSE, 0);
        /* Show the label. */
        gtk_widget_show (label);
        
        /* Pack box2 into box1 (the vbox remember ? :) */
        gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
        gtk_widget_show (box2);

        /* A separator for the bottom. */
        separator = gtk_hseparator_new ();
        /* This explicitly sets the separator to 400 pixels wide by 5 pixels
         * high. This is so the hbox we created will also be 400 pixels wide,
         * and the "end" label will be separated from the other labels in the
         * hbox. Otherwise, all the widgets in the hbox would be packed as
         * close together as possible. */
        gtk_widget_set_size_request (separator, 400, 5);
        /* pack the separator into the vbox (box1) created near the start 
         * of main() */
        gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 5);
        gtk_widget_show (separator);
    }

    quitbox = gtk_hbox_new(FALSE, 0);

    button = gtk_button_new_with_label("Quit");

    g_signal_connect_swapped(button,
                             "clicked",
                             G_CALLBACK(gtk_main_quit),
                             window);

    /* Pack the button into the quitbox.
     * The last 3 arguments to gtk_box_pack_start are:
     * expand, fill, padding. */
    gtk_box_pack_start (GTK_BOX (quitbox), button, TRUE, FALSE, 0);
    /* pack the quitbox into the vbox (box1) */
    gtk_box_pack_start (GTK_BOX (box1), quitbox, FALSE, FALSE, 0);
    
    /* Pack the vbox (box1) which now contains all our widgets, into the
     * main window. */
    gtk_container_add (GTK_CONTAINER (window), box1);
    
    /* And show everything left */
    gtk_widget_show (button);
    gtk_widget_show (quitbox);

    gtk_widget_show (box1);
    /* Showing the window last so everything pops up at once. */
    gtk_widget_show (window);
    
    /* And of course, our main function. */
    gtk_main ();

    /* Control returns here when gtk_main_quit() is called, but not when 
     * exit() is used. */

    return 0;
}
