#ifndef qjson_H
#define qjson_H
#ifdef __cplusplus 
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>

const char* qjson_version();

typedef struct
{
	void (*on_parse_error) (void* context, const char* message);
    void (*on_null)        (void* context);
    void (*on_boolean)     (void* context, bool value);
    void (*on_int)         (void* context, int64_t value);
    void (*on_float)       (void* context, double value);
    void (*on_string)      (void* context, const char* value);
    void (*on_list_start)  (void* context);
    void (*on_list_end)    (void* context);
    void (*on_map_start)   (void* context);
    void (*on_map_end)     (void* context);
} qjson_parse_callbacks;

/**
 * Parse a JSON string.
 *
 * @param input The string to parse.
 * @param callbacks The callbacks to call as the parser encounters entities.
 * @param context Pointer to a user-supplied context object that gets passed directly to the callback functions.
 * @return true if parsing was successful.
 */
bool qjson_parse_string(const char* input, const qjson_parse_callbacks* callbacks, void* context);


#ifdef __cplusplus 
}
#endif
#endif // qjson_H
