#include "JSONElement.h"
#include <string.h>
#include <stdlib.h>

void JSONError( const char* msg )
{

}

// resulting string will always be smaller so we can modify it in place
void JSONUnescapeString( char* data )
{
	if ( !data || !*data ) return;

	char *str = data;
	char *str2 = data;
	
	do
	{
		if ( *str == '\\' )
		{
			str++;
			switch( *str )
			{
				case 'n': *str2 = '\n'; break;
				case 'r': *str2 = '\r'; break;
				case '"': *str2 = '"'; break;
				case 'b': *str2 = '\b'; break;
				case 'f': *str2 = '\f'; break;
				case 't': *str2 = '\t'; break;
				case '/': *str2 = '/'; break;
				case '\\': *str2 = '\\'; break;
				default: *str2 = *str;
			}
			if ( *str == 0 ) break;
		}
		else 
		{
			*str2 = *str;
		}
		str++;
		str2++;
	} while ( *str );
		
	*str2 = 0;
}

const char* JSONElement::TypeToString( int type )
{
	switch( type )
	{
		case 0: return "Undefined";
		case JSON_OBJECT: return "Object";
		case JSON_ARRAY: return "Array";
		case JSON_STRING: return "String";
		case JSON_NUMBER: return "Number";
		case JSON_BOOL: return "Bool";
		default: return "Unknown";
	}
}

/*
JSONElement* JSONElement::LoadJSONFromFile( const char* filename )
{
	cFile oFile;
	if ( !oFile.OpenToRead( filename ) ) return 0;

	uint32_t size = oFile.GetSize();
	char* data = new char[ size+1 ];
	oFile.ReadData( data, size );
	oFile.Close();

	data[ size ] = 0;
	JSONElement* element = LoadJSONFromData( data );
	delete [] data;
	return element;
}
*/

JSONElement* JSONElement::LoadJSONFromData( const char* data )
{
	uint32_t index = 0;

	while( 1 )
	{
		switch( data[ index ] )
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n': break;
			
			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				if ( pObject->ParseObject( data+index ) < 0 ) 
				{
					delete pObject;
					return 0;
				}
				return pObject;
			}
			
			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				if ( pArray->ParseArray( data+index ) < 0 ) 
				{
					delete pArray;
					return 0;
				}
				return pArray;
			}
			
			default:
			{
				JSONError( "Failed to parse JSON file, must begin with an object or array" );
				return 0;
			}
		}
		
		index++;
	}

	return 0;
}

int JSONObject::ParseObject( const char* data )
{
	int index = 0;

	JSONKeyPair *pPairList = 0;
	JSONKeyPair *pLastPair = 0;
	int count = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: 
			{
				while ( pPairList ) { pLastPair = pPairList; pPairList = pPairList->m_pTempNext; delete pLastPair; }
				JSONError("Invalid JSON, unexpected end of object"); 
				return -1;
			}

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;
			
			case '}': return index+1;
			
			case '"':
			{
				JSONKeyPair *pNewPair = new JSONKeyPair();
				index++;
				int length = pNewPair->ParsePair( data+index );
				if ( length < 0 ) 
				{
					while ( pPairList ) { pLastPair = pPairList; pPairList = pPairList->m_pTempNext; delete pLastPair; }
					delete pNewPair;
					return -1;
				}

				if ( pLastPair ) pLastPair->m_pTempNext = pNewPair;
				else pPairList = pNewPair;
				pLastPair = pNewPair;
				count++;

				index += length;
				if ( data[index] == '}' ) 
				{
					m_iNumPairs = count;
					m_pPairs = new JSONKeyPair*[ count ];
					for( int i = 0; i < count; i++ )
					{
						if ( pPairList )
						{
							m_pPairs[ i ] = pPairList;
							pPairList = pPairList->m_pTempNext;
							m_pPairs[ i ]->m_pTempNext = 0;
						}
						else m_pPairs[ i ] = 0;
					}
					return index+1;
				}

				break;
			}

			default:
			{
				JSONError("Invalid JSON, unexpected character in object"); 
				return -1;
			}
		}
	}
}

