#include "str.h"

#include <stdio.h>
#include <assert.h>

static void test_str (int n, str_t *s)
{
    // Test the integrity of the string object
    assert ((s->allocated == 0) || (s->allocated > s->size));
    assert ((s->size == 0) || (s->data != NULL));

    for (unsigned i = 0; i < s->size; i++)
        assert (s->data [i] != 0);

    printf ("%d. [%s]\n", n, str_c (s));
    fflush (stdout);
}

int main ()
{
    int i;

    str_t *hello = str_new_c_const ("Hello, world!", -1);

    test_str (1, hello);

    str_t *pain = str_new_c_copy (", full of pain", -1);
    assert (str_insert (hello, 12, pain));

    test_str (2, hello);

    str_t *copy = str_new_copy (hello);
    test_str (3, copy);

    str_t gory;
    str_init_c_const (&gory, " gory", 5);
    str_insert (copy, 6, &gory);

    str_t exclam;
    str_init_c_copy (&exclam, "!!", -1);
    str_append (copy, &exclam);
    test_str (4, copy);

    str_t goodbye = STR_INIT_C ("Goodbye");
    str_replace (copy, 0, 5, &goodbye);
    str_delete (copy, 33, 2);
    test_str (5, copy);

    // print the originally shared string
    test_str (6, hello);

    str_t words [4];
    str_init_substr (&words [0], hello, 0, 5);
    str_init_substr (&words [1], hello, 7, 5);
    str_init_substr (&words [2], hello, 14, 4);
    str_init_substr (&words [3], hello, 22, 5);

    str_free (hello);

    for (i = 0; i < 4; i++)
        test_str (7 + i, &words [i]);

    str_done (&goodbye);
    str_done (&exclam);
    str_done (&gory);
    str_free (copy);
    str_free (pain);

    for (i = 0; i < 4; i++)
        str_done (&words [i]);

    str_finalize ();

    printf ("...\nN. profit!\n");

    return 0;
}
