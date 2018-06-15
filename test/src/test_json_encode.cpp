#include <gtest/gtest.h>
#include <qjson/qjson.h>

#define DEFINE_ENCODE_TEST(NAME, EXPECTED, ...) \
TEST(QJson_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    qjson_encode_context context = qjson_new_context(buff, buff + sizeof(buff)); \
    __VA_ARGS__ \
    qjson_end_encoding(&context); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

DEFINE_ENCODE_TEST(null, "null", { qjson_add_null(&context); })
DEFINE_ENCODE_TEST(false, "false", { qjson_add_boolean(&context, false); })
DEFINE_ENCODE_TEST(true, "true", { qjson_add_boolean(&context, true); })
DEFINE_ENCODE_TEST(int_1000, "1000", { qjson_add_integer(&context, 1000); })
DEFINE_ENCODE_TEST(int_9223372036854775807, "9223372036854775807", { qjson_add_integer(&context, 9223372036854775807); })
DEFINE_ENCODE_TEST(int_n9223372036854775808, "-9223372036854775808", { qjson_add_integer(&context, -9223372036854775808); })
DEFINE_ENCODE_TEST(float_1_1, "1.1", { qjson_add_float(&context, 1.1); })
DEFINE_ENCODE_TEST(float_924_5122045, "924.5122045", { qjson_add_float(&context, 924.5122045); })
DEFINE_ENCODE_TEST(string, "\"a string\"", { qjson_add_string(&context, "a string"); })

DEFINE_ENCODE_TEST(list, "[1,2,3]",
{
    qjson_start_list(&context);
    qjson_add_integer(&context, 1);
    qjson_add_integer(&context, 2);
    qjson_add_integer(&context, 3);
    qjson_end_container(&context);
})

DEFINE_ENCODE_TEST(map, "{\"a\":1,\"b\":2,\"c\":3}",
{
    qjson_start_map(&context);
    qjson_add_string(&context, "a");
    qjson_add_integer(&context, 1);
    qjson_add_string(&context, "b");
    qjson_add_integer(&context, 2);
    qjson_add_string(&context, "c");
    qjson_add_integer(&context, 3);
    qjson_end_container(&context);
})

DEFINE_ENCODE_TEST(complex, "{\"null\":null,\"one\":1,\"list\":[1,2,3,{\"a\":1,\"b\":2,\"c\":3}],\"true\":true}",
{
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
})
