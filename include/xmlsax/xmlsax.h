#pragma once

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
typedef int32_t xmlsax_result_t;
typedef char xmlsax_char_t;
//////////////////////////////////////////////////////////////////////////
static const xmlsax_result_t XMLSAX_TRUE = 1;
static const xmlsax_result_t XMLSAX_FALSE = 0;
static xmlsax_char_t * XMLSAX_NULLPTR = (xmlsax_char_t *)(0);
//////////////////////////////////////////////////////////////////////////
typedef struct xmlsax_attribute_t
{
    uint32_t count;

    const xmlsax_char_t * key[64];
    const xmlsax_char_t * value[64];
} xmlsax_attribute_t;
//////////////////////////////////////////////////////////////////////////
typedef void(*xmlsax_callback_end_node_t)(xmlsax_char_t * _node);
typedef void(*xmlsax_callback_begin_node_t)(xmlsax_char_t * _node);
typedef void(*xmlsax_callback_node_attributes_t)(xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** _key, const xmlsax_char_t ** _value);
//////////////////////////////////////////////////////////////////////////
typedef struct xmlsax_callbacks_t
{
    xmlsax_callback_begin_node_t begin_node;
    xmlsax_callback_end_node_t end_node;
    xmlsax_callback_node_attributes_t node_attributes;
} xmlsax_callbacks_t;
//////////////////////////////////////////////////////////////////////////
xmlsax_result_t xmlsax_parse( xmlsax_char_t * _buffer, const xmlsax_callbacks_t * _callbacks );
//////////////////////////////////////////////////////////////////////////