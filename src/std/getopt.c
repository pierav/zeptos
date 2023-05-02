#include <stdint.h>
#include <stdio.h>
#include <string.h>

int opterr = 1,      /* if error message should be printed */
    optind = 1,      /* index into parent argv vector */
    optopt = 0,      /* character checked for validity */
    optreset = 0;    /* reset getopt */
char *optarg = NULL; /* argument associated with option */

#define BADCH (int)'?'
#define BADARG (int)':'
#define EMSG ""

int getopt(int argc, char *const argv[], const char *optstring) {
    static char *progname = NULL;
    static char *place = EMSG; /* option letter processing */
    char *oli;                 /* option letter list index */

    if (!progname) {
        progname = *argv;
    }

    if (optreset || !*place) { /* update scanning pointer */
        optreset = 0;
        if (optind >= argc || *(place = argv[optind]) != '-') {
            place = EMSG;
            return EOF;
        }
        if (place[1] && *++place == '-') { /* found "--" */
            ++optind;
            place = EMSG;
            return EOF;
        }
    } /* option letter okay? */
    if ((optopt = (int)*place++) == (int)':' ||
        !(oli = strchr(optstring, optopt))) {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means EOF.
         */
        if (optopt == (int)'-')
            return EOF;
        if (!*place)
            ++optind;
        if (opterr && *optstring != ':')
            fprintf(stderr, "%s: illegal option -- %c\n", progname, optopt);
        return BADCH;
    }
    if (*++oli != ':') { /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++optind;
    } else {        /* need an argument */
        if (*place) /* no white space */
            optarg = place;
        else if (argc <= ++optind) { /* no arg */
            place = EMSG;
            if (*optstring == ':')
                return BADARG;
            if (opterr)
                fprintf(stderr, "%s: option requires an argument -- %c\n",
                        progname, optopt);
            return BADCH;
        } else /* white space */
            optarg = argv[optind];
        place = EMSG;
        ++optind;
    }
    return optopt; /* dump back option letter */
}