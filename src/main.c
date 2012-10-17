#include <unistd.h>
#include "gtk/gtk.h"
#include "config.h"
#include "main.h"

gboolean delete_event( GtkWidget *widget,
                       GdkEvent *event,
                       gpointer data )
{
    g_print("delete event\n");
    return FALSE;  // FALSE causes destroy even to fire
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
    int opt;

    gtk_init( &argc, &argv );

    while( ( opt = getopt( argc, argv, "df" ) ) != -1 )
    {
        switch( opt )
        {
            case 'd':
                opt_debug = TRUE;
                break;
            case 'f':
                opt_fullscreen = TRUE;
                break;
            default:
                break;
        }
    }
    if (opt_debug)
    {
        printf("debug\n");
        if (opt_fullscreen)
        {
            printf("fullscreen\n");
        }
    }
    
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
