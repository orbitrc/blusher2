#ifndef _BLUSHER_UTILS_H
#define _BLUSHER_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

double pixel_to_pango_size(double pixel);

int set_cloexec_or_close(int fd);

int create_tmpfile_cloexec(char *tmpname);

int os_create_anonymous_file(off_t size);

bool point_is_in(double x, double y, double width, double height);

#endif /* _BLUSHER_UTILS_H */
