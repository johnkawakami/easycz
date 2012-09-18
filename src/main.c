/*
#include <config.h>
#include <signal.h>
#include <stdlib.h>
#include <glib/glib.h>
*/

#include "gtk/gtk.h"
#include "config.h"


static gboolean delete_event( GtkWidget *widget,
                              GdkEvent *event,
                              gpointer data )
{
    g_print("delete event\n");
    return TRUE;
}

static void destroy( GtkWidget *widget,
                     gpointer data )
{
    gtk_main_quit();
}
static void hello( GtkWidget *widget, gpointer data )
{
    g_print("Hello");
}

int main( int argc, char *argv[] )
{
    GtkWidget *window;
    GtkWidget *button[6];
    GtkWidget *box;
    GtkWidget *image[6];
    int i;

    config->windowcounter = 0;
    
    gtk_init( &argc, &argv );
    /* parse the yaml file to build the gui */
    if (!config_parse("conf.yaml"))
    {
        return 0;
    }
    else 
    {
        printf("Entering main event loop.\n");
        gtk_main();
    }
    return 1;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    g_signal_connect( window, "delete-event",
                      G_CALLBACK(delete_event), NULL );
    g_signal_connect( window, "destroy",
                      G_CALLBACK (destroy), NULL );
    gtk_container_set_border_width( GTK_CONTAINER (window), 10 );

    button[0] = gtk_button_new_with_label( "HP 530 A" );
    g_signal_connect( button[0], "clicked",
                      G_CALLBACK (hello), NULL );
    g_signal_connect_swapped( button[0], "clicked",
                              G_CALLBACK (gtk_widget_destroy), window );

    box = gtk_hbox_new( TRUE, 5 );

    button[1] = gtk_button_new_with_label( "HP 530 B" );
    button[2] = gtk_button_new_with_label( "Acer One A" );
    button[3] = gtk_button_new_with_label( "Acer One B" );
    button[4] = gtk_button_new_with_label( "Acer Aspire" );
    button[5] = gtk_button_new_with_label( "UFCW" );

    image[0] = gtk_image_new_from_file( "hp530a.png" );
    image[1] = gtk_image_new_from_file( "hp530b.png" );
    image[2] = gtk_image_new_from_file( "aceronea.png" );
    image[3] = gtk_image_new_from_file( "aceroneb.png" );
    image[4] = gtk_image_new_from_file( "aceroneb.png" );
    image[5] = gtk_image_new_from_file( "aceroneb.png" );

    for(i=0;i<6;i++) {
        gtk_button_set_image( GTK_BUTTON(button[i]), image[i] );
        gtk_button_set_image_position( GTK_BUTTON(button[i]),
                                       GTK_POS_TOP );
        gtk_box_pack_start( GTK_BOX(box), button[i], FALSE, FALSE, 5 );
        gtk_widget_show( button[i] );
    }
    gtk_widget_show( GTK_WIDGET(box) );
    gtk_container_add( GTK_CONTAINER (window), GTK_WIDGET(box) );
    gtk_widget_show( window );
    gtk_main();
    return 0;
}
