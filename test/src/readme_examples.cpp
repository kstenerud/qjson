#include <gtest/gtest.h>
#include <qjson/qjson.h>
#include <stdio.h>

TEST(QJson_Examples, encode)
{
    int indent_spaces = 4;
    uint8_t buffer[1000];
    qjson_encode_context context = qjson_new_encode_context_with_config(buffer,
                                                                        buffer + sizeof(buffer),
                                                                        indent_spaces,
                                                                        DEFAULT_FLOAT_DIGITS_PRECISION);

    qjson_start_map(&context);
    qjson_add_string(&context, "null");
    qjson_add_null(&context);
    qjson_add_string(&context, "one");
    qjson_add_integer(&context, 1);
    qjson_add_string(&context, "list");
        qjson_start_list(&context);
        qjson_add_integer(&context, 1);
        qjson_add_integer(&context, 2);
        qjson_add_integer(&context, 3);
            qjson_start_map(&context);
            qjson_add_string(&context, "a");
            qjson_add_integer(&context, 1);
            qjson_add_string(&context, "b");
            qjson_add_integer(&context, 2);
            qjson_add_string(&context, "c");
            qjson_add_integer(&context, 3);
            qjson_end_container(&context);
        qjson_end_container(&context);
    qjson_add_string(&context, "true");
    qjson_add_boolean(&context, true);
    qjson_end_container(&context);
    qjson_end_encoding(&context);

    printf("%s\n", buffer);
}

typedef struct
{
    int depth;
} json_parse_context;

static void indent(json_parse_context* context)
{
    for(int i = 0; i < context->depth; i++)
    {
        printf("  ");
    }
}

static void on_parse_error(void* context, const char* message)
{
    printf("Error: %s\n", message);
}

static void on_string(void* context, const char* str)
{
    indent((json_parse_context*)context);
    printf("\"%s\"\n", str);
}

static void on_null(void* context)
{
    indent((json_parse_context*)context);
    printf("null\n");
}
    
static void on_bool(void* context, bool value)
{
    indent((json_parse_context*)context);
    printf(value ? "true\n" : "false\n");
}
    
static void on_int(void* context, int64_t value)
{
    indent((json_parse_context*)context);
    printf("%ld\n", value);
}
    
static void on_float(void* context, double value)
{
    indent((json_parse_context*)context);
    printf("%lg\n", value);
}

static void on_list_start(void* context)
{
    indent((json_parse_context*)context);
    printf("[\n");
    ((json_parse_context*)context)->depth++;
}
    
static void on_list_end(void* context)
{
    ((json_parse_context*)context)->depth--;
    indent((json_parse_context*)context);
    printf("]\n");
}
    
static void on_map_start(void* context)
{
    indent((json_parse_context*)context);
    printf("{\n");
    ((json_parse_context*)context)->depth++;
}
    
static void on_map_end(void* context)
{
    ((json_parse_context*)context)->depth--;
    indent((json_parse_context*)context);
    printf("}\n");
}

TEST(QJson_Examples, parse)
{
    qjson_parse_callbacks callbacks;
    callbacks.on_null = on_null;
    callbacks.on_boolean = on_bool;
    callbacks.on_int = on_int;
    callbacks.on_float = on_float;
    callbacks.on_string = on_string;
    callbacks.on_parse_error = on_parse_error;
    callbacks.on_list_start = on_list_start;
    callbacks.on_list_end = on_list_end;
    callbacks.on_map_start = on_map_start;
    callbacks.on_map_end = on_map_end;

    json_parse_context context = {0};
    const char* json_string = "{\"null\":null,\"one\":1,\"list\":[1,2,3,{\"a\":1,\"b\":2,\"c\":3}],\"true\":true}";
    qjson_parse_string(json_string, &callbacks, &context);
}
