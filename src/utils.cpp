#include <blusher/utils.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <linux/input.h>

#include <pango/pango.h>

double pixel_to_pango_size(double pixel)
{
    return (pixel * 0.75) * PANGO_SCALE;
}

double pixel_to_pango_absolute_size(double pixel)
{
    return pixel * PANGO_SCALE;
}

int set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1) {
        return -1;
    }

    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        goto err;
    }

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
        goto err;
    }

    return fd;

err:
    close(fd);
    return -1;
}

int create_tmpfile_cloexec(char *tmpname)
{
    int fd;

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
    if (fd >= 0) {
        unlink(tmpname);
    }
#else
    fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        unlink(tmpname);
    }
#endif

    return fd;
}

int os_create_anonymous_file(off_t size)
{
    const char tpl[] = "/blusher-shared-XXXXXX";    // Template
    const char *path;
    char *name;
    int fd;

    path = getenv("XDG_RUNTIME_DIR");
    // Why this fails??
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = static_cast<char*>(malloc(strlen(path) + strlen(tpl) + 1));

    strcpy(name, path);
    strcat(name, tpl);

    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0) {
        return -1;
    }
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

void truncate_fd(int fd, off_t size)
{
    if (ftruncate(fd, size) < 0) {
        fprintf(stderr, "WARN: ftruncate error!\n");
    }
}

bool point_is_in(double x, double y, double width, double height)
{
    // Negative point means not in the area.
    if (x < 0 || y < 0) {
        return false;
    }
    if (x > width || y > height) {
        return false;
    }
    if (x <= width && y <= height) {
        return true;
    }

    return false;
}

void get_clip_geometry(double parent_clip_x, double parent_clip_y,
        double parent_clip_width, double parent_clip_height,
        double x, double y,
        double width, double height,
        double *clip_x, double *clip_y,
        double *clip_width, double *clip_height)
{
    *clip_x = (x > parent_clip_x) ? x : parent_clip_x - x;
    *clip_y = (y > parent_clip_y) ? y : parent_clip_y - y;

    // If contains, do not clip.
    if ((x + width) < parent_clip_width && (y + height) < parent_clip_height) {
        *clip_width = width;
        *clip_height = height;

        return;
    }

    *clip_width = width - *clip_x;
    *clip_height = height - *clip_y;

    if (*clip_width > parent_clip_width) {
        *clip_width = parent_clip_width;
    }
    if (*clip_height > parent_clip_height) {
        *clip_height = parent_clip_height;
    }
}

Qt::MouseButton libinput_button_to_qt_mouse_button(uint32_t libinput_button)
{
    switch (libinput_button) {
    case BTN_LEFT:
        return Qt::MouseButton::LeftButton;
    case BTN_RIGHT:
        return Qt::MouseButton::RightButton;
    case BTN_MIDDLE:
        return Qt::MouseButton::MiddleButton;
    default:
        return Qt::MouseButton::NoButton;
    }
}

uint32_t qt_mouse_button_to_libinput_button(Qt::MouseButton qt_mouse_button)
{
    switch (qt_mouse_button) {
    case Qt::MouseButton::LeftButton:
        return BTN_LEFT;
    case Qt::MouseButton::RightButton:
        return BTN_RIGHT;
    case Qt::MouseButton::MiddleButton:
        return BTN_MIDDLE;
    default:
        return 0;
    }
}

bl::Button libinput_btn_to_button(uint32_t libinput_button)
{
    switch (libinput_button) {
    case BTN_LEFT:
        return bl::Button::Left;
    case BTN_RIGHT:
        return bl::Button::Right;
    case BTN_MIDDLE:
        return bl::Button::Middle;
    default:
        return bl::Button::None;
    }
}

uint32_t button_to_libinput_btn(bl::Button button)
{
    switch (button) {
    case bl::Button::Left:
        return BTN_LEFT;
    case bl::Button::Right:
        return BTN_RIGHT;
    case bl::Button::Middle:
        return BTN_MIDDLE;
    default:
        return 0;
    }
}

