QJSON
=====

Quick JSON codec in pure C.



Features
--------

 * Pure C
 * Bison & flex based parsing
 * Simple, low level interface suitable for bridging to other languages
 * No runtime dependencies besides standard C library
 * Small footprint
 * Pretty printing with user-configurable indentation
 * User-configurable floating point precision



Building
--------

    mkdir build
    cd build
    cmake ..
    make

Produces: build/libqjson.a



Testing
-------

    ./test/qjson_test



Usage
-----

### Encoding

    #include <qjson/qjson.h>
    #include <stdio.h>

    void encode_example()
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

#### Result:

    {
        "null": null,
        "one": 1,
        "list": [
            1,
            2,
            3,
            {
                "a": 1,
                "b": 2,
                "c": 3
            }
        ],
        "true": true
    }



### Parsing

    #include <qjson/qjson.h>
    #include <stdio.h>

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

    void parse_example()
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

#### Result:

    {
      "null"
      null
      "one"
      1
      "list"
      [
        1
        2
        3
        {
          "a"
          1
          "b"
          2
          "c"
          3
        }
      ]
      "true"
      true
    }



License
-------

Copyright 2018 Karl Stenerud

Released under MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
