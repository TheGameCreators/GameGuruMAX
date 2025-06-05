//
// cStr code
//

// Includes 
#include "stdafx.h"
#include "cStr.h"
#include "windows.h"
#include <map>
#include <sstream>

// External Includes
#include "globstruct.h"
#include "..\..\Dark Basic Public Shared\Dark Basic Pro SDK\DarkSDK\Core\resource.h"
#include "..\..\Dark Basic Public Shared\Dark Basic Pro SDK\Shared\Core\SteamCheckForWorkshop.h"
#include <ctype.h>

extern "C" FILE* GG_fopen( const char* filename, const char* mode );

bool noDeleteCSTR = false;
#define STRMAXSIZE 8192
//#define STRMINSIZE 256 // some DLUA strings can easily exceed 256 characters!
#ifdef PRODUCTCLASSIC
//PE: We cant affort this in classic, when you add one object to a level:
//PE: we have around 30 cstr inside entityelement , each is constructed with s="" that preallocate STRMINSIZE.
//PE: 30*1024 = 30kb per entity.
//PE: I have set it to 128 (default) in classic, it should be fine as it will grow if needed.
//PE: Classic, everything above 100 has a very low realloc rate so use 100.
#define STRMINSIZE 100
#else
//#define STRMINSIZE 1024
//PE: The default STRMINSIZE use tons of mem, i think we fixed all bugs in here so we should be able to use a lower amount, it will increase if needed.
//#define STRMINSIZE 512
//PE: eleprof have around 620 cstr = 620 * cstr(512) = 310kb a normal level with 5000 objects = 1.4GB , and we take a backup of all entitydat  when using test game = 2.8GB.
//PE: 128 works fine, and have a low realloc rate. so 77.5kb per entity, in sample above (2.8gb), this with testgame backup only 0.73GB. used.
//PE: The above is ONLY for cstr alloc per object, each object takes up more mem, need to look at that.
#define STRMINSIZE 128
#endif

//PE: ZTEMP just use double mem, no need, use a ringbuffer instead. And we have very many of these in entityelements.
#define DISABLEZTEMP


#ifdef DISABLEZTEMP
char* m_sTemp[30] = { nullptr };
int m_iLen[30] = { 0 };
int m_iCurrentBuffer = 0;
#endif

//#define TESTUSEOFSTRINGS
#ifdef TESTUSEOFSTRINGS
int CheckPreSize[5000];
int CheckReSize[5000];
#endif

cStr::cStr ( const cStr& cString )
{

	m_size = (int) strlen ( cString.m_pString );

	#ifdef TESTUSEOFSTRINGS
	if (m_size < 5000) CheckPreSize[m_size]++;
	#endif

	//PE: Should be >= if size was exactly STRMINSIZE we would get a heap error.
	if ( m_size >= STRMINSIZE )
	{
		m_pString = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#ifndef DISABLEZTEMP
		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#endif
	}
	else
	{
		m_pString = new char [ STRMINSIZE ];
#ifndef DISABLEZTEMP
		m_szTemp = new char [ STRMINSIZE ];	
#endif
	}

	memset ( m_pString, 0, sizeof ( m_pString ) );
#ifndef DISABLEZTEMP
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
	strcpy ( m_pString, cString.m_pString );
	
}

cStr::cStr ( char* szString )
{
	m_size = (int) strlen ( szString );
#ifdef TESTUSEOFSTRINGS
	if (m_size < 5000) CheckPreSize[m_size]++;
#endif

	if ( m_size >= STRMINSIZE )
	{
		m_pString = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#ifndef DISABLEZTEMP
		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#endif
	}
	else
	{
		m_pString = new char [ STRMINSIZE ];
#ifndef DISABLEZTEMP
		m_szTemp = new char [ STRMINSIZE ];
#endif
	}

	memset ( m_pString, 0, sizeof ( m_pString ) );
#ifndef DISABLEZTEMP
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
	strcpy ( m_pString, szString );
	
}

