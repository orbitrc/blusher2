#ifndef _BL_UTILS_H
#define _BL_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

double pixel_to_pango_size(double pixel);

int set_cloexec_or_close(int fd);

int create_tmpfile_cloexec(char *tmpname);

int os_create_anonymous_file(off_t size);

bool point_is_in(double x, double y, double width, double height);

void get_clip_geometry(double parent_clip_x, double parent_clip_y,
        double parent_clip_width, double parent_clip_height,
        double x, double y,
        double width, double height,
        double *clip_x, double *clip_y,
        double *clip_width, double *clip_height);

#ifdef __cplusplus
}
#endif

#endif /* _BL_UTILS_H */
