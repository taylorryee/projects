
#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int size = 0;
    const char *cachePolicy = "-F";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-N") == 0) {

            if (i + 1 < argc) {
                size = atoi(argv[i + 1]);
                i++;
            }
        } else {

            if (strcmp(argv[i], "-F") == 0 || strcmp(argv[i], "-L") == 0
                || strcmp(argv[i], "-C") == 0) {
                cachePolicy = argv[i];
            }
        }
    }

    if (size <= 0) {
        fprintf(stderr, "Warning: Cache size must be a positive integer.\n");
        return 1;
    }

    List cache = new_list(size);

    char *item = (char *) malloc(100 * sizeof(char));

    while (fgets(item, 100, stdin) != NULL) {
        if (strcmp(cachePolicy, "-F") == 0) {
            if (f_add(cache, item) == 0) {
                fprintf(stdout, "MISS\n");
            } else {
                fprintf(stdout, "HIT\n");
            }
        } else if (strcmp(cachePolicy, "-L") == 0) {
            if (l_add(cache, item) == 0) {

                fprintf(stdout, "MISS\n");
            } else {

                fprintf(stdout, "HIT\n");
            }
        } else {
            if (c_add(cache, item) == 0) {

                fprintf(stdout, "MISS\n");

            } else {

                fprintf(stdout, "HIT\n");
            }
        }

        item = (char *) malloc(100 * sizeof(char));
    }

    free(item);
    fprintf(stdout, "%d %d\n", co_count(cache), ca_count(cache));

    return 0;
}

