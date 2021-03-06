/*
 * Copyright (c) 2014 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif /* STDC_HEADERS */
#ifdef HAVE_STRING_H
# if defined(HAVE_MEMORY_H) && !defined(STDC_HEADERS)
#  include <memory.h>
# endif
# include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */
#if defined(HAVE_STDINT_H)
# include <stdint.h>
#elif defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#endif

#define SUDO_ERROR_WRAP 0

#include "sudo_compat.h"

int hexchar(const char *s);

__dso_public int main(int argc, char *argv[]);

struct hexchar_test {
    char hex[3];
    int value;
};

int
main(int argc, char *argv[])
{
    struct hexchar_test *test_data;
    int i, ntests, result, errors = 0;

    /* Build up test data. */
    ntests = 256 + 256 + 3;
    test_data = calloc(sizeof(*test_data), ntests);
    for (i = 0; i < 256; i++) {
	/* lower case */
	snprintf(test_data[i].hex, sizeof(test_data[i].hex), "%02x", i);
	test_data[i].value = i;
	/* upper case */
	snprintf(test_data[i + 256].hex, sizeof(test_data[i + 256].hex), "%02X", i);
	test_data[i + 256].value = i;
    }
    /* Also test invalid data */
    test_data[ntests - 3].hex[0] = '\0';
    test_data[ntests - 3].value = -1;
    strlcpy(test_data[ntests - 2].hex, "AG", sizeof(test_data[ntests - 2].hex));
    test_data[ntests - 2].value = -1;
    strlcpy(test_data[ntests - 1].hex, "-1", sizeof(test_data[ntests - 1].hex));
    test_data[ntests - 1].value = -1;

    for (i = 0; i < ntests; i++) {
	result = hexchar(test_data[i].hex);
	if (result != test_data[i].value) {
	    fprintf(stderr, "check_hexchar: expected %d, got %d",
		test_data[i].value, result);
	    errors++;
	}
    }
    printf("check_hexchar: %d tests run, %d errors, %d%% success rate\n",
	ntests, errors, (ntests - errors) * 100 / ntests);
    exit(errors);
}
