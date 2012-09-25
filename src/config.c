#include "config.h"

/* private methods */
gboolean config_read_document();
gboolean config_read_document_mapping();
gboolean config_read_table();
gboolean config_read_table_rows();
gboolean config_read_icons();
gboolean config_read_icon();
void parser_error_handler( yaml_parser_t *parser );
gboolean config_next_scalar_value( GString *value );
gboolean run_command( GtkButton *button, GHashTable *command );

/* helper to print error messages */
char * stringify_event_type( yaml_event_t *event )
{
    static char buffer[1024];
    switch (event->type) 
    {
        case YAML_STREAM_START_EVENT:
            return "YAML_STREAM_START_EVENT";
            break;
        case YAML_STREAM_END_EVENT:
            return "YAML_STREAM_END_EVENT";
            break;
        case YAML_DOCUMENT_START_EVENT:
            return "YAML_DOCUMENT_START_EVENT";
            break;
        case YAML_DOCUMENT_END_EVENT:
            return "YAML_DOCUMENT_END_EVENT";
            break;
        case YAML_ALIAS_EVENT:
            return "YAML_ALIAS_EVENT";
            break;
        case YAML_SCALAR_EVENT:
            sprintf( buffer, "SCALAR_EVENT anchor %s, tag %s, value %s",
                event->data.scalar.anchor,
                event->data.scalar.tag,
                event->data.scalar.value) ;
            return buffer;
            break;
        case YAML_SEQUENCE_START_EVENT:
            sprintf( buffer, "YAML_SEQUENCE_START_EVENT, tag %s",
                event->data.sequence_start.tag);
            return buffer;
            break;
        case YAML_SEQUENCE_END_EVENT:
            return "YAML_SEQUENCE_END_EVENT";
            break;
        case YAML_MAPPING_START_EVENT:
            return "YAML_MAPPING_START_EVENT";
            break;
        case YAML_MAPPING_END_EVENT:
            return "YAML_MAPPING_END_EVENT";
            break;
        case YAML_NO_EVENT:
            return "YAML_NO_EVENT";
            break;
        default:
            return "default";
            break;
    }
}

void parser_error_handler( yaml_parser_t *parser )
{
    /* Display a parser error message. */

    switch (parser->error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser->problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", parser->problem,
                        parser->problem_value, (int) parser->problem_offset);
            }
            else {
                fprintf(stderr, "Reader error: %s at %d\n", parser->problem,
                        (int) parser->problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser->context) {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser->context,
                        (int) parser->context_mark.line+1, (int) parser->context_mark.column+1,
                        parser->problem, (int) parser->problem_mark.line+1,
                        (int) parser->problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        parser->problem, (int) parser->problem_mark.line+1,
                        (int) parser->problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser->context) {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser->context,
                        (int) parser->context_mark.line+1, (int) parser->context_mark.column+1,
                        parser->problem, (int) parser->problem_mark.line+1,
                        (int) parser->problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        parser->problem, (int) parser->problem_mark.line+1,
                        (int) parser->problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }
}

gboolean config_parse_next_event( yaml_event_t *event )
{
    memset( event, 0, sizeof(*event) );
    if ( yaml_parser_parse( config->parser, event ) == FALSE )
    {
        fprintf(stderr, "Memory error: Not enough memory for creating an event\n");
        yaml_event_delete(event);
        yaml_parser_delete(config->parser);
        return FALSE;
    }
    // printf( "%d ", (int) parser->offset );
    return TRUE;
}
/**
 * Expecting a scalar, and return the value.  If it's not a scalar,
 * returns FALSE.  
 */
gboolean config_next_scalar_value( GString *value )
{
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;
    
    config_parse_next_event( event );
    if ( event->type != YAML_SCALAR_EVENT )
    {
        fprintf( stderr,
                 "Expected a scalar. \nLine: %d.\n",
                 (int) event->end_mark.line );
        return FALSE;
    }
    else
    {
        printf("config_next_scalar_value read: %s\n", event->data.scalar.value );
        g_string_assign( value, event->data.scalar.value );
        return TRUE;
    }
}

