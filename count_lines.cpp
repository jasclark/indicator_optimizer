#include <stdio.h>

int count_lines(FILE* fp)
{
    int                 c;              /* Nb. int (not char) for the EOF */
    int      newline_count = 0;

        /* count the newline characters */
    while ( (c=fgetc(fp)) != EOF ) {
        if ( c == '\n' ) {
            newline_count = newline_count + 1;
        }
    }

    return newline_count;
}
 
