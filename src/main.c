#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "gtk/gtk.h"
#include "config.h"
#include "main.h"

int opt_debug = 0;
int opt_fullscreen = 0;
GString *config_file[3];
gchar *config_dir = NULL;


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
    int i;
    int opt;
    struct passwd *pw;
    const gchar *homedir = g_getenv("HOME");;
    FILE *file;
    GString *active_config_file;
    
    gtk_init( &argc, &argv );

    config_file[0] = NULL;
    config_file[1] = NULL;
    config_file[2] = NULL;

    /* make up paths to search: ~/.easycz */
    if (NULL==homedir)
    {
        config_file[0] = g_string_new( g_get_home_dir() );
    }
    else
    {
        config_file[0] = g_string_new( homedir );
    }
    g_string_append( config_file[0], "/.easycz/conf.yaml" );

    /* search in current dir for conf.yaml */
    config_file[1] = g_string_new( g_get_current_dir() );
    g_string_append( config_file[1], "/conf.yaml" );

    
    while( ( opt = getopt( argc, argv, "c:df" ) ) != -1 )
    {
        switch( opt )
        {
            case 'c': // config file
                if (file = fopen(optarg,"r"))
                {
                    fclose(file);
                    config_file[2] = g_string_new( g_get_current_dir() );
                    g_string_append( config_file[2], "/" );
                    g_string_append( config_file[2], optarg );
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
    if (opt_debug) printf("finding config\n" );
    active_config_file = NULL;
    for( i=2; i>=0; i--)
    {
        if ( config_file[i] && ( 0 == access( config_file[i]->str, R_OK) ) )
        {
            active_config_file = config_file[i];
        }
    }
    if ( NULL == active_config_file )
    {
        fprintf( stderr, "Config file not found.\n" );
        exit(1);
    }

    config_dir = g_path_get_dirname( active_config_file->str );
    
    if ( -1 == g_chdir( config_dir ) )
    {
            fprintf( stderr, "cannot chdir to %s\n", config_dir );
            exit(0);
    }
    if (opt_debug) printf( "cd to %s\n", config_dir );
    if (opt_debug)
    {
        printf("debug\n");
        if (opt_fullscreen)
        {
            if (opt_debug) printf("fullscreen\n");
        }
    }
    
    /* parse the yaml file to build the gui */
    if (!config_parse( active_config_file->str ))
    {
        return 0;
    }
    else 
    {
        if (opt_debug) printf("Entering main event loop.\n");
        gtk_main();
    }
}