/*
 * This is a generic template for a loop to parse a sequence that
 * contains mappings.  The outline of the logic is:
 * parse events
 *    if exit case, exit successfully
 *    if drill object case, drill down
 *    otherwise return an error
 * fallthrough here, and return an error
 *
 * The idea is that you loop, and exit only when you reach the matching
 * YAML_*_END_EVENT.  If you reach an object that should be parsed by
 * another function - a drilldown, you call the next parser.  Don't
 * return the returned value; you can use the return value but this loop
 * should exit only when the above YAML_*_END_EVENT is reached.
 * Use the return value only to check for an error, and return FALSE.
 *
 * Otherwise, return FALSE, which is the error value.
 * 
 * Note that you have only one return TRUE, but multiple return FALSEs.
 * You return TRUE only if the parsing is successful.
gboolean config_read_sequence()
{
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;

    while( config_parse_next_event( event ) )
    {
        if ( event->type == YAML_SEQUENCE_END_EVENT )
        {
            return TRUE;
        }
        else if ( event->type == YAML_MAPPING_START_EVENT )
        {
            if (!config_read_mapping( hbox ))
            {
                fprintf( stderr, "Reading a mapping failed.\n");
                exit FALSE;
            }
        }
        else // error 
        {
            fprintf( stderr, "Expected a mapping for...\n" );
            return FALSE;
        }
    }
    // should not get here 
    return FALSE;
}
 * Parsing a mapping is similar, except you're looking for the
 * YAML_MAPPING_END_EVENT for the exit, and loop over scalars.
 * You have to use strcmp to match the strings.  The utility function
 * config_next_scalar_value reads the next scalar into a GString.
 * See config_read_icon for the canonical example.
 *
 * The issue with mappings is that the sequence shouldn't matter.
 * It's like an object and properties.  The easy way is to
 * define all the properties as GStrings, and read the scalar values
 * into these GStrings.  When the YAML_MAPPING_END_EVENT is reached,
 * convert these values into the object you're creating.  Then put the
 * object into the relevant collection.  (Don't return it as the return
 * value.  You should use the return value to indicate errors or success.)
 */

int config_parse( char *filename )
{
    FILE *fh = fopen( filename, "r");
    yaml_event_t eventobj;
    yaml_parser_t* parser;
    yaml_event_t* event = &eventobj;
    int done = 0;   

    memset( parser, 0, sizeof(parser) );
    memset( config, 0, sizeof(config) );

    config->parser = malloc(sizeof(yaml_parser_t));
    parser = config->parser;

    /* Initialize parser */
    if(!yaml_parser_initialize(parser))
        fputs("Failed to initialize parser!\n", stderr);
    if(fh == NULL)
        fputs("Failed to open file!\n", stderr);

    /* Set input file */
    yaml_parser_set_input_file(parser, fh);

    /* BEGIN new code */
    if (!yaml_stream_start_event_initialize( event, YAML_UTF8_ENCODING ))
    {
        parser_error_handler( parser );
        yaml_event_delete(event);
        yaml_parser_delete(parser);
        return 0;
    }

    /* A stream can contain multiple documents, separated by lines with
     * three dashes (---).
     */

    while (yaml_parser_parse( parser, event )) 
    {
		if (event->type == YAML_STREAM_END_EVENT) { /* exit event */
            yaml_event_delete(event);
            yaml_parser_delete(parser);
            fclose(fh);
            printf("Finished reading stream.\n");
            return TRUE;
        }
		else if (event->type == YAML_DOCUMENT_START_EVENT) 
		{
            /* drill down */
            config_read_document();
        }
        else if (event->type == YAML_NO_EVENT)
        {
            fprintf( stderr, "No Event???\n");
            yaml_event_delete( event );
        }
        else /* error, unexpected event */
        {
        }
	}
    /* we should never get here */
    return FALSE;
}

/* each document creates a new window */
gboolean config_read_document()
{
    GtkWidget *window;
    GString *value;
    gint tables = 0;
    gboolean istable = FALSE;
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;

    printf("read document\n");

    value = g_string_new("");

    /* every document populates a single window */
    if (window = gtk_window_new( GTK_WINDOW_TOPLEVEL ))
    {
    }
    else
    {
        fprintf( stderr, "Could not allocate a window.\n");
    }

    while (config_parse_next_event( event ))
    {
        if (event->type == YAML_DOCUMENT_END_EVENT)
        {
            // show window when document is complete
            printf( "ended the document.");
            gtk_container_set_border_width( GTK_CONTAINER (window), 10 );
            gtk_widget_show( window ); 
            return TRUE;
        }
        else if ( event->type == YAML_MAPPING_START_EVENT )
        {
            if ( !config_read_document_mapping(window) )
            {
                fprintf( stderr, "Problem reading the mapping in the document.\n");
                return FALSE;
            }
        }
        else /* error */
        {
            fprintf( stderr, "Unsupported event type, %s, in read document.\n", stringify_event_type(event) );
            fprintf( stderr, "Line: %d\n", (int) event->end_mark.line );
            return FALSE;
        }
    } // while
    /* end of the mapping */
    printf("fell out of event loop");
    return FALSE;
}

