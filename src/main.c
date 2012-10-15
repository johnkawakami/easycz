/*
#include <config.h>
#include <signal.h>
#include <stdlib.h>
#include <glib/glib.h>
*/

#include "gtk/gtk.h"
#include "config.h"
#include "main.h"

gboolean delete_event( GtkWidget *widget,
                       GdkEvent *event,
                       gpointer data )
{
    g_print("delete event\n");
    return FALSE;
}

void destroy( GtkWidget *widget, 
                     gpointer data )
{
    gtk_main_quit();
}

int main( int argc, char *argv[] )
{
    GtkWidget *window;
    GtkWidget *button[6];
    GtkWidget *box;
    GtkWidget *image[6];
    int i;


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
}
