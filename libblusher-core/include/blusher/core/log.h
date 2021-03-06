#ifndef _BLUSHER_CORE_LOG_H
#define _BLUSHER_CORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bl_log_level {
    BL_LOG_LEVEL_INFO = 0,
    BL_LOG_LEVEL_WARN = 1,
    BL_LOG_LEVEL_ERROR = 2,
} bl_log_level;

void bl_log(bl_log_level level, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _BLUSHER_CORE_LOG_H */
