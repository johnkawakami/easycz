#include <unistd.h>
#include "gtk/gtk.h"
#include "config.h"
#include "main.h"

int opt_debug = 0;
int opt_fullscreen = 0;

gboolean delete_event( GtkWidget *widget,
                       GdkEvent *event,
                       gpointer data )
{
    if (opt_debug) g_print("delete event\n");
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
    char *config_file = "conf.yaml";
    FILE *file;

    gtk_init( &argc, &argv );

    while( ( opt = getopt( argc, argv, "c:df" ) ) != -1 )
    {
        switch( opt )
        {
            case 'c': // config file
                if (file = fopen(optarg,"r"))
                {
                    fclose(file);
                    config_file = optarg;
                }
                else
                {
                    printf("%s cannot be opened or does not exist\n", optarg);
                    exit(0);
                }
                // user specifies alternate config file
                break;
            case 'd':
                opt_debug = 1;
                break;
            case 'f':
                opt_fullscreen = 1;
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
            if (opt_debug) printf("fullscreen\n");
        }
    }
    
    /* parse the yaml file to build the gui */
    if (!config_parse( config_file ))
    {
        return 0;
    }
    else 
    {
        if (opt_debug) printf("Entering main event loop.\n");
        gtk_main();
    }
}
