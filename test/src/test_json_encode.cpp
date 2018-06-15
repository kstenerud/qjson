#include <gtest/gtest.h>
#include <qjson/qjson.h>

#define DEFINE_ENCODE_TEST(NAME, EXPECTED, ...) \
TEST(QJson_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    qjson_encode_context context = qjson_new_encode_context(buff, buff + sizeof(buff)); \
    __VA_ARGS__ \
    ASSERT_NE(nullptr, qjson_end_encoding(&context)); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

#define DEFINE_ENCODE_FLOAT_TEST(NAME, DIGITS, EXPECTED, ...) \
TEST(QJson_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    qjson_encode_context context = qjson_new_encode_context_with_config(buff, \
                                                                        buff + sizeof(buff), \
                                                                        0, \
                                                                        DIGITS); \
    __VA_ARGS__ \
    ASSERT_NE(nullptr, qjson_end_encoding(&context)); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

#define DEFINE_ENCODE_FAIL_TEST(NAME, BUFFER_SIZE, ...) \
TEST(QJson_Encode, NAME) \
{ \
    uint8_t buff[BUFFER_SIZE]; \
    qjson_encode_context context = qjson_new_encode_context(buff, buff + sizeof(buff)); \
    __VA_ARGS__ \
}

DEFINE_ENCODE_TEST(null, "null", { qjson_add_null(&context); })
DEFINE_ENCODE_TEST(false, "false", { qjson_add_boolean(&context, false); })
DEFINE_ENCODE_TEST(true, "true", { qjson_add_boolean(&context, true); })
DEFINE_ENCODE_TEST(int_1000, "1000", { qjson_add_integer(&context, 1000); })
DEFINE_ENCODE_TEST(int_9223372036854775807, "9223372036854775807", { qjson_add_integer(&context, 9223372036854775807L); })
DEFINE_ENCODE_TEST(int_n9223372036854775807, "-9223372036854775807", { qjson_add_integer(&context, -9223372036854775807L); })
DEFINE_ENCODE_TEST(float_1_1, "1.1", { qjson_add_float(&context, 1.1); })
DEFINE_ENCODE_TEST(float_924_5122045, "924.5122045", { qjson_add_float(&context, 924.5122045); })
DEFINE_ENCODE_TEST(string, "\"a string\"", { qjson_add_string(&context, "a string"); })
DEFINE_ENCODE_TEST(escaped, "\"q\\\"s\\\\b\\bf\\fn\\nr\\rt\\t\"", { qjson_add_string(&context, "q\"s\\b\bf\fn\nr\rt\t"); })

DEFINE_ENCODE_FLOAT_TEST(float_limit_10, 10, "1.012345679", { qjson_add_float(&context, 1.0123456789); })

DEFINE_ENCODE_TEST(substring, "\"a string\"",
{
    const char* string = "a string that's a substring";
    qjson_add_substring(&context, string, string + 8);
})

DEFINE_ENCODE_TEST(list, "[1,2,3]",
{
    qjson_start_list(&context);
    qjson_add_integer(&context, 1);
    qjson_add_integer(&context, 2);
    qjson_add_integer(&context, 3);
    qjson_end_container(&context);
})

DEFINE_ENCODE_TEST(empty_list, "[]",
{
    qjson_start_list(&context);
    qjson_end_container(&context);
})

DEFINE_ENCODE_TEST(single_list, "[1]",
{
    qjson_start_list(&context);
    qjson_add_integer(&context, 1);
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

DEFINE_ENCODE_TEST(empty_map, "{}",
{
    qjson_start_map(&context);
    qjson_end_container(&context);
})

DEFINE_ENCODE_TEST(single_map, "{\"a\":1}",
{
    qjson_start_map(&context);
    qjson_add_string(&context, "a");
    qjson_add_integer(&context, 1);
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

DEFINE_ENCODE_FAIL_TEST(fail_int_size_0,0,
{
    ASSERT_FALSE(qjson_add_integer(&context, 1));
})

DEFINE_ENCODE_FAIL_TEST(fail_int_size_1,1,
{
    ASSERT_FALSE(qjson_add_integer(&context, 10));
})

DEFINE_ENCODE_FAIL_TEST(fail_float_size_1,1,
{
    ASSERT_FALSE(qjson_add_float(&context, 0.1));
})

DEFINE_ENCODE_FAIL_TEST(fail_string_size_10,10,
{
    ASSERT_FALSE(qjson_add_string(&context, "this is a test"));
})

DEFINE_ENCODE_FAIL_TEST(fail_null,3,
{
    ASSERT_FALSE(qjson_add_null(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_true,3,
{
    ASSERT_FALSE(qjson_add_boolean(&context, true));
})

DEFINE_ENCODE_FAIL_TEST(fail_false,4,
{
    ASSERT_FALSE(qjson_add_boolean(&context, false));
})

DEFINE_ENCODE_FAIL_TEST(fail_completion,5,
{
    ASSERT_TRUE(qjson_add_boolean(&context, false));
    ASSERT_FALSE(qjson_end_encoding(&context)); \
})

DEFINE_ENCODE_FAIL_TEST(fail_list,0,
{
    ASSERT_FALSE(qjson_start_list(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_map,0,
{
    ASSERT_FALSE(qjson_start_map(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_close_container,1,
{
    ASSERT_TRUE(qjson_start_list(&context));
    ASSERT_FALSE(qjson_end_container(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_end_encoding,4,
{
    ASSERT_TRUE(qjson_start_list(&context));
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_EQ(nullptr, qjson_end_encoding(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_early_map_close,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_TRUE(qjson_add_string(&context, "a"));
    ASSERT_FALSE(qjson_end_container(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_too_many_list_closes,100,
{
    ASSERT_TRUE(qjson_start_list(&context));
    ASSERT_TRUE(qjson_end_container(&context));
    ASSERT_FALSE(qjson_end_container(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_too_many_map_closes,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_TRUE(qjson_end_container(&context));
    ASSERT_FALSE(qjson_end_container(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_null,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_add_null(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_boolean,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_add_boolean(&context, true));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_integer,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_add_integer(&context, 1));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_float,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_add_float(&context, 0.1));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_map,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_start_map(&context));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_list,100,
{
    ASSERT_TRUE(qjson_start_map(&context));
    ASSERT_FALSE(qjson_start_list(&context));
})
