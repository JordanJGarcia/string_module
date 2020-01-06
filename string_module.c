
#include "string_module.h"

/*********************************************************************/
/*                                                                   */
/*      Function name: build_string                                  */
/*      Return type:   char*                                         */
/*      Parameter(s):  1                                             */
/*          char c: char to add to command                           */
/*          char**: pointer to the string we are appending to.       */
/*                                                                   */
/*      Description:                                                 */
/*          build_command accepts a character and appends it to the  */
/*          string "cmd." It creates a string one letter at a time.  */
/*                                                                   */
/*********************************************************************/
int build_string( char c, char** cmd )
{
    //printf( "cmd = %s\n", *cmd );
    int size = ( *cmd == NULL ? 0 : strlen(*cmd) );

    if ( size > 0 )
    {
        if ( ((*cmd) = (char*)realloc( *cmd, (size + 2) * 
                                        sizeof(char) ) ) == NULL 
           )
            return FAILURE;
    }
    else
    {
        if ( ((*cmd) = (char*)malloc( (size + 2) * sizeof(char) )
             ) == NULL )
            return FAILURE; 
    }
 
    (*cmd)[size] = c;
    (*cmd)[size + 1] = '\0';

    return SUCCESS;
}/* end build_string() */



/*********************************************************************/
/*                                                                   */
/*      Function name: add_string                                    */
/*      Return type:   int                                           */
/*      Parameter(s):                                                */
/*          char** str: pointer to string we are adding              */
/*          char*** arr: pointer to array we are adding string to    */
/*          int* str_count: pointer to number of strings in arr.     */
/*                                                                   */
/*      Description:                                                 */
/*          adds a string to an array of strings.                    */
/*                                                                   */
/*********************************************************************/
int add_string( char** str, char*** arr, int* str_count )
{
    /* if there hasn't been a command initialized */
    if ( *str == NULL )
        return FAILURE;

    /* allocate memory to add new string to array */
    if ( *str_count == 0 )
    {
        if ( (*arr = (char**)malloc( sizeof(char*) ) ) == NULL )
            return FAILURE;
    }
    else
    {
        if ( (*arr = (char**)realloc(*arr, (*str_count + 2) 
                                     * sizeof(char*) ) ) == NULL
           )
            return FAILURE;
    }

    /* allocate memory for the actual string in the array */
    if ( ( (*arr)[*str_count] = (char*)malloc( ( strlen(*str) + 1 )
                                          * sizeof(char) ) ) == NULL
       )
        return FAILURE;

    strcpy( (*arr)[*str_count], *str );

    /* increase command count */
    *str_count += 1;

    /* last element set to NULL for execv() to work */
    (*arr)[*str_count] = NULL;

    /* free up memory */
    free( *str );
    *str = NULL;

    return SUCCESS;
}/* end add_string() */


/*********************************************************************/
/*                                                                   */
/*      Function name: move_strings_down                             */
/*      Return type:   int                                           */
/*      Parameter(s):                                                */
/*          char*** arr: pointer to array we are adjusting.          */
/*          int* arr_size: size of arr.                              */
/*          int n_indices: number of indices we are moving down.     */
/*          int start: starting position of adjustment               */
/*                                                                   */
/*      Description:                                                 */
/*          moves strings from start onwards down n_indices in arr.  */
/*          removes the string *arr[start] from the array.           */
/*                                                                   */
/*********************************************************************/
int move_strings_down( char*** arr, int* arr_size, int add_arr_size, 
                       int start )
{
    /* subtract 1 to account for the 2 NULLs (1 per array) */
    int new_size = *arr_size + add_arr_size - 1;
    int next_elem = *arr_size - 1;
    int last_elem = start + 1; 
    int spaces_to_move = add_arr_size - 1;
    int new_spot; 
    int stop_cleaning_mem = start + add_arr_size;

    /* allocate memory for new array */
    if ( (*arr = (char**) realloc( *arr, (new_size + 1)
                            * sizeof(char*) ) ) == NULL 
       )
        return FAILURE; 

    /* copy all next elements down to new indices */
    for ( ; next_elem >= last_elem; next_elem-- )
    {
        new_spot = next_elem + spaces_to_move;

        /* test if memory is not freed */
        if( new_spot < *arr_size - 1 )
            free( (*arr)[new_spot] );

        (*arr)[new_spot] = (char*) malloc( 
                ( strlen((*arr)[next_elem]) + 1 ) * sizeof(char) ); 

        /* memory allocation error */
        if( (*arr)[new_spot] == NULL )
            return FAILURE; 

        strcpy( (*arr)[new_spot], (*arr)[next_elem] );
    }

    /* clear up old memory and set nulls */
    for ( ; start < stop_cleaning_mem; start++ )
    {
        if ( start < *arr_size )
            free( (*arr)[start] );

        (*arr)[start] = NULL;
    }

    /* set new size & last elem to null */
    *arr_size = new_size; 
    (*arr)[*arr_size] = NULL;

    return SUCCESS; 
} /* end move_strings_down() */


