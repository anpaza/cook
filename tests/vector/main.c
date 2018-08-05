#include "strvec.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

static const str_t comma = STR_INIT_C (", ");
static const str_t sbo = STR_INIT_C ("[");
static const str_t sbc = STR_INIT_C ("]");
static const str_t null = STR_INIT_C ("*NULL*");

static void test_str (str_t *s)
{
    if (!s)
        return;

    // Test the integrity of the string object
    assert ((s->allocated == 0) || (s->allocated > s->size));
    assert ((s->size == 0) || (s->data != NULL));

    for (unsigned i = 0; i < s->size; i++)
        assert ((s->data [i] != 0) &&
                (s->data [i] >= ' ') &&
                (s->data [i] < 127));
}

static void test_vector (int n, vector_t *vec)
{
    // Test the integrity of the vector object
    assert ((vec->allocated >= vec->size));
    assert ((vec->size == 0) || (vec->data != NULL));

    str_t s;
    str_init (&s);

    for (unsigned i = 0; i < vec->size; i++)
    {
        test_str (vec->data [i]);
        if (s.size)
            str_append (&s, &comma);
        str_append (&s, &sbo);
        str_append (&s, vec->data [i] ? vec->data [i] : &null);
        str_append (&s, &sbc);
    }

    if (s.size < 1000)
        printf ("%d. %s\n", n, str_c (&s));
    else
        printf ("%d. [ok]\n", n);

    str_done (&s);
}

int main ()
{
    int i, j;
    vector_t v1;
    vector_str_init (&v1, 0);

    for (i = 1; i < 10; i++)
    {
        int n = rand ();
        char temp [50];
        snprintf (temp, sizeof (temp), "str <%d>", n);
        assert (vector_insert_sorted (&v1, str_new_c_copy (temp, -1)) >= 0);
    }

    test_vector (1, &v1);

    assert (vector_delete (&v1, 3, 2));
    test_vector (2, &v1);

    vector_t *v2 = vector_str_new (100);

    for (i = 1; i < 1000; i++)
    {
        char temp [64];
        int size = 1 + (rand () & 63);
        for (j = 0; j < size; j++)
        {
            char c = ' ' + (rand () & 63);
            if (c > ' ')
                c += '0' - ' ' - 1;
            if (c > '9')
                c += 'A' - '9' - 1;
            if (c > 'Z')
                c += 'a' - 'Z' - 1;
            temp [j] = c;
        }

        assert (vector_append (v2, str_new_c_copy (temp, size)));
    }

    assert (vector_qsort (v2));
    test_vector (3, v2);

    for (i = 1; i < 675; i++)
    {
        if (!v2->size)
            break;

        int pos = (rand () & INT_MAX) % v2->size;
        int count = rand () & 3;
        if ((pos + count) >= v2->size)
            count = v2->size - pos;
        assert (vector_delete (v2, pos, count));
    }
    test_vector (4, v2);

    vector_free (v2);
    vector_done (&v1);

    str_finalize ();

    printf ("...\nN. profit!\n");

    return 0;
}