cStr::cStr ( int iValue )
{
	m_pString = new char [ STRMINSIZE ];
	memset ( m_pString, 0, sizeof ( m_pString ) );
	sprintf ( m_pString, "%i", iValue );

#ifndef DISABLEZTEMP
	m_szTemp = new char [ STRMINSIZE ];
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
}

cStr::cStr ( float fValue )
{
	m_pString = new char [ STRMINSIZE ];
	memset ( m_pString, 0, sizeof ( m_pString ) );
	sprintf ( m_pString, "%.2f", fValue );
#ifndef DISABLEZTEMP
	m_szTemp = new char [ STRMINSIZE ];
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
}

cStr::cStr ( double dValue )
{
	m_pString = new char [ STRMINSIZE ];
	memset ( m_pString, 0, sizeof ( m_pString ) );
	sprintf ( m_pString, "%.2f", ( float ) dValue );
#ifndef DISABLEZTEMP
	m_szTemp = new char [ STRMINSIZE ];
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
}

cStr::cStr ( )
{
	m_pString = new char [ STRMINSIZE ];
	memset ( m_pString, 0, sizeof ( m_pString ) );
	strcpy ( m_pString, "" );
#ifndef DISABLEZTEMP
	m_szTemp = new char [ STRMINSIZE ];
	memset ( m_szTemp, 0, sizeof ( m_szTemp ) );
#endif
}

cStr::~cStr ( )
{
	if ( ! noDeleteCSTR )
	{
		if ( m_pString )
			delete [ ] m_pString;
#ifndef DISABLEZTEMP
		if ( m_szTemp )
			delete [] m_szTemp;  //PE: Heap error here if >256 strings and using s=s+" "
#endif
	}
}

cStr cStr::operator + ( const cStr& other )
{
	//PE: We have a bug here if using >= 256 strings and using:
	// cstr s = (large string)
	// s = s + " " //This generates a heap error.
	// s += " " // This works.

	m_size = (int) strlen ( other.m_pString ) + (int) strlen ( m_pString );
#ifdef DISABLEZTEMP

	//PE: Use a ringbuffer.
	m_iCurrentBuffer++;
	if (m_iCurrentBuffer >= 29)
		m_iCurrentBuffer = 0;

	if (m_iLen[m_iCurrentBuffer] <= (m_size + 50) || !m_sTemp[m_iCurrentBuffer])
	{
		if(m_sTemp[m_iCurrentBuffer])
			delete[] m_sTemp[m_iCurrentBuffer];
		m_sTemp[m_iCurrentBuffer] = new char[sizeof(char) * ((m_size) + 51)];
		m_iLen[m_iCurrentBuffer] = (m_size + 50);
	}

	if (m_sTemp[m_iCurrentBuffer])
	{
		if (m_pString)
			strcpy(m_sTemp[m_iCurrentBuffer], m_pString);
		else
			m_sTemp[m_iCurrentBuffer][0] = 0;

		if (other.m_pString)
			strcat(m_sTemp[m_iCurrentBuffer], other.m_pString);

		return cStr(m_sTemp[m_iCurrentBuffer]);
	}
	else
		return(cStr("m_sTemp[m_iCurrentBuffer] alloc failed.")); //PE: Debug only should never happen.

#else
	if ( m_size >= STRMINSIZE )
	{
#ifdef TESTUSEOFSTRINGS
		if (m_size < 5000) CheckReSize[m_size]++;
#endif

		if ( m_szTemp )
			delete [] m_szTemp;

		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
	}

	if ( m_pString )
		strcpy ( m_szTemp, m_pString );

	if ( other.m_pString )
		strcat ( m_szTemp, other.m_pString );

	return cStr(m_szTemp);
#endif

}

cStr& cStr::operator += ( const cStr& other )
{

	m_size = (int) strlen ( other.m_pString ) + (int) strlen ( m_pString );

	if ( m_size >= STRMINSIZE )
	{
#ifdef TESTUSEOFSTRINGS
		if (m_size < 5000) CheckReSize[m_size]++;
#endif

		char* newstring = new char [ sizeof ( char ) * ( m_size ) + 1 ];
		strcpy ( newstring , m_pString );
		delete[] m_pString;
		m_pString = newstring;
#ifndef DISABLEZTEMP
		delete[] m_szTemp;
		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#endif
	}

	if ( other.m_pString )
		strcat ( m_pString, other.m_pString );

	return *this;
}
		
