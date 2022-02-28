#include <blusher/utils.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <pango/pango.h>

double pixel_to_pango_size(double pixel)
{
    return (pixel * 0.75) * PANGO_SCALE;
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
