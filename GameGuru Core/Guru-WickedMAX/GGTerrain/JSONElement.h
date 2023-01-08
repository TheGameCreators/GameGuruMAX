#ifndef _H_JSONELEMENT
#define _H_JSONELEMENT

#include "stdint.h"

#define JSON_OBJECT		1
#define JSON_ARRAY		2
#define JSON_STRING		3
#define JSON_NUMBER		4
#define JSON_BOOL		5

class JSONElement
{
	public:
		JSONElement *m_pTempNext;

		JSONElement() { m_pTempNext = 0; }
		virtual ~JSONElement() {};

		virtual int GetType() { return 0; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
			
		//static JSONElement* LoadJSONFromFile( const char* filename );
		static JSONElement* LoadJSONFromData( const char* data );
		static int ParseString( const char* data );
		static int ParseNumber( const char* data );
		static const char* TypeToString( int type );

};

class JSONKeyPair
{
	public:
		char* m_sName;
		JSONElement *m_pElement;
		JSONKeyPair *m_pTempNext;

		JSONKeyPair() { m_sName = 0; m_pElement = 0; m_pTempNext = 0; }
		~JSONKeyPair() 
		{ 
			if ( m_pElement ) delete m_pElement; 
			if ( m_sName ) delete [] m_sName;
		}

		int ParsePair( const char* data );
		int FindPairEnd( const char* data );
};

class JSONObject : public JSONElement
{
	public:
		uint32_t m_iNumPairs;
		JSONKeyPair **m_pPairs;

		JSONObject() { m_iNumPairs = 0; m_pPairs = 0; }
		~JSONObject() 
		{ 
			if ( m_pPairs ) 
			{
				for ( uint32_t i = 0; i < m_iNumPairs; i++ ) if ( m_pPairs[ i ] ) delete m_pPairs[ i ];
					
				delete [] m_pPairs; 
			}
		}

		int ParseObject( const char* data );

		int GetType() { return JSON_OBJECT; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
		JSONElement* GetElement( const char* szKey );
};

class JSONArray : public JSONElement
{
	public:
		uint32_t m_iNumElements;
		JSONElement **m_pElements;

		JSONArray() { m_iNumElements = 0; m_pElements = 0; }
		~JSONArray() 
		{ 
			if ( m_pElements ) 
			{
				for ( uint32_t i = 0; i < m_iNumElements; i++ ) if ( m_pElements[ i ] ) delete m_pElements[ i ];

				delete [] m_pElements; 
			}
		}

		int ParseArray( const char* data );

		int GetType() { return JSON_ARRAY; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
		JSONElement* GetElement( int index );

		int FindArrayEnd( const char* data );
};

class JSONString : public JSONElement
{
	public:
		char* m_sValue = 0;

		JSONString() {}
		~JSONString() { if ( m_sValue ) delete [] m_sValue; }

		int GetType() { return JSON_STRING; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
};

class JSONNumber : public JSONElement
{
	public:
		float m_fValue;
		int m_iValue;
		int m_iIsInt;

		JSONNumber() { m_fValue = 0; m_iValue = 0; m_iIsInt = 0; }
		~JSONNumber() {}

		int GetType() { return JSON_NUMBER; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
};

class JSONBool : public JSONElement
{
	public:
		bool m_bValue;

		JSONBool() { m_bValue = false; }
		~JSONBool() {}

		int GetType() { return JSON_BOOL; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
};

class JSONNull : public JSONElement
{
	public:

		JSONNull() {}
		~JSONNull() {}

		int GetType() { return 0; } // 0=Undefined, 1=Object, 2=Array, 3=String, 4=Number, 5=Bool
};

#endif