/******************************************************************************
*
* Author, Yuriy Levchenko <irov13@mail.ru>
*
* This is free and unencumbered software released into the public domain.
*
* Anyone is free to copy, modify, publish, use, compile, sell, or
* distribute this software, either in source code form or as a compiled
* binary, for any purpose, commercial or non - commercial, and by any
* means.
*
* In jurisdictions that recognize copyright laws, the author or authors
* of this software dedicate any and all copyright interest in the
* software to the public domain.We make this dedication for the benefit
* of the public at large and to the detriment of our heirs and
* successors.We intend this dedication to be an overt act of
* relinquishment in perpetuity of all present and future rights to this
* software under copyright law.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* For more information, please refer to <http://unlicense.org/>
*
*****************************************************************************/

#ifndef XMLSAX_H_
#define XMLSAX_H_

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
typedef void(*xmlsax_callback_begin_node_t)(const xmlsax_char_t * _node, void * _userdata);
typedef void(*xmlsax_callback_end_node_t)(const xmlsax_char_t * _node, void * _userdata);
typedef void(*xmlsax_callback_node_attributes_t)(const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** _keys, const xmlsax_char_t ** _values, void * _userdata);
//////////////////////////////////////////////////////////////////////////
typedef struct xmlsax_callbacks_t
{
    xmlsax_callback_begin_node_t begin_node;
    xmlsax_callback_node_attributes_t node_attributes;
    xmlsax_callback_end_node_t end_node;    
} xmlsax_callbacks_t;
//////////////////////////////////////////////////////////////////////////
xmlsax_result_t xmlsax_parse( xmlsax_char_t * _buffer, const xmlsax_callbacks_t * _callbacks, void * _userdata );
//////////////////////////////////////////////////////////////////////////

#endif