/*********************************************************************/
/*                                                                   */
/*      Function name: add_strings                                   */
/*      Return type:   int                                           */
/*      Parameter(s):                                                */
/*          char*** to: pointer to array we are adjusting.           */
/*          char*** from: pointer to array of strings we are adding. */
/*          int start: index to start placing strings.               */
/*          int n_indices: num indices to copy to.                   */
/*                                                                   */
/*      Description:                                                 */
/*          moves strings from start onwards down n_indices in arr.  */
/*                                                                   */
/*********************************************************************/
int add_strings( char*** to, char*** from, int start, int n_indices )
{
    puts( "In add_strings..." );
    int stop = start + n_indices; 
    int ind = 0;

    /* place strings into arr from start */
    for( ; start < stop; start++ )
    {
        /* allocate memory for new string */
        (*to)[start] = (char*) malloc( (strlen( (*from)[ind] ) + 1) *
                                        sizeof(char) );

        if( (*to)[start] == NULL )
            return FAILURE; 

        /* copy old string into new arr */
        strcpy( (*to)[start], (*from)[ind++] );
    }

    return SUCCESS;
}



/*********************************************************************/
/*                                                                   */
/*      Function name: parse_string                                  */
/*      Return type:   char**                                        */
/*      Parameter(s):  1                                             */
/*          char* line: line of commands user types in               */
/*          char*** cmds: array to place strings in.                 */
/*          int* n_cmds: pointer to length of array cmds.            */ 
/*                                                                   */
/*      Description:                                                 */
/*          This function splits a string into an array              */
/*          of strings.                                              */
/*                                                                   */
/*********************************************************************/
int parse_string( char* line, char*** cmds, int* n_cmds )
{
    int line_size = strlen( line );
    char* cmd = NULL;

    for( int i = 0; i < line_size; i++ )
    {
        /* special characters to watch out for */
        if ( line[i] == '$' || line[i] == '|' || line[i] == '<' || 
             line[i] == '>' || line[i] == '&' || line[i] == '?' ||
             line[i] == '!' || line[i] == ',' || line[i] == '=' || 
             line[i] == ':'
           )
        {
            //puts( "Found special character!" );
            if( cmd != NULL )
                add_string( &cmd, cmds, n_cmds );

            build_string( line[i], &cmd );
            add_string( &cmd, cmds, n_cmds );
        }
        else if ( i == line_size - 1 ) /* end of line */
        {
            if( !isspace( line[i] ) )
                build_string( line[i], &cmd );

            add_string( &cmd, cmds, n_cmds );
        }
        else if ( line[i] == '\"' || line[i] == '\'' ) /* string in quotes */
        {
            char term = line[i++];

            /* set quote marker */
            if ( *n_cmds > 0 && strcmp( (*cmds)[0], "alias" ) != 0 )
            {
                build_string( '*', &cmd );
                build_string( '*', &cmd );
            }

            /* build command with everything inside quotes */
            do
            {
                /* if user forgot end quote, continue */
                if ( i == line_size - 1 )
                {
                    build_string( line[i], &cmd );
                    break;
                } 
                build_string( line[i++], &cmd );
            } while( line[i] != term );

            add_string( &cmd, cmds, n_cmds );
        }
        else if ( isspace( line[i] ) ) /* spacing */
        {
            if ( cmd != NULL )
                add_string( &cmd, cmds, n_cmds );

            /* if more than one space */
            while ( isspace( line[i + 1] ) )
                build_string( line[i++], &cmd );

            if ( cmd != NULL )
                add_string( &cmd, cmds, n_cmds );
        }
        else /* everything else */
            build_string( line[i], &cmd ); 
    }
    return SUCCESS;
} /* end split_input() */
