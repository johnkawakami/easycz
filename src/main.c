#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "gtk/gtk.h"
#include "config.h"
#include "main.h"

int opt_debug = 0;
int opt_fullscreen = 0;
GString *config_file = NULL;
GString *config_dir = NULL;


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
    struct passwd *pw;
    const gchar *homedir = g_getenv("HOME");;
    FILE *file;

    gtk_init( &argc, &argv );

    /* look for config file in .easycz */
    if (NULL==homedir)
    {
        pw = getpwuid(getuid());
        config_dir = g_string_new( pw->pw_dir );
    }
    else
    {
        config_dir = g_string_new( homedir );
    }
    g_string_append( config_dir, "/.easycz/" );
    config_file = g_string_new( config_dir->str );
    g_string_append( config_file, "conf.yaml" );
    
    while( ( opt = getopt( argc, argv, "c:df" ) ) != -1 )
    {
        switch( opt )
        {
            case 'c': // config file
                if (file = fopen(optarg,"r"))
                {
                    fclose(file);
                    config_file = g_string_new( optarg );
                    g_string_assign( config_dir, g_getenv("PWD") );
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
    /* now check the file location.  if it fails, look for conf.yaml
     * in the current directory. */
    if (opt_debug) printf("finding config in %s\n", config_file->str );
    if (file = fopen( config_file->str, "r" ))
    {
        fclose(file);
    }
    else
    {
        if (opt_debug) printf("could not find the default ~/.easycz/conf.yaml\n");
        if (file = fopen( "conf.yaml", "r" ))
        {
            fclose(file);
            config_file = g_string_new( "conf.yaml" );
            g_string_assign( config_dir, g_getenv("PWD") );
            if (opt_debug) printf("found conf.yaml in current dir \n");
        }
    }
    if ( -1 == g_chdir( config_dir->str ) )
    {
            fprintf( stderr, "cannot chdir to %s\n", config_dir->str );
            exit(0);
    }
    if (opt_debug) printf( "cd to %s\n", config_dir->str );
    if (opt_debug)
    {
        printf("debug\n");
        if (opt_fullscreen)
        {
            if (opt_debug) printf("fullscreen\n");
        }
    }
    
    /* parse the yaml file to build the gui */
    if (!config_parse( config_file->str ))
    {
        return 0;
    }
    else 
    {
        if (opt_debug) printf("Entering main event loop.\n");
        gtk_main();
    }
}
