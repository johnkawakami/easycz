#include <glib.h>
#include <stdio.h>

int main( int argc, char *argv[] )
{
	GString *s;
    GString gs;
    GArray *a;
    int i;

    a = g_array_sized_new( FALSE, FALSE, sizeof(GString), 100 );
    for( i=0; i<100; i++ )
    {
        s = g_string_new( "element" );
        g_string_append_printf( s, ",%d", i );
        g_array_append_val( a, *s );
        printf( "inserting %s\n", s->str );
    }
    gs = g_array_index( a, GString, 40 );
    printf( "retrieving %s\n", gs.str );
}