int JSONArray::ParseArray( const char* data )
{
	int index = 0;

	JSONElement *pElementList = 0;
	JSONElement *pLastElement = 0;
	int count = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: 
			{
				while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
				JSONError("Invalid JSON, unexpected end of array"); 
				return -1;
			}

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;

			case ']': 
			{
				m_iNumElements = count;
				m_pElements = new JSONElement*[ count ];
				for( int i = 0; i < count; i++ )
				{
					if ( pElementList )
					{
						m_pElements[ i ] = pElementList;
						pElementList = pElementList->m_pTempNext;
						m_pElements[ i ]->m_pTempNext = 0;
					}
					else m_pElements[ i ] = 0;
				}
				return index+1;
			}

			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				int length = pArray->ParseArray( data+index );
				
				if ( length < 0 ) 
				{
					delete pArray;
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}

				index += length;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pArray;
				else pElementList = pArray;
				pLastElement = pArray;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				int length = pObject->ParseObject( data+index );
				if ( length < 0 ) 
				{
					delete pObject;
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}

				index += length;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pObject;
				else pElementList = pObject;
				pLastElement = pObject;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '"': 
			{
				index++;
				int length = JSONElement::ParseString( data+index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				JSONString *pString = new JSONString();
				pString->m_sValue = new char[ length + 1 ];
				strncpy_s( pString->m_sValue, length+1, data+index, length );
				JSONUnescapeString( pString->m_sValue );
				
				index += (length+1);
								
				if ( pLastElement ) pLastElement->m_pTempNext = pString;
				else pElementList = pString;
				pLastElement = pString;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			{
				int length = JSONElement::ParseNumber( data+index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				char* str = new char[ length + 1 ];
				strncpy_s( str, length+1, data+index, length );
				JSONNumber *pNumber = new JSONNumber();
				if ( strchr(str, '.') == 0 && strchr(str, 'e') == 0 && strchr(str, 'E') == 0 ) 
				{
					pNumber->m_iIsInt = 1;
					pNumber->m_iValue = atoll( str );
				}
				pNumber->m_fValue = (float) atof( str );
				index += length;
				delete [] str;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pNumber;
				else pElementList = pNumber;
				pLastElement = pNumber;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case 'n':
			{
				if ( data[ index+1 ]  == 'u' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 'l' )
				{
					JSONNull *pNull = new JSONNull();

					if ( pLastElement ) pLastElement->m_pTempNext = pNull;
					else pElementList = pNull;
					pLastElement = pNull;
					count++;

					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			case 't':
			{
				if ( data[ index+1 ]  == 'r' 
				  && data[ index+2 ]  == 'u' 
				  && data[ index+3 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = true;
					
					if ( pLastElement ) pLastElement->m_pTempNext = pBool;
					else pElementList = pBool;
					pLastElement = pBool;
					count++;
					
					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			case 'f':
			{
				if ( data[ index+1 ]  == 'a' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 's'
				  && data[ index+4 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = false;
					
					if ( pLastElement ) pLastElement->m_pTempNext = pBool;
					else pElementList = pBool;
					pLastElement = pBool;
					count++;
					
					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			default:
			{
				JSONError("Invalid JSON, unexpected character in array element");
				return -1;
			}
		}
	}
}

int JSONArray::FindArrayEnd( const char* data )
{
	int index = 0;
	while ( data[index] && data[index] != ',' && data[index] != ']' ) index++;
	if ( !data[index] ) 
	{
		JSONError("Invalid JSON, unexpected end of array element");
		return -1;
	}

	if ( data[index] == ',' ) index++;
	return index;
}

int JSONKeyPair::FindPairEnd( const char* data )
{
	int index = 0;
	while ( data[index] && data[index] != ',' && data[index] != '}' ) index++;
	if ( !data[index] ) 
	{
		JSONError("Invalid JSON, unexpected end of object key pair");
		return -1;
	}

	if ( data[index] == ',' ) index++;
	return index;
}

int JSONKeyPair::ParsePair( const char *data )
{
	int length = JSONElement::ParseString( data );
	if ( length < 0 ) return -1;
	m_sName = new char[ length + 1 ];
	strncpy_s( m_sName, length+1, data, length );
	JSONUnescapeString( m_sName );
		
	int index = length+1;

	while ( data[index] && data[index] != ':' ) index++;
	if ( !data[index] ) 
	{
		JSONError("Invalid JSON, unexpected end of object key pair");
		return -1;
	}

	index++;
	
	while( 1 )
	{
		switch( data[ index ] )
		{
			case 0: JSONError("Invalid JSON, unexpected end of object key pair"); return -1;

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;

			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				int length = pArray->ParseArray( data+index );
				
				if ( length < 0 ) 
				{
					delete pArray;
					return -1;
				}

				index += length;
				m_pElement = pArray;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				int length = pObject->ParseObject( data+index );
				if ( length < 0 ) 
				{
					delete pObject;
					return -1;
				}

				index += length;
				m_pElement = pObject;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '"': 
			{
				index++;
				int length = JSONElement::ParseString( data+index );
				if ( length < 0 ) return -1;
				JSONString *pString = new JSONString();
				pString->m_sValue = new char[ length + 1 ];
				strncpy_s( pString->m_sValue, length+1, data+index, length );
				JSONUnescapeString( pString->m_sValue );

				index += (length+1);
				m_pElement = pString;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			{
				int length = JSONElement::ParseNumber( data+index );
				if ( length < 0 ) return -1;

				char* str = new char[ length + 1 ];
				strncpy_s( str, length+1, data+index, length );
				JSONNumber *pNumber = new JSONNumber();
				if ( strchr(str, '.') == 0 && strchr(str, 'e') == 0 && strchr(str, 'E') == 0 ) 
				{
					pNumber->m_iIsInt = 1;
					pNumber->m_iValue = atoll( str );
				}
				pNumber->m_fValue = (float) atof( str );
				index += length;
				delete [] str;

				m_pElement = pNumber;
								
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case 'n':
			{
				if ( data[ index+1 ]  == 'u' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 'l' )
				{
					JSONNull *pNull = new JSONNull();
					m_pElement = pNull;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}

			case 't':
			{
				if ( data[ index+1 ]  == 'r' 
				  && data[ index+2 ]  == 'u' 
				  && data[ index+3 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = true;
					m_pElement = pBool;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}
			case 'f':
			{
				if ( data[ index+1 ]  == 'a' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 's'
				  && data[ index+4 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = false;
					m_pElement = pBool;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					JSONError("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}
			default:
			{
				JSONError("Invalid JSON, unexpected character in object key pair");
				return -1;
			}
		}
	}
}

int JSONElement::ParseString( const char* data )
{
	int index = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: JSONError("Invalid JSON, unexpected end of string"); return -1;
			case '"': return index;
			case '\\': 
			{
				// skip escaped character
				index++; 
				if ( data[ index ] == 0 ) 
				{
					JSONError("Invalid JSON, unexpected end of string"); 
					return -1;
				}
				break;
			}
		}

		index++;
	}
}

int JSONElement::ParseNumber( const char* data )
{
	int index = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: JSONError("Invalid JSON, unexpected end of string"); return -1;
			case ',': return index;
			case '}': return index;
			case ']': return index;
			case '\n': return index;
			case '\r': return index;
			case ' ':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '+':
			case '.':
			case 'e':
			case 'E': break;
			default: JSONError("Invalid JSON, unexpected character in number"); return -1;
		}

		index++;
	}
}

JSONElement* JSONObject::GetElement( const char *szKey )
{
	if ( !m_pPairs ) return 0;

	for( uint32_t i = 0; i < m_iNumPairs; i++ )
	{
		if ( _stricmp( m_pPairs[ i ]->m_sName, szKey ) == 0 ) return m_pPairs[ i ]->m_pElement;
	}

	return 0;
}

JSONElement* JSONArray::GetElement( int index )
{
	if ( !m_pElements ) return 0;
	if ( index < 0 || index >= (int) m_iNumElements ) return 0;
	
	return m_pElements[ index ];
}