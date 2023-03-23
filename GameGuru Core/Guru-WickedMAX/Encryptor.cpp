// Encryptor.cpp: implementation of the CEncryptor class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "Encryptor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncryptor::CEncryptor(DWORD dwUniqueKeyValue)
{
	m_dwUniqueKey=dwUniqueKeyValue;
}

CEncryptor::~CEncryptor()
{

}

bool CEncryptor::EncryptFileData(LPSTR filebuffer, DWORD filebuffersize, bool bEncryptIfTrue)
{
	// wouldn't you like to know!
	return true;
}