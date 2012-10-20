/*
 * Config format.  Format is a CSV file.
 *
 * button,"button name","image",1,1,"shell command"
 * linebreak
 * button,"name","image",1,1,"shell command"
 */
#include <glib.h>
#include <gtk/gtk.h>
#include <yaml.h>

typedef struct config_s {
    yaml_parser_t *parser;
} config_t;

typedef struct command_s {
    GString *path;
} command_t;

// a singleton that holds the config object
static config_t configobj;
static config_t *config = &configobj;
