#include "qjson/qjson.h"
#include "qjson_version.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

#define FLOAT_PRECISION_DIGITS 15

#define STRINGIFY(a) STRINGIFY_EXPAND(a)
#define STRINGIFY_EXPAND(a) #a

#define RETURN_FALSE_IF_NO_ROOM(CONTEXT, LENGTH) \
	if((CONTEXT)->pos + (LENGTH) > (CONTEXT)->end) return false

#define RETURN_FALSE_IF_IS_KEY_SLOT(CONTEXT) \
	if((CONTEXT)->is_key_slot) return false

const char* qjson_version()
{
    return QJSON_VERSION;
}

qjson_encode_context qjson_new_context(uint8_t* const memory_start, uint8_t* const memory_end)
{
    qjson_encode_context context =
    {
        .start = memory_start,
        .pos = memory_start,
        .end = memory_end,
        .container_level = 0,
        .is_first_entry = 0,
        .is_key_slot = 0,
    };
    return context;
}

static void add_bytes(qjson_encode_context* const context, const char* bytes, size_t length)
{
	memcpy(context->pos, bytes, length);
	context->pos += length;
}

static bool begin_new_object(qjson_encode_context* const context)
{
	if(context->container_level > 0 && !context->is_first_entry)
	{
		RETURN_FALSE_IF_NO_ROOM(context, 1);
		if(context->is_inside_map[context->container_level])
		{
			if(context->is_key_slot)
			{
				add_bytes(context, ",", 1);
			}
			else
			{
				add_bytes(context, ":", 1);
			}
			context->is_key_slot = !context->is_key_slot;
		}
		else
		{
			add_bytes(context, ",", 1);
		}
	}
	context->is_first_entry = false;
	// todo: pretty print
	return true;
}

static bool add_object(qjson_encode_context* const context, const char* encoded_object)
{
	if(!begin_new_object(context)) return false;
	int length = strlen(encoded_object);
	RETURN_FALSE_IF_NO_ROOM(context, length);
	add_bytes(context, encoded_object, length);
	return true;
}

bool qjson_add_null(qjson_encode_context* const context)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	return add_object(context, "null");
}

bool qjson_add_boolean(qjson_encode_context* const context, const bool value)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	return add_object(context, value ? "true" : "false");
}

bool qjson_add_integer(qjson_encode_context* const context, const int64_t value)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	char buffer[21];
	sprintf(buffer, "%ld", value);
	return add_object(context, buffer);
}

bool qjson_add_float(qjson_encode_context* const context, const double value)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	char buffer[FLOAT_PRECISION_DIGITS + 2];
	sprintf(buffer, "%." STRINGIFY(FLOAT_PRECISION_DIGITS) "lg", value);
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
			RETURN_FALSE_IF_NO_ROOM(context, 2);
			*context->pos++ = '\\';
			*context->pos++ = escape_ch;
		}
		else
		{
			RETURN_FALSE_IF_NO_ROOM(context, 1);
			*context->pos++ = ch;
		}
	}
	return true;
}

bool qjson_add_substring(qjson_encode_context* const context, const char* const start, const char* const end)
{
	size_t byte_count = end - start;
	if(!add_object(context, "\"")) return false;
	RETURN_FALSE_IF_NO_ROOM(context, byte_count + 1);
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
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	begin_new_object(context);
	RETURN_FALSE_IF_NO_ROOM(context, 1);
	add_bytes(context, is_map ? "{" : "[", 1);
	context->container_level++;
	context->is_first_entry = true;
	context->is_inside_map[context->container_level] = is_map;
	context->is_key_slot = false;
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
	RETURN_FALSE_IF_NO_ROOM(context, 1);
	// TODO: sanity checks
	add_bytes(context, context->is_inside_map[context->container_level] ? "}" : "]", 1);
	context->container_level--;
	context->is_first_entry = false;
	context->is_key_slot = true;
	return true;
}

bool qjson_end_encoding(qjson_encode_context* const context)
{
	while(context->container_level > 0)
	{
		if(!qjson_end_container(context))
		{
			return false;
		}
	}
	*context->pos = 0;
	return true;
}