cStr cStr::operator = ( const cStr& other )
{
	m_size = (int) strlen ( other.m_pString );

	if ( m_size >= STRMINSIZE )
	{
#ifdef TESTUSEOFSTRINGS
		if (m_size < 5000) CheckReSize[m_size]++;
#endif

		char* newstring = new char [ sizeof ( char ) * ( m_size ) + 1 ];
		delete[] m_pString;
		m_pString = newstring;
#ifndef DISABLEZTEMP
		delete[] m_szTemp;
		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#endif
	}

	strcpy ( m_pString, other.m_pString );

	//return *this;
	return cStr ( other.m_pString );
}

cstr global_string_error = "Error in cStr ? "; //PE: Remove warning.

cStr& cStr::operator = ( const char* other )
{
	if(!m_pString || !other)
	{
		//PE: Strange. had this, but after a recomile it worked again, what was wrong ? keep it here with breakpoint just in case.
		return(global_string_error);
	}
	m_size = (int) strlen ( other );

	if ( m_size >= STRMINSIZE )
	{
#ifdef TESTUSEOFSTRINGS
		if (m_size < 5000) CheckReSize[m_size]++;
#endif
		char* newstring = new char [ sizeof ( char ) * ( m_size ) + 1 ];
		delete[] m_pString;
		m_pString = newstring;
#ifndef DISABLEZTEMP
		delete[] m_szTemp;
		m_szTemp = new char [ sizeof ( char ) * ( m_size ) + 1 ];
#endif
	}
	//PE: m_pString was 0 here ? called from line (t.tfile_s="audiobank\\materials\\materialdefault.txt";) ?
	strcpy ( m_pString, other );

	return *this;
}

bool cStr::operator == ( const cStr& s )
{
	if ( m_pString )
		if ( strcmp ( s.m_pString , m_pString ) == 0 ) 
			return true;

	return false;
}
bool cStr::operator == ( const char* s )
{
	if ( m_pString )
		if ( strcmp ( s , m_pString ) == 0 )
			return true;

	return false;
}

bool cStr::operator != ( const cStr& s )
{
	if ( m_pString )
		if ( strcmp ( s.m_pString , m_pString ) != 0 )
			return true;
	
	return false;
}

bool cStr::operator != ( const char* s )
{
	if ( m_pString )
		if ( strcmp ( s, m_pString ) != 0 )
			return true;

	return false;
}

int cStr::Len ( void )
{
	if ( m_pString )
		return (int) strlen ( m_pString );

	return 0;
}

char* cStr::Get ( void )
{
	return m_pString;
}

int cStr::Val ( void )
{
	if ( m_pString )
		return atoi ( m_pString );

	return 0;
}

float cStr::ValF ( void )
{
	if ( m_pString )
		return (float)atof ( m_pString );

	return 0.0f;
}

cStr cStr::Upper ( void )
{
	char szTemp [STRMAXSIZE] = "";
	
	if ( m_pString )
	{
		for ( int i = 0; i < (int)strlen ( m_pString ); i++ )
			szTemp [ i ] = toupper ( m_pString [ i ] );
	}

	return cStr ( szTemp );
}

cStr cStr::Lower ( void )
{
	char szTemp [STRMAXSIZE] = "";
	
	if ( m_pString )
	{
		for ( int i = 0; i < (int)strlen ( m_pString ); i++ )
			szTemp [ i ] = tolower ( m_pString [ i ] );
	}

	return cStr ( szTemp );
}

cStr cStr::Left ( int iCount )
{
	char szTemp [STRMAXSIZE] = "";
	
	if ( m_pString && iCount < (int)strlen ( m_pString ) )
	{
		for ( int i = 0; i <= iCount; i++ )
			szTemp [ i ] = m_pString [ i ];
	}

	return cStr ( szTemp );
}

