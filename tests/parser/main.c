#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_RCP "tests/tokenizer/test.rcp"

bool load (const char *fn, str_t *str)
{
    FILE *inf = fopen (fn, "r");
    if (!inf)
        return false;

    fseek (inf, 0, SEEK_END);
    long fsize = ftell (inf);
    fseek (inf, 0, SEEK_SET);

    if (fsize < 0)
    {
        fclose (inf);
        return false;
    }

    char *buf = malloc (fsize);
    if (fread (buf, 1, fsize, inf) != fsize)
    {
        fclose (inf);
        return false;
    }

    str_init_c_prealloc (str, buf, fsize);
    fclose (inf);
    return true;
}

int main (int argc, const char **argv)
{
    const char *fn;
    input_t in;

    input_init (&in);

    if (argc < 2)
        fn = TEST_RCP;
    else
        fn = argv [1];

    if (!load (fn, &in.text))
    {
        fprintf (stderr, "Failed to load input file '%s'\n", fn);
        return -1;
    }

    token_t tok;
    while (input_token (&in, &tok))
    {

        str_done (&tok.text);
    }

    input_done (&in);

    str_finalize ();
    printf ("\nDone!\n");

    return 0;
}
