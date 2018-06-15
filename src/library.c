#include "qjson/qjson.h"
#include "qjson_version.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

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

static void add_bytes(qjson_encode_context* const context, const char* bytes, int length)
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
		}
		else
		{
			add_bytes(context, ",", 1);
		}
	}
	// todo: pretty print
	return true;
}

static bool add_object(qjson_encode_context* const context, const char* encoded_object)
{
	if(!begin_new_object(context)) return false;
	int length = strlen(encoded_object);
	RETURN_FALSE_IF_NO_ROOM(context, length);
	add_bytes(context, encoded_object, length);
	context->is_first_entry = false;
	if(context->is_inside_map[context->container_level])
	{
		context->is_key_slot = !context->is_key_slot;
	}
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
	// requires 21 bytes
	char buffer[30];
	sprintf(buffer, "%ld", value);
	return add_object(context, buffer);
}

bool qjson_add_float(qjson_encode_context* const context, const double value)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	char buffer[20];
	sprintf(buffer, "%10lg", value);
	return add_object(context, buffer);
}

bool qjson_add_string(qjson_encode_context* const context, const char* const str, const int byte_count)
{
	if(!add_object(context, "\"")) return false;
	// todo: escaping
	RETURN_FALSE_IF_NO_ROOM(context, byte_count);
	add_bytes(context, str, byte_count);
	add_bytes(context, "[", 1);
	return true;
}

bool qjson_start_list(qjson_encode_context* const context)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	begin_new_object(context);
	RETURN_FALSE_IF_NO_ROOM(context, 1);
	add_bytes(context, "[", 1);
	context->container_level++;
	context->is_first_entry = true;
	context->is_inside_map[context->container_level] = false;
	context->is_key_slot = false;
	return true;
}

bool qjson_start_map(qjson_encode_context* const context)
{
	RETURN_FALSE_IF_IS_KEY_SLOT(context);
	begin_new_object(context);
	RETURN_FALSE_IF_NO_ROOM(context, 1);
	add_bytes(context, "{", 1);
	context->container_level++;
	context->is_first_entry = true;
	context->is_inside_map[context->container_level] = true;
	context->is_key_slot = true;
	return true;
}

bool qjson_end_container(qjson_encode_context* const context)
{
	RETURN_FALSE_IF_NO_ROOM(context, 1);
	// TODO: sanity checks
	if(context->is_inside_map[context->container_level])
	{
		add_bytes(context, "}", 1);
	}
	else
	{
		add_bytes(context, "]", 1);
	}
	context->container_level--;
	return true;
}