cStr cStr::Right ( int iCount )
{
	char szTemp [STRMAXSIZE] = "";
	
	if ( m_pString && iCount >= 0 )
	{
		for ( int i = (int)strlen ( m_pString ) - iCount, j = 0; i < (int)strlen ( m_pString ); i++ )
			szTemp [ j++ ] = m_pString [ i ];
	}

	return cStr ( szTemp );
}

cStr cStr::Mid ( int iPoint )
{
	char szTemp [STRMAXSIZE] = "";
	
	if ( m_pString && iPoint >= 0 && iPoint < (int)strlen ( m_pString ) )
		szTemp [ 0 ] = m_pString [ iPoint ];

	return cStr ( szTemp );
}

#ifdef bent
char cStr::work[2048] = "";

char* cStr::Get ( void )
{
	if ( !pString )
	{
		pString = new char [ sizeof ( char ) * 1024 ];
		strcpy ( pString , "" );
		iSize = 0;
	}
	return pString;
}

cStr::cStr ( )
{
	pString = new char [ sizeof ( char ) * 1024 ];
	strcpy ( pString , "" );
	iSize = 0;
}

cStr::cStr ( char* szSet )
{

	if ( strlen ( szSet ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * strlen ( szSet ) + 1 ];

	strcpy ( pString, szSet );

	iSize = strlen ( szSet );
}

cStr::cStr ( int iSet )
{
	sprintf ( work , "%i" , iSet );
	if ( strlen ( work ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * strlen ( work ) + 1 ];
	strcpy ( pString, work );

	iSize = strlen ( work );
}

cStr::cStr ( float fSet )
{
	sprintf ( work , "%.3f" , fSet );
	if ( strlen ( work ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * strlen ( work ) + 1 ];

	strcpy ( pString, work );

	iSize = strlen ( work );
}

cStr::~cStr ( )
{
	/*if ( pString ) 
	{
		delete[] pString;
		pString = NULL;
	}*/
}

/*const cStr &cStr::operator = ( const cstr& Equal )
{
	if ( pString ) delete[] pString;

	if ( strlen ( Equal.pString ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * (strlen ( Equal.pString ) + 1) ];

	strcpy ( pString, Equal.pString );

	iSize = strlen ( pString );

	return *this;
}*/

const cStr &cStr::operator = ( const char* szEqual )
{
	if ( pString ) delete[] pString;

	if ( strlen ( szEqual ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * (strlen ( szEqual ) + 1) ];

	strcpy ( pString, szEqual );

	iSize = strlen ( pString );

	return *this;
}

const cStr &cStr::operator = ( const unsigned long pDWEqual )
{
	char* szEqual = (char*)pDWEqual;

	if ( pString ) delete[] pString;

	if ( strlen ( szEqual ) < 1024 )
		pString = new char [ sizeof ( char ) * 1024 ];
	else
		pString = new char [ sizeof ( char ) * (strlen ( szEqual ) + 1) ];

	strcpy ( pString, szEqual );

	iSize = strlen ( pString );

	delete [] szEqual;

	return *this;
}

cStr &cStr::operator + ( const cStr &add )
{
	char* ret = "";

	if ( pString && add.pString )
	{

		if ( strlen ( pString ) + strlen ( add.pString ) < 1024 )
			ret = new char [ sizeof ( char ) * 1024 ];
		else
			ret = new char[ sizeof ( char ) * ( strlen ( pString ) + strlen ( add.pString ) + 1 ) ];

		strcpy ( ret, pString );
		strcat ( ret, add.pString );

		delete [] pString;
		pString = ret;

	}

	iSize = strlen ( pString );

	return *this;
}

cStr &cStr::operator + ( const unsigned long pDWAdd )
{
	char* pAdd = (char*)pDWAdd;
	char* ret = "";

	if ( pString && pAdd )
	{
		if ( strlen ( pString ) + strlen ( pAdd ) < 1024 )
			ret = new char [ sizeof ( char ) * 1024 ];
		else
			ret = new char[ sizeof ( char ) * ( strlen ( pString ) + strlen ( pAdd ) + 1 ) ];

		strcpy ( ret, pString );
		strcat ( ret, pAdd );

		delete [] pAdd;

		delete [] pString;
		pString = ret;
	}

	iSize = strlen ( pString );

	return *this;
}

cStr &cStr::operator += ( const cStr &add )
{

	if ( pString )
	{
		char* p;

		if ( strlen ( pString ) + strlen ( add.pString ) < 1024 )
			p = new char [ sizeof ( char ) * 1024 ];
		else
			p = new char[ sizeof ( char ) * ( strlen ( pString ) + strlen ( add.pString ) ) + 1 ];

		strcpy ( p , pString );
		strcat ( p , add.pString );

		delete [] pString;
		pString = p;

		iSize = strlen ( pString );

	}
	return *this;
}

cStr &cStr::operator += ( const unsigned long pDWAdd )
{
	char* pAdd = (char*)pDWAdd;

	if ( pString && pAdd )
	{
		char* p;

		if ( strlen ( pString ) + strlen ( pAdd ) < 1024 )
			p = new char [ sizeof ( char ) * 1024 ];
		else
			p = new char[ sizeof ( char ) * ( strlen ( pString ) + strlen ( pAdd ) ) + 1 ];

		strcpy ( p , pString );
		strcat ( p , pAdd );
		
		delete pAdd;

		delete [] pString;
		pString = p;

		iSize = strlen ( pString );

	}
	return *this;
}


bool cStr::operator == ( const cStr& s ) const
{
	if ( _strnicmp ( s.pString , pString , strlen(s.pString) ) == 0 ) return true;
	return false;
}

bool cStr::operator == ( const char* s ) const
{
	if ( _strnicmp ( s , pString , strlen(s) ) == 0 ) return true;
	return false;
}

bool cStr::operator != ( const cStr& s ) const
{
	if ( _strnicmp ( s.pString , pString , strlen(s.pString) ) != 0 ) return true;
	return false;
}

bool cStr::operator != ( const char* s ) const
{
	if ( _strnicmp ( s , pString , strlen(s) ) != 0 ) return true;
	return false;
}
#endif

/*int ArrayCount ( std::vector<cstr> array )
{
	return array.size()-1;
}*/

int ArrayCount2 ( std::vector<std::vector<cstr>> array )
{
	return ( (int) array.size()* (int) array[0].size()) - 1;
}

cstr ArrayAt ( std::vector<std::vector<cstr>>& array, int l )
{
	int a,b;
	b = (int)(floor((float)l / (float)array.size()));
	a = l - (b* (int) array.size());
	return array[a][b];
}


void SaveArray ( char* filename , std::vector<cstr> array )
{
	FILE* file;
	char t[2048];

	DeleteFileA ( filename );

	file = GG_fopen ( filename , "w" );

	if ( file )
	{
		for ( int c = 0 ; c < (int)array.size() ; c++ )
		{
			strcpy ( t , array[c].Get() );
			strcat ( t , "\n" );
			fputs ( t , file );
		}
		fclose ( file );
	}

}

void SaveArray ( char* filename , std::vector<std::vector<cstr>> array )
{
	FILE* file;
	char t[2048];
	int size = (int) array[0].size();

	DeleteFileA ( filename );

	file = GG_fopen ( filename , "w" );

	if ( file )
	{		

		for ( int b = 0 ; b <size ; b++ )
		{
			for ( int a = 0 ; a < (int)array.size() ; a++ )
			{
				strcpy ( t , array[a][b].Get() );
				strcat ( t , "\n" );
				fputs ( t , file );
			}
		}

		fclose ( file );
	}

}

void LoadArray ( char* filename , std::vector<cstr>& array )
{
	FILE* file;
	char t[2048];
	int lineCount = 0;

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, filename);
	//g_pGlob->UpdateFilenameFromVirtualTable( VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( VirtualFilename );

	file = GG_fopen ( VirtualFilename , "r" );

	// first we see how big the file is to ensure array is big enough
	if ( file )
	{
		while ( !feof(file) )
		{
			fgets ( t , 2047 , file );
			lineCount++;
		}

		// if the last line is blank, remove it
		if ( strcmp ( t , "" ) == 0 || strcmp ( t , "\n" ) == 0 )
			lineCount--;

		fclose (file);

		if ( lineCount >= (int)array.size() ) Dim ( array , lineCount );

		file = GG_fopen ( VirtualFilename , "r" );
		if ( file )
		{
			for ( int c = 0 ; c < lineCount ; c++ )
			{
				fgets ( t , 2047 , file );
				if ( t[strlen(t)-1] == '\n' ) t[strlen(t)-1] = '\0';
				array[c] = t;
			}
			fclose ( file );
		}
	}

	g_pGlob->Encrypt( VirtualFilename );

}

void LoadArray ( char* filename , std::vector<std::vector<cstr>>& array )
{
	FILE* file;
	char t[2048];
	int lineCount = 0;
	int size = (int) array[0].size();

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, filename);
	//g_pGlob->UpdateFilenameFromVirtualTable( VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( VirtualFilename );

	file = GG_fopen ( VirtualFilename , "r" );

	if ( file )
	{

		file = GG_fopen ( VirtualFilename , "r" );
		if ( file )
		{
			for ( int b = 0 ; b < size ; b++ )
			{
				for ( int a = 0 ; a < (int)array.size() ; a++ )
				{
					fgets ( t , 2047 , file );
					if ( t[strlen(t)-1] == '\n' ) t[strlen(t)-1] = '\0';
					array[a][b] = t;
				}
			}
			fclose ( file );
		}
	}

	g_pGlob->Encrypt( VirtualFilename );

}

// MATHEMATICAL COMMANDS
extern double gDegToRad;
extern double gRadToDeg;
float atan2deg( float fA , float fB ) { return (float)(atan2(fA, fB)*gRadToDeg); }

// Other string functions
void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

#ifdef WICKEDENGINE
std::string target;
std::string removeChars(std::string s, int pos, bool f(char c))
{
	if (pos >= s.size()) return s;
	std::string::iterator end = remove_if(s.begin() + pos, s.end(), f);
	return s.substr(0, end - s.begin());
}
bool isIn(char c) { return target.find(toupper(c)) != std::string::npos; }
bool notAlpha(char c) { return !isalpha(c); }
bool notAlphaPlusSpace(char c) { return c != ' ' && !isalpha(c); }

std::string soundex(std::string word)
{
	std::map<char, char> cmap = { {'B','1'}, {'F','1'}, {'P','1'}, {'V','1'},
							{'C','2'}, {'G','2'}, {'J','2'}, {'K','2'}, {'Q','2'}, {'S','2'}, {'X','2'}, {'Z','2'},
							{'D','3'}, {'T','3'},
							{'L','4'},
							{'M','5'}, {'N','5'},
							{'R','6'} };


	// Remove non-alphabetic characters
	std::string result = removeChars(word, 0, notAlpha);

	// Put in upper case
	for (char &c : result) c = toupper(c);

	// Store first character
	char firstLetter = result[0];

	// Remove all occurrences of H and W except first letter
	target = "HW";   result = removeChars(result, 1, isIn);

	// Map consonants to digits
	for (char &c : result) { if (cmap.count(c)) c = cmap[c]; }

	// Replace all adjacent same digits with one digit
	for (int i = 1; i < result.size(); i++) if (result[i] == result[i - 1]) result[i - 1] = '*';
	target = "*";   result = removeChars(result, 1, isIn);

	// Remove all occurrences of AEIOUY except first letter
	target = "AEIOUY";   result = removeChars(result, 1, isIn);

	// Replace first letter
	result[0] = firstLetter;

	// Get correct length and return
	result += "000";
	return result.substr(0, 4);
}

std::string soundexall(std::string all)
{
	using namespace std;
	string result = "";
	string search = removeChars(all, 0, notAlphaPlusSpace);
	istringstream iss(search);
	vector<string> tokens{ istream_iterator<string>{iss},istream_iterator<string>{} };
	for (int i = 0; i < tokens.size(); i++)
	{
		result += soundex(tokens[i]);
		if (i < tokens.size()-1)
			result += " ";
	}
	return result;
}
#endif


