/*
 * Config format.  Format is a CSV file.
 *
 * button,"button name","image",1,1,"shell command"
 * linebreak
 * button,"name","image",1,1,"shell command"
 */
#include <glib.h>
#include <gtk/gtk.h>
#include "globals.h"
#include <yaml.h>

typedef struct config_s {
    yaml_parser_t *parser;
    GtkWidget *windows[MAX_WINDOWS]; // store all windows here
    gint windowcounter;
} config_t;

// a singleton that holds the config object
static config_t configobj;
static config_t *config = &configobj;