/**
 * Reads the mapping within the document.  This avoids the tangle of
 * code required to correctly reach the YAML_END_DOCUMENT_EVENT.
 */
gboolean config_read_document_mapping( GtkWidget *window )
{
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;
    GString *value;

    value = g_string_new("");

    while (config_parse_next_event( event ))
    {
        if (event->type == YAML_MAPPING_END_EVENT )
        {
            printf("yaml mapping end");
            return TRUE;
        }
        else if (event->type == YAML_SCALAR_EVENT )
        {
            /* expect either "title" or "table" */
            /* if "title" set the window's title */
            printf("scalar value %s\n", event->data.scalar.value);
            if ( 0==strcmp( "title", event->data.scalar.value ) )
            {
                if (config_next_scalar_value( value ))
                {
                    printf("setting title to %s\n", value->str);
                    gtk_window_set_title( GTK_WINDOW(window), value->str );
                }
                else
                {
                    fprintf( stderr, "-title must be followed by a string.\n");
                    return FALSE;
                }
            }
            else if ( 0==strcmp( "table", event->data.scalar.value ) )
            {
                printf("starting a table\n");
                // fixme add code to restrict number of tables
                if (config_read_table( window ))
                {
                    printf("read the table ok.\n");
                }
                else
                {
                    fprintf( stderr, "Table didn't end correctly.\n");
                    return FALSE;
                }
            }
        }
        else if (event->type == YAML_NO_EVENT )
        {
            fprintf( stdout, "No Event... usually means a syntax error.\n");
        }
        else /* error */
        {
            fprintf( stderr, "Unsupported event type, %s, in read document.\n", stringify_event_type(event) );
            fprintf( stderr, "Line: %d\n", (int) event->end_mark.line );
            return FALSE;
        }
    } // while
    /* shouldn't get here */
    return FALSE;
}

/* A table is a sequence of mappings.  Each mapping is like a row.
 * A row can be either a "title" or "icons".  The data should look
 * like:
 * table:
 *   -title: A title row
 *   -icons:
 *       # a sequence of mappings
 *   -title: Another title row
 *  */
gboolean config_read_table( GtkWidget *window )
{
    GtkWidget *vbox;
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;

    /* a table corresponds to a vertical stack of widgets */
    vbox = gtk_vbox_new( FALSE, BUTTON_SPACING );
    gtk_container_add( GTK_CONTAINER(window), vbox ); 
    gtk_widget_show( vbox );
    
    config_parse_next_event( event );
    /* expect a sequence */
    if ( event->type != YAML_SEQUENCE_START_EVENT )
    {
        fprintf( stderr, "A table must be a sequence.\n" );
        fprintf( stderr, "event type = %s\n", stringify_event_type(event) );        
        return FALSE;
    }

    while(config_parse_next_event( event ))
    {
        if ( event->type == YAML_SEQUENCE_END_EVENT ) /* end of the table */
        {
            return TRUE;
        }
        else if ( event->type == YAML_MAPPING_START_EVENT )
        {
            config_read_table_rows( vbox );
        }
        else /* error */
        {
            fprintf( stderr, "Expecting mapping or end of sequence.\n");
        }
    }
    /* should not get here */
    return FALSE;
}

