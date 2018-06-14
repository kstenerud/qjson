#include "parse_test_helpers.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static void* g_allocated[100000];
static int g_allocated_index = 0;
static void* allocate(int size)
{
    void* memory = malloc(size);
    g_allocated[g_allocated_index++] = memory;
    return memory;
}
static void clear_allocated()
{
    for(int i = 0; i < g_allocated_index; i++)
    {
        free(g_allocated[i]);
    }
    memset(g_allocated, 0, sizeof(g_allocated));
    g_allocated_index = 0;
}



static parsed_type g_parsed_type[100000];
static void* g_parsed_data[100000];
static int g_parsed_index = 0;

void parse_reset_all()
{
    memset(g_parsed_type, 0, sizeof(g_parsed_type));
    memset(g_parsed_data, 0, sizeof(g_parsed_data));
    g_parsed_index = 0;
    clear_allocated();
}

int parse_get_item_count()
{
    return g_parsed_index;
}

parsed_type parse_get_type(int index)
{
    return g_parsed_type[index];
}

const char* parse_get_string(int index)
{
    return (const char*)g_parsed_data[index];
}

int64_t parse_get_int(int index)
{
    return *((int64_t*)g_parsed_data[index]);
}

double parse_get_float(int index)
{
    return *((double*)g_parsed_data[index]);
}

bool parse_get_bool(int index)
{
    return *((bool*)g_parsed_data[index]);
}

void parse_add_type(parsed_type type)
{
    g_parsed_type[g_parsed_index] = type;
    g_parsed_index++;
}

void parse_add_string(parsed_type type, const char* value)
{
    char* newvalue = (char*)allocate(strlen(value) + 1);
    strcpy(newvalue, value);
    g_parsed_data[g_parsed_index] = newvalue;
    parse_add_type(type);
}

void parse_add_int(int64_t value)
{
    void* newvalue = allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    g_parsed_data[g_parsed_index] = newvalue;
    parse_add_type(TYPE_INT);
}

void parse_add_float(double value)
{
    void* newvalue = allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    g_parsed_data[g_parsed_index] = newvalue;
    parse_add_type(TYPE_FLOAT);
}

void parse_add_bool(bool value)
{
    void* newvalue = allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    g_parsed_data[g_parsed_index] = newvalue;
    parse_add_type(TYPE_BOOLEAN);
}

static void on_parse_error(void* context, const char* message)
{
    printf("Error: %s\n", message);
    parse_add_string(TYPE_ERROR, message);
}

static void on_string(void* context, const char* str)
{
    parse_add_string(TYPE_STRING, str);
}

static void on_null(void* context)
{
    parse_add_type(TYPE_NULL);
}
    
static void on_bool(void* context, bool value)
{
    parse_add_bool(value);
}
    
static void on_int(void* context, int64_t value)
{
    parse_add_int(value);
}
    
static void on_float(void* context, double value)
{
    parse_add_float(value);
}

static void on_list_start(void* context)
{
    parse_add_type(TYPE_LIST_START);
}
    
static void on_list_end(void* context)
{
    parse_add_type(TYPE_LIST_END);
}
    
static void on_map_start(void* context)
{
    parse_add_type(TYPE_MAP_START);
}
    
static void on_map_end(void* context)
{
    parse_add_type(TYPE_MAP_END);
}

qjson_parse_callbacks parse_new_callbacks()
{
    qjson_parse_callbacks callbacks;
    // sorry, unimplemented: non-trivial designated initializers not supported
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
	return callbacks;	
}
