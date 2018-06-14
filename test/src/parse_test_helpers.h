#ifndef parse_test_helpers_H
#define parse_test_helpers_H
#ifdef __cplusplus
extern "C" {
#endif


#include <qjson/qjson.h>

typedef enum
{
    TYPE_UNSET = 0,
    TYPE_ERROR,
    TYPE_UNEXPECTED,
    TYPE_NULL,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_LIST_START,
    TYPE_LIST_END,
    TYPE_MAP_START,
    TYPE_MAP_END,
} parsed_type;


void parse_reset_all();

int parse_get_item_count();
parsed_type parse_get_type(int index);
const char* parse_get_string(int index);
int64_t parse_get_int(int index);
double parse_get_float(int index);
bool parse_get_bool(int index);
void parse_add_type(parsed_type type);
void parse_add_string(parsed_type type, const char* value);
void parse_add_int(int64_t value);
void parse_add_float(double value);
void parse_add_bool(bool value);

qjson_parse_callbacks parse_new_callbacks();

#ifdef __cplusplus 
}
#endif
#endif // parse_test_helpers_H
