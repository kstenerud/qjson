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



typedef struct
{
    const uint8_t* const start;
    const uint8_t* const end;
    uint8_t* pos;
    int container_level;
    bool is_inside_map[200];
    bool is_first_entry;
    bool next_object_is_key;
} qjson_encode_context;


/**
 * Create a new context metadata object.
 *
 * @param memory_start The start of the context's memory.
 * @param memory_end The end of the context's memory.
 * @return The new context.
 */
qjson_encode_context qjson_new_encode_context(uint8_t* const memory_start, uint8_t* const memory_end);

/**
 * Add a null value to the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool qjson_add_null(qjson_encode_context* const context);

/**
 * Add a boolean value to the context.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool qjson_add_boolean(qjson_encode_context* const context, const bool value);

/**
 * Add an integer value to the context.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool qjson_add_integer(qjson_encode_context* const context, const int64_t value);

/**
 * Add a floating point value to the context.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool qjson_add_float(qjson_encode_context* const context, const double value);

/**
 * Add a UTF-8 encoded string value to the context.
 * Do not include a byte order marker (BOM)
 *
 * @param context The context to add to.
 * @param str The string to add.
 * @return true if the operation was successful.
 */
bool qjson_add_string(qjson_encode_context* const context, const char* const str);

/**
 * Add a UTF-8 encoded string value to the context.
 * Do not include a byte order marker (BOM)
 *
 * @param context The context to add to.
 * @param start The start of the substring to add.
 * @param end The end of the substring to add.
 * @return true if the operation was successful.
 */
bool qjson_add_substring(qjson_encode_context* const context, const char* const start, const char* const end);

/**
 * Begin a list in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool qjson_start_list(qjson_encode_context* const context);

/**
 * Begin a map in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool qjson_start_map(qjson_encode_context* const context);

/**
 * End the current container in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool qjson_end_container(qjson_encode_context* const context);

/**
 * End the encoding process and ensure the encoded buffer is properly terminated.
 * Any opened lists or maps will be closed, and the encoded buffer will be null terminated.
 *
 * @param context The encoding context.
 * @return A pointer to the end of the encoded buffer. Returns NULL on error.
 */
const char* qjson_end_encoding(qjson_encode_context* const context);


#ifdef __cplusplus
}
#endif
#endif // qjson_H
