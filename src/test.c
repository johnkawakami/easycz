#include "config.h"

void cb( GtkButton *button, GHashTable *hash );

int main( int argc, char *argv[] )
{
    GString *s;
    GString *value;
    GHashTable *command;
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    g_type_init();
    gtk_init( &argc, &argv );
    
    command = g_hash_table_new( g_direct_hash, g_direct_equal );

    s = g_string_new("hello");
    g_hash_table_insert( command, "test", s );
    value = g_hash_table_lookup( command, "test" );
    printf("%s\n", value->str);
    
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_container_set_border_width( GTK_CONTAINER (window), 10 );
    button = gtk_button_new_with_label( "test" );
    g_signal_connect_data( button, "clicked",
                             G_CALLBACK(cb),
                             command,
                             NULL,
                             0 );
    box = gtk_hbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 5 );
    gtk_widget_show( GTK_WIDGET(box) );    
    gtk_widget_show( GTK_WIDGET(button) );    
    gtk_container_add( GTK_CONTAINER (window), GTK_WIDGET(box) );
    gtk_widget_show( window );
    gtk_main();
}

void cb( GtkButton *button, GHashTable *hash )
{
    GString *value;
    value = g_hash_table_lookup( hash, "test" );
    printf("in cb %s\n", value->str);    
}