gboolean config_read_table_rows( GtkWidget *vbox )
{
    GtkWidget *label;
    GtkWidget *hbox;
    GString *value;
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;

    value = g_string_new("");
    
    while(config_parse_next_event( event ))
    {        
        /* a mapping start event brought us here so we look for an end
         * event to exit */
        if (event->type == YAML_MAPPING_END_EVENT)
        {
            return TRUE;
        }
        else if ( event->type == YAML_SCALAR_EVENT )
        {
            printf("config_read_table_rows scalar: %s\n",
                   event->data.scalar.value);
            if ( 0==strcmp( "title", event->data.scalar.value ) )
            {
                if (config_next_scalar_value( value ))
                {
                    label = gtk_label_new( value->str );
                    gtk_misc_set_alignment( GTK_MISC(label),
                                            (gfloat) 0,
                                            (gfloat) 0 );
                    // gtk_label_set_text( label, event->data.scalar.value );
                    hbox = gtk_hbox_new( FALSE, 0 );
                    gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(hbox),
                                        FALSE, TRUE, 5 );
                    gtk_box_pack_start( GTK_BOX(hbox), GTK_WIDGET(label),
                                        TRUE, TRUE, 5 );
                    gtk_widget_show( hbox );
                    gtk_widget_show( label );
                }
                else
                {
                    fprintf( stderr, "Value for title is not a scalar.\n" );
                    return FALSE;
                }
            }
            else if ( 0==strcmp( "icons", event->data.scalar.value ) )
            {
                config_parse_next_event( event );
                if ( event->type == YAML_SEQUENCE_START_EVENT )
                {
                    hbox = gtk_hbox_new( FALSE, 0 );
                    gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(hbox),
                                        FALSE, TRUE, 5 );
                    // parse a list of icons
                    config_read_icons( hbox );
                    gtk_widget_show( hbox );
                }
                else /* error */
                {
                    fprintf( stderr, "Expected a sequence for icons.\n" );
                    return FALSE;
                }
            }
            else /* error */
            {
                fprintf( stderr, "Expected \"-title\" or \"-icons\".\n" );
                return FALSE;
            }
        } // YAML_SCALAR_EVENT
    } // while
    /* should not get here */
    return FALSE;
}

gboolean config_read_icons( GtkWidget *hbox )
{
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;

    printf("Reading icons\n");
    while( config_parse_next_event( event ) )
    {
        if ( event->type == YAML_SEQUENCE_END_EVENT )
        {
            return TRUE;
        }
        else if ( event->type == YAML_MAPPING_START_EVENT )
        {
            config_read_icon( hbox );
        }
        else /* error */
        {
            fprintf( stderr, "Expected a mapping for an icon.\n" );
            return FALSE;
        }
    }
    /* should not get here */
    return FALSE;
}

gboolean config_read_icon( GtkWidget *hbox )
{
    yaml_event_t eventobj;
    yaml_event_t *event = &eventobj;
    GString *title = NULL;
    GString *icon = NULL;
    GString *path = NULL;
    GtkWidget *image = NULL;
    GtkWidget *button = NULL;
    GHashTable *command = NULL;
    
    command = g_hash_table_new( g_direct_hash, g_direct_equal );
  
    printf("Reading an icon\n");
    while( config_parse_next_event( event ) )
    {
        if ( event->type == YAML_MAPPING_END_EVENT )
        {
            // add the widget to the container
            printf( "adding icon: %s %s %s\n", title->str,
                                               icon->str,
                                               path->str );
            button = gtk_button_new_with_label( title->str );
            image = gtk_image_new_from_file( icon->str );
            gtk_button_set_image( GTK_BUTTON(button), image );
            gtk_button_set_image_position( GTK_BUTTON(button),
                                           GTK_POS_TOP );
            g_signal_connect_data( button, "clicked",
                      G_CALLBACK(run_command), command, NULL, 0 );
            gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 5 );
            gtk_widget_show( GTK_WIDGET(button) );
            /*
              gtk_image_new_from_file( "hp530a.png" );
              gtk_button_set_image( GTK_BUTTON(button[i]), image[i] );
              gtk_button_set_image_position( GTK_BUTTON(button[i]),
                                       GTK_POS_TOP );
              gtk_box_pack_start( GTK_BOX(box), button[i], FALSE, FALSE, 5 );
              gtk_widget_show( button[i] );
              gtk_widget_show( GTK_WIDGET(box) );
              gtk_container_add( GTK_CONTAINER (window), GTK_WIDGET(box) );
            */
        
            return TRUE;
        }
        else if ( event->type == YAML_SCALAR_EVENT )
        {
            printf( "config_read_icon scalar: %s\n", event->data.scalar.value );
            if ( 0 == strcmp( "title", event->data.scalar.value ) )
            {
                title = g_string_new("");
                config_next_scalar_value( title );
            }
            else if ( 0 == strcmp( "icon", event->data.scalar.value ) )
            {
                icon = g_string_new("default icon value");
                config_next_scalar_value( icon );
            }
            else if ( 0 == strcmp( "command", event->data.scalar.value ) )
            {
                path = g_string_new("");
                config_next_scalar_value( path );
                g_hash_table_insert( command, "path", path );
            }
        }
        else /* error */
        {
            fprintf( stderr, "Expected a mapping for an icon.\n" );
            return FALSE;
        }
    }
    /* should not get here */
    return FALSE;
}

gboolean run_command( GtkButton *button, GHashTable *command )
{
    GString *path;
    path = g_hash_table_lookup( command, "path" );
    if (path != NULL)
        printf("%s\n", path->str );
}
