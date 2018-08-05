#include "useful.h"
#include "diehard/header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SIZE (16 * 1024 * 1024)

int main ()
{
    srand (0xd0decaed);

    int i;
    uint32_t *data;
    data = malloc (SIZE * sizeof (uint32_t));
    for (i = 0; i < SIZE; i++)
        data [i] = rand ();

    uint32_t freq [64];
    memset (freq, 0, sizeof (freq));
    for (i = 0; i < SIZE; i++)
        freq [data [i] & (ARRAY_LEN (freq) - 1)]++;

    uint32_t total = 0;
    for (i = 0; i < ARRAY_LEN (freq); i++)
        total += freq [i];

    assert (total == SIZE);

    // A simple preliminary homemade test

    const int expected_freq = SIZE / ARRAY_LEN (freq);
    // assume < 1% value scattering
    const int max_delta = expected_freq / 100;
    for (i = 0; i < ARRAY_LEN (freq); i++)
    {
        int delta = freq [i] - expected_freq;
        assert ((delta > -max_delta) && (delta < max_delta));
    }

    // Now push it through all circles of hell of the
    // Marsaglia's Diehard Battery of Tests of Randomness.

    static char *fn = "rand.bin";
    FILE *of = fopen (fn, "w");
    assert (of);
    assert (fwrite (data, sizeof (uint32_t), SIZE, of) == SIZE);
    fclose (of);

    puts("\n\t\t\t\tNOTE\n");

    puts("\tMost of the tests in DIEHARD return a p-value, which");
    puts("\tshould be uniform on [0,1) if the input file contains truly");
    puts("\tindependent random bits.   Those p-values are obtained by");
    puts("\tp=1-F(X), where F is the assumed distribution of the sample");
    puts("\trandom variable X---often normal. But that assumed F is often just");
    puts("\tan asymptotic approximation, for which the fit will be worst");
    puts("\tin the tails. Thus you should not be surprised with  occasion-");
    puts("\tal p-values near 0 or 1, such as .0012 or .9983. When a bit");
    puts("\tstream really FAILS BIG, you will get p`s of 0 or 1 to six ");
    puts("\tor more places.  By all means, do not, as a Statistician ");
    puts("\tmight, think that a p < .025 or p> .975 means that the RNG");
    puts("\thas \"failed the test at the .05 level\".  Such p`s happen");
    puts("\tamong the hundreds that DIEHARD produces, even with good RNGs.");
    puts("\t So keep in mind that \"p happens\"\n");


    bday(fn);
    operm5(fn);
    binrnk(fn,"31x31"); binrnk(fn,"32x32");
    binrnk(fn,"6x8");
    bitst(fn);
    monky(fn, "OPSO"); monky(fn, "OQSO"); monky(fn, "DNA");
    cnt1s(fn, "stream");
    cnt1s(fn, "specific");
    park(fn);
    mindist(fn);
    sphere(fn);
    squeez(fn);
    osum(fn);
    runtest(fn);
    craptest(fn);

    return 0;
}
