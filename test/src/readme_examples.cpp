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

TEST(QJson_Examples, parse)
{
}
