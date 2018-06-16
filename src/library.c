#include "qjson/qjson.h"
#include "qjson_version.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>


static inline bool has_room_for_bytes(qjson_encode_context* context, int byte_count)
{
	return context->pos + byte_count <= context->end;
}

const char* qjson_version()
{
    return QJSON_VERSION;
}

qjson_encode_context qjson_new_encode_context_with_config(uint8_t* const memory_start,
                                                          uint8_t* const memory_end,
                                                          int indent_spaces,
                                                          int float_digits_precision)
{
    qjson_encode_context context =
    {
        .start = memory_start,
        .pos = memory_start,
        .end = memory_end,
        .indent_spaces = indent_spaces,
        .float_digits_precision = float_digits_precision,
        .container_level = 0,
        .is_first_in_document = true,
        .is_first_in_container = false,
        .next_object_is_key = false,
    };
    return context;

}

qjson_encode_context qjson_new_encode_context(uint8_t* const memory_start, uint8_t* const memory_end)
{
	return qjson_new_encode_context_with_config(memory_start,
												memory_end,
												DEFAULT_INDENT_SPACES,
												DEFAULT_FLOAT_DIGITS_PRECISION);
}

static void add_bytes(qjson_encode_context* const context, const char* bytes, size_t length)
{
	memcpy(context->pos, bytes, length);
	context->pos += length;
}

static bool add_indentation(qjson_encode_context* const context)
{
	if(context->indent_spaces <= 0)
	{
		return true;
	}

	int num_spaces = context->indent_spaces * context->container_level;
	if(!has_room_for_bytes(context, num_spaces + 1)) return false;
	*context->pos++ = '\n';
	memset(context->pos, ' ', num_spaces);
	context->pos += num_spaces;
	return true;
}

static bool begin_new_object(qjson_encode_context* const context)
{
	if(context->is_first_in_document)
	{
		context->is_first_in_document = false;
		return true;
	}

	bool is_in_map = context->is_inside_map[context->container_level];
	bool next_object_is_key = context->next_object_is_key;

	if(is_in_map)
	{
		context->next_object_is_key = !next_object_is_key;
	}

	if(context->is_first_in_container)
	{
		if(!add_indentation(context)) return false;
		context->is_first_in_container = false;
		return true;
	}

	if(!has_room_for_bytes(context, 1)) return false;

	if(!is_in_map || next_object_is_key)
	{
		*context->pos++ = ',';
		if(!add_indentation(context)) return false;
		return true;
	}

	*context->pos++ = ':';
	if(context->indent_spaces > 0)
	{
		if(!has_room_for_bytes(context, 1)) return false;
		*context->pos++ = ' ';
	}
	return true;
}

static bool add_object(qjson_encode_context* const context, const char* encoded_object)
{
	if(!begin_new_object(context)) return false;
	int length = strlen(encoded_object);
	if(!has_room_for_bytes(context, length)) return false;
	add_bytes(context, encoded_object, length);
	return true;
}

bool qjson_add_null(qjson_encode_context* const context)
{
	if(context->next_object_is_key) return false;
	return add_object(context, "null");
}

bool qjson_add_boolean(qjson_encode_context* const context, const bool value)
{
	if(context->next_object_is_key) return false;
	return add_object(context, value ? "true" : "false");
}

bool qjson_add_integer(qjson_encode_context* const context, const int64_t value)
{
	if(context->next_object_is_key) return false;
	char buffer[21];
	sprintf(buffer, "%ld", value);
	return add_object(context, buffer);
}

bool qjson_add_float(qjson_encode_context* const context, const double value)
{
	if(context->next_object_is_key) return false;
	char fmt[10];
	sprintf(fmt, "%%.%dlg", context->float_digits_precision);
	char buffer[context->float_digits_precision + 2];
	sprintf(buffer, fmt, value);
	return add_object(context, buffer);
}

static char get_escape_char(char ch)
{
	switch(ch)
	{
		case '\\': return '\\';
		case  '"': return '"';
		case '\b': return 'b';
		case '\f': return 'f';
		case '\n': return 'n';
		case '\r': return 'r';
		case '\t': return 't';
		default:   return 0;
	}
}

static bool add_substring_with_escaping(qjson_encode_context* const context, const char* const start, const char* const end)
{
	for(const char* src = start; src < end; src++)
	{
		char ch = *src;
		char escape_ch = get_escape_char(ch);
		if(escape_ch != 0)
		{
			if(!has_room_for_bytes(context, 2)) return false;
			*context->pos++ = '\\';
			*context->pos++ = escape_ch;
		}
		else
		{
			if(!has_room_for_bytes(context, 1)) return false;
			*context->pos++ = ch;
		}
	}
	return true;
}

bool qjson_add_substring(qjson_encode_context* const context, const char* const start, const char* const end)
{
	size_t byte_count = end - start;
	if(!add_object(context, "\"")) return false;
	if(!has_room_for_bytes(context, byte_count + 1)) return false;
	add_substring_with_escaping(context, start, end);
	add_bytes(context, "\"", 1);
	return true;
}

bool qjson_add_string(qjson_encode_context* const context, const char* const str)
{
	return qjson_add_substring(context, str, str + strlen(str));
}

static bool start_container(qjson_encode_context* const context, bool is_map)
{
	if(context->next_object_is_key) return false;
	begin_new_object(context);
	if(!has_room_for_bytes(context, 1)) return false;
	add_bytes(context, is_map ? "{" : "[", 1);
	context->container_level++;
	context->is_first_in_container = true;
	context->is_inside_map[context->container_level] = is_map;
	context->next_object_is_key = is_map;
	return true;

}

bool qjson_start_list(qjson_encode_context* const context)
{
	return start_container(context, false);
}

bool qjson_start_map(qjson_encode_context* const context)
{
	return start_container(context, true);
}

bool qjson_end_container(qjson_encode_context* const context)
{
	if(context->container_level <= 0)
	{
		return false;
	}
	bool is_in_map = context->is_inside_map[context->container_level];
	if(is_in_map && !context->next_object_is_key)
	{
		return false;
	}

	context->container_level--;
	if(!add_indentation(context)) return false;
	if(!has_room_for_bytes(context, 1)) return false;
	add_bytes(context, is_in_map ? "}" : "]", 1);
	context->next_object_is_key = context->is_inside_map[context->container_level];
	return true;
}

const char* qjson_end_encoding(qjson_encode_context* const context)
{
	while(context->container_level > 0)
	{
		if(!qjson_end_container(context))
		{
			return NULL;
		}
	}
	if(!has_room_for_bytes(context, 1)) return NULL;
	*context->pos = 0;
	return (const char*)context->pos;
}
