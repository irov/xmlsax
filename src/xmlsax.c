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

#include "xmlsax.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
static const uint8_t xmlsax_lookup_digits_tables[256] =
{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255,
    255, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
//////////////////////////////////////////////////////////////////////////
static void xmlsax_strmove( xmlsax_char_t * _src, const xmlsax_char_t * _dst )
{
    size_t memmove_len = strlen( _dst );
    memmove( _src, _dst, memmove_len );
    _src[memmove_len] = '\0';
}
//////////////////////////////////////////////////////////////////////////
static xmlsax_result_t xmlsax_adapt_value_attribute( xmlsax_char_t * _value )
{
    xmlsax_char_t * src = _value;

    for( ;;)
    {
        src = strchr( src, '&' );

        if( src == XMLSAX_NULLPTR )
        {
            break;
        }

        switch( src[1] )
        {
        case 'a': // &amp; &apos;
            {
                if( src[2] == 'm' && src[3] == 'p' && src[4] == ';' )
                {
                    *src = '&';
                    src += 1;

                    xmlsax_strmove( src, src + 4 );
                }
                else if( src[2] == 'p' && src[3] == 'o' && src[4] == 's' && src[5] == ';' )
                {
                    *src = '\'';
                    src += 1;

                    xmlsax_strmove( src, src + 5 );
                }
                else
                {
                    return XMLSAX_FALSE;
                }
            }break;
        case 'q': // &quot;
            {
                if( src[2] == 'u' && src[3] == 'o' && src[4] == 't' && src[5] == ';' )
                {
                    *src = '"';
                    src += 1;

                    xmlsax_strmove( src, src + 5 );
                }
                else
                {
                    return XMLSAX_FALSE;
                }
            }break;
        case 'g': // &gt;
            {
                if( src[2] == 't' && src[3] == ';' )
                {
                    *src = '>';
                    src += 1;

                    xmlsax_strmove( src, src + 3 );
                }
                else
                {
                    return XMLSAX_FALSE;
                }
            }break;
        case 'l': // &lt;
            {
                if( src[2] == 't' && src[3] == ';' )
                {
                    *src = '<';
                    src += 1;

                    xmlsax_strmove( src, src + 3 );
                }
                else
                {
                    return XMLSAX_FALSE;
                }
            }break;
        case 'n': // &nbsp;
            {
                if( src[2] == 'b' && src[3] == 's' && src[4] == 'p' && src[5] == ';' )
                {
                    *(src + 0) = -64; //0xC2
                    *(src + 1) = -96; //0xA0
                    src += 2;

                    xmlsax_strmove( src, src + 4 );
                }
                else
                {
                    return XMLSAX_FALSE;
                }
            }break;
        case '#':
            {
                uint32_t code = 0;
                const xmlsax_char_t * src_code = src;

                if( src[2] == 'x' )
                {
                    src_code += 3;
                    for( ;;)
                    {
                        uint8_t table_code = (uint8_t)(*src_code);
                        uint8_t digit = xmlsax_lookup_digits_tables[table_code];
                        if( digit == 0xFF )
                        {
                            break;
                        }

                        code = code * 16 + digit;
                        ++src_code;
                    }
                }
                else
                {
                    src_code += 2;
                    for( ;;)
                    {
                        uint8_t table_code = (uint8_t)(*src_code);
                        uint8_t digit = xmlsax_lookup_digits_tables[table_code];
                        if( digit == 0xFF )
                        {
                            break;
                        }

                        code = code * 10 + digit;
                        ++src_code;
                    }
                }

                if( *src_code != ';' )
                {
                    return XMLSAX_FALSE;
                }

                ++src_code;

                if( code < 0x80 ) // 1 byte sequence
                {
                    src[0] = (uint8_t)(code);

                    src += 1;
                }
                else if( code < 0x800 ) // 2 byte sequence
                {
                    src[1] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[0] = (uint8_t)(code | 0xC0);

                    src += 2;
                }
                else if( code < 0x10000 ) // 3 byte sequence
                {
                    src[2] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[1] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[0] = (uint8_t)(code | 0xE0);

                    src += 3;
                }
                else if( code < 0x110000 ) // 4 byte sequence
                {
                    src[3] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[2] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[1] = (uint8_t)((code | 0x80) & 0xBF); code >>= 6;
                    src[0] = (uint8_t)(code | 0xF0);

                    src += 4;
                }
                else
                {
                    return XMLSAX_FALSE;
                }

                xmlsax_strmove( src, src_code );
            }break;
        }
    }

    return XMLSAX_TRUE;
}
//////////////////////////////////////////////////////////////////////////
inline static xmlsax_char_t * xmlsax_unfind( xmlsax_char_t * s, xmlsax_char_t ch )
{
    xmlsax_char_t * e = s;
    while( *e && *e == ch )
    {
        ++e;
    }

    return e;
}
//////////////////////////////////////////////////////////////////////////
static xmlsax_char_t * xmlsax_parse_node_attribute( xmlsax_attribute_t * _attr, xmlsax_char_t * _attribute, const xmlsax_char_t * _node )
{
    xmlsax_char_t * end_name_node_attribute = strpbrk( _attribute, " =" );

    if( end_name_node_attribute == XMLSAX_NULLPTR )
    {
        return XMLSAX_NULLPTR;
    }

    if( *end_name_node_attribute == ' ' )
    {
        *end_name_node_attribute = '\0';
    }
    else if( *end_name_node_attribute == '=' )
    {
        *end_name_node_attribute = '\0';
    }

    xmlsax_char_t * begin_value_node_attribute = strchr( end_name_node_attribute + 1, '"' );
    xmlsax_char_t * end_value_node_attribute = strchr( begin_value_node_attribute + 1, '"' );

    *end_value_node_attribute = '\0';

    if( xmlsax_adapt_value_attribute( begin_value_node_attribute + 1 ) == XMLSAX_FALSE )
    {
        return XMLSAX_NULLPTR;
    }

    uint32_t attr_count = _attr->count++;
    _attr->key[attr_count] = _attribute;
    _attr->value[attr_count] = begin_value_node_attribute + 1;

    xmlsax_char_t * find_next_node_attribute = xmlsax_unfind( end_value_node_attribute + 1, ' ' );

    if( *find_next_node_attribute == '>' )
    {
        return find_next_node_attribute;
    }
    else if( *find_next_node_attribute == '/' )
    {
        return find_next_node_attribute;
    }

    xmlsax_char_t * end_node_attribute = xmlsax_parse_node_attribute( _attr, find_next_node_attribute, _node );

    return end_node_attribute;
}
//////////////////////////////////////////////////////////////////////////
static xmlsax_char_t * xmlsax_parse_node( xmlsax_char_t * _node, const xmlsax_callbacks_t * _callbacks, void * _userdata )
{
    xmlsax_char_t * begin_name_node = xmlsax_unfind( _node, ' ' );

    if( *begin_name_node == '/' )
    {
        xmlsax_char_t * begin_name_node_end = xmlsax_unfind( begin_name_node + 1, ' ' );
        xmlsax_char_t * end_name_node_end = strpbrk( begin_name_node, " >" );

        if( end_name_node_end == XMLSAX_NULLPTR )
        {
            return XMLSAX_NULLPTR;
        }

        if( *end_name_node_end == ' ' )
        {
            *end_name_node_end = '\0';

            (*_callbacks->end_node)(begin_name_node_end, _userdata);

            xmlsax_char_t * end_name_node2 = strchr( begin_name_node_end + 1, '>' );

            return end_name_node2 + 1;
        }
        else if( *end_name_node_end == '>' )
        {
            *end_name_node_end = '\0';

            (*_callbacks->end_node)(begin_name_node_end, _userdata);

            return end_name_node_end + 1;
        }
    }
    else if( *begin_name_node == '!' )
    {
        xmlsax_char_t * end_name_node2 = strstr( begin_name_node + 1, "-->" );

        return end_name_node2 + 3;
    }
    else if( *begin_name_node == '?' )
    {
        xmlsax_char_t * end_name_node2 = strstr( begin_name_node + 1, "?>" );

        return end_name_node2 + 2;
    }

    xmlsax_char_t * end_name_node = strpbrk( begin_name_node, " />" );

    if( end_name_node == XMLSAX_NULLPTR )
    {
        return XMLSAX_NULLPTR;
    }

    if( *end_name_node == ' ' )
    {
        *end_name_node = '\0';

        (*_callbacks->begin_node)(begin_name_node, _userdata);

        xmlsax_char_t * find_node_attribute = xmlsax_unfind( end_name_node + 1, ' ' );

        if( *find_node_attribute == '>' )
        {
            return find_node_attribute + 1;
        }
        else if( *find_node_attribute == '/' )
        {
            (*_callbacks->end_node)(begin_name_node, _userdata);

            xmlsax_char_t * end_name_node2 = strchr( find_node_attribute + 1, '>' );

            return end_name_node2 + 1;
        }

        xmlsax_attribute_t attr;
        attr.count = 0;

        xmlsax_char_t * end_node_attribute = xmlsax_parse_node_attribute( &attr, find_node_attribute, begin_name_node );

        if( end_node_attribute == XMLSAX_NULLPTR )
        {
            return XMLSAX_NULLPTR;
        }

        (*_callbacks->node_attributes)(begin_name_node, attr.count, attr.key, attr.value, _userdata);

        if( *end_node_attribute == '>' )
        {
            return end_node_attribute + 1;
        }
        else if( *end_node_attribute == '/' )
        {
            (*_callbacks->end_node)(begin_name_node, _userdata);

            xmlsax_char_t * end_node_attribute2 = strchr( end_node_attribute + 1, '>' );

            return end_node_attribute2 + 1;
        }

        return end_node_attribute;
    }
    else if( *end_name_node == '>' )
    {
        *end_name_node = '\0';

        (*_callbacks->begin_node)(begin_name_node, _userdata);

        return end_name_node + 1;
    }
    else if( *end_name_node == '/' )
    {
        *end_name_node = '\0';

        (*_callbacks->begin_node)(begin_name_node, _userdata);
        (*_callbacks->end_node)(begin_name_node, _userdata);

        xmlsax_char_t * end_name_node2 = strchr( end_name_node + 1, '>' );
        return end_name_node2 + 1;
    }

    return XMLSAX_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
static xmlsax_char_t * xmlsax_parse_node_wobe( xmlsax_char_t * _node, const xmlsax_callbacks_t * _callbacks, void * _userdata )
{
    xmlsax_char_t * begin_name_node = xmlsax_unfind( _node, ' ' );

    if( *begin_name_node == '/' )
    {
        xmlsax_char_t * begin_name_node_end = xmlsax_unfind( begin_name_node + 1, ' ' );
        xmlsax_char_t * end_name_node_end = strpbrk( begin_name_node, " >" );

        if( end_name_node_end == XMLSAX_NULLPTR )
        {
            return XMLSAX_NULLPTR;
        }

        if( *end_name_node_end == ' ' )
        {
            *end_name_node_end = '\0';

            xmlsax_char_t * end_name_node2 = strchr( begin_name_node_end + 1, '>' );

            return end_name_node2 + 1;
        }
        else if( *end_name_node_end == '>' )
        {
            *end_name_node_end = '\0';

            return end_name_node_end + 1;
        }
    }
    else if( *begin_name_node == '!' )
    {
        xmlsax_char_t * end_name_node2 = strstr( begin_name_node + 1, "-->" );

        return end_name_node2 + 3;
    }
    else if( *begin_name_node == '?' )
    {
        xmlsax_char_t * end_name_node2 = strstr( begin_name_node + 1, "?>" );

        return end_name_node2 + 2;
    }

    xmlsax_char_t * end_name_node = strpbrk( begin_name_node, " />" );

    if( end_name_node == XMLSAX_NULLPTR )
    {
        return XMLSAX_NULLPTR;
    }

    if( *end_name_node == ' ' )
    {
        *end_name_node = '\0';

        xmlsax_char_t * find_node_attribute = xmlsax_unfind( end_name_node + 1, ' ' );

        if( *find_node_attribute == '>' )
        {
            return find_node_attribute + 1;
        }
        else if( *find_node_attribute == '/' )
        {
            xmlsax_char_t * end_name_node2 = strchr( find_node_attribute + 1, '>' );

            return end_name_node2 + 1;
        }

        xmlsax_attribute_t attr;
        attr.count = 0;

        xmlsax_char_t * end_node_attribute = xmlsax_parse_node_attribute( &attr, find_node_attribute, begin_name_node );

        if( end_node_attribute == XMLSAX_NULLPTR )
        {
            return XMLSAX_NULLPTR;
        }

        (*_callbacks->node_attributes)(begin_name_node, attr.count, attr.key, attr.value, _userdata);

        if( *end_node_attribute == '>' )
        {
            return end_node_attribute + 1;
        }
        else if( *end_node_attribute == '/' )
        {
            xmlsax_char_t * end_node_attribute2 = strchr( end_node_attribute + 1, '>' );

            return end_node_attribute2 + 1;
        }

        return end_node_attribute;
    }
    else if( *end_name_node == '>' )
    {
        *end_name_node = '\0';

        return end_name_node + 1;
    }
    else if( *end_name_node == '/' )
    {
        *end_name_node = '\0';

        xmlsax_char_t * end_name_node2 = strchr( end_name_node + 1, '>' );
        return end_name_node2 + 1;
    }

    return XMLSAX_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
xmlsax_result_t xmlsax_parse( xmlsax_char_t * _buffer, const xmlsax_callbacks_t * _callbacks, void * _userdata )
{
    xmlsax_char_t * begin_node = _buffer;

    uint32_t BOM = 0;
    BOM |= (uint8_t)(begin_node[0]) << 16;
    BOM |= (uint8_t)(begin_node[1]) << 8;
    BOM |= (uint8_t)(begin_node[2]);

    if( BOM == 0x00EFBBBF )
    {
        begin_node += 3;
    }

    typedef xmlsax_char_t * (*xmlsax_parse_node_t)(xmlsax_char_t * _node, const xmlsax_callbacks_t * _callbacks, void * _userdata);

    xmlsax_parse_node_t parse_node = (_callbacks->begin_node == 0 && _callbacks->end_node == 0)
        ? xmlsax_parse_node_wobe : xmlsax_parse_node;

    for( ;;)
    {
        begin_node = strchr( begin_node, '<' );

        if( begin_node == XMLSAX_NULLPTR )
        {
            break;
        }

        begin_node = (*parse_node)(begin_node + 1, _callbacks, _userdata);
        
        if( begin_node == XMLSAX_NULLPTR )
        {
            return XMLSAX_FALSE;
        }
    }

    return XMLSAX_TRUE;
}