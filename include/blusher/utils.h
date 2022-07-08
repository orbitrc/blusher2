#ifndef _BL_UTILS_H
#define _BL_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

#include <Qt>

#include <blusher/input.h>

#ifdef __cplusplus
extern "C" {
#endif

double pixel_to_pango_size(double pixel);

double pixel_to_pango_absolute_size(double pixel);

int set_cloexec_or_close(int fd);

int create_tmpfile_cloexec(char *tmpname);

int os_create_anonymous_file(off_t size);

void truncate_fd(int fd, off_t size);

bool point_is_in(double x, double y, double width, double height);

void get_clip_geometry(double parent_clip_x, double parent_clip_y,
        double parent_clip_width, double parent_clip_height,
        double x, double y,
        double width, double height,
        double *clip_x, double *clip_y,
        double *clip_width, double *clip_height);

Qt::MouseButton libinput_button_to_qt_mouse_button(uint32_t libinput_button);

uint32_t qt_mouse_button_to_libinput_button(Qt::MouseButton qt_mouse_button);

bl::Button libinput_btn_to_button(uint32_t libinput_btn);

uint32_t button_to_libinput_btn(bl::Button button);

#ifdef __cplusplus
}
#endif

#endif /* _BL_UTILS_H */
