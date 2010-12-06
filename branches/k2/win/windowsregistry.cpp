
//advapi32.lib

#include "windowsregistry.h"

BOOL DeleteSubkey(HKEY hkey, LPCTSTR pcszSubkey);

WindowsRegistry::WindowsRegistry()
{
    m_hkey = NULL;
}

WindowsRegistry::~WindowsRegistry()
{
    Close();
}

bool WindowsRegistry::Open(HKEY hkeyParent, const TCHAR* pcszSubkey, bool fCreateIfNew)
{
    if (fCreateIfNew)
    {
        if (RegCreateKeyEx(hkeyParent, pcszSubkey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hkey, &m_dwDisposition) != ERROR_SUCCESS)
        {
            return false;
        }
    }
    else
    {
        if (RegOpenKeyEx(hkeyParent, pcszSubkey, 0, KEY_ALL_ACCESS, &m_hkey) != ERROR_SUCCESS)
        {
            m_hkey = 0;
            return false;
        }
    }
    return true;
}

bool WindowsRegistry::Close()
{
    if (m_hkey)
        RegCloseKey(m_hkey);
    m_hkey = NULL;
    return true;
}

bool WindowsRegistry::ReadBinary(const TCHAR* pcszKey, LPBYTE pData, LPDWORD lpcbData)
{
    DWORD dwType = REG_BINARY;
    return (RegQueryValueEx(m_hkey, pcszKey, 0, &dwType, (PBYTE)&pData, lpcbData) == ERROR_SUCCESS);
}

DWORD WindowsRegistry::ReadDWORD(const TCHAR* pcszKey, DWORD dwDefault)
{
    DWORD dwValue;
    DWORD dwValueSize = sizeof(DWORD);
    DWORD dwType = REG_DWORD;

    if (RegQueryValueEx(m_hkey, pcszKey, 0, &dwType, (PBYTE)&dwValue, &dwValueSize) == ERROR_SUCCESS)
        return dwValue;

    if (dwDefault != 0xDEF0)	// Default specified
        RegSetValueEx(m_hkey, pcszKey, 0, REG_DWORD, (PBYTE)&dwDefault, sizeof(DWORD));

    return dwDefault;
}

TCHAR* WindowsRegistry::ReadString(const TCHAR* pcszKey, TCHAR* value, const TCHAR* pcszDefault)
{
    if (!value)
        return NULL;
    _tcscpy_s(value, MAX_PATH, _T(""));

    DWORD dwDataSize = 0;
    DWORD dwType = REG_SZ;

    if (RegQueryValueEx(m_hkey, pcszKey, 0, &dwType, (PBYTE)NULL, &dwDataSize) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(m_hkey, pcszKey, 0, &dwType, (PBYTE)value, &dwDataSize) == ERROR_SUCCESS)
        {
            return value;
        }
    }

    if (pcszDefault)
    {
        if (!WriteString(pcszKey, pcszDefault))
            _tcscpy_s(value, MAX_PATH, pcszDefault);
        return value;
    }

    return NULL;
}

bool WindowsRegistry::WriteBinary(const TCHAR* pcszKey, LPBYTE pData, DWORD cbData)
{
    return (RegSetValueEx(m_hkey, pcszKey, 0, REG_BINARY, (PBYTE)pData, cbData) == ERROR_SUCCESS);
}

bool WindowsRegistry::WriteDWORD(const TCHAR* pcszKey, DWORD dwValue)
{
    return (RegSetValueEx(m_hkey, pcszKey, 0, REG_DWORD, (PBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS);
}

bool WindowsRegistry::WriteString(const TCHAR* pcszKey, const TCHAR* pcszValue)
{
    return (RegSetValueEx(m_hkey, pcszKey, 0, REG_SZ, (PBYTE)pcszValue, (_tcslen(pcszValue) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS);
}

bool WindowsRegistry::DeleteSubkey(LPCTSTR pcszSubkey)
{
    return (::DeleteSubkey(m_hkey, pcszSubkey) != 0);
}

bool WindowsRegistry::DeleteValue(LPCTSTR pcszValueName)
{
    return (RegDeleteValue(m_hkey, pcszValueName) == ERROR_SUCCESS);
}

// Recursively delete a key in the registry
BOOL DeleteSubkey(HKEY hkey, LPCTSTR pcszSubkey)
{
    if (!pcszSubkey)
        return FALSE;
    else
    {
        // Recursively delete any subkeys for the target subkey
        HKEY	hkeySubkey;
        TCHAR	pszKey[MAX_PATH];

        DWORD dwRC = RegOpenKeyEx(hkey, pcszSubkey, 0, KEY_ALL_ACCESS, &hkeySubkey);

        if (dwRC != ERROR_SUCCESS)
        {
            return FALSE;
        }

        DWORD	dwNameSize = MAX_PATH;

        while (RegEnumKeyEx(hkeySubkey, 0, pszKey, &dwNameSize, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            if (! DeleteSubkey(hkeySubkey, pszKey))
            {
                RegCloseKey(hkeySubkey);
                return FALSE;
            }

            dwNameSize = MAX_PATH;
        }

        RegCloseKey(hkeySubkey);
    }

    return (RegDeleteKey(hkey, pcszSubkey) == ERROR_SUCCESS);
}


bool WindowsRegistry::GetFirstSubkey(TCHAR* pszKey)
{
    DWORD	dwNameSize = MAX_PATH;

    m_nSubkeyIndex = 0;

    if (RegEnumKeyEx(m_hkey, m_nSubkeyIndex, pszKey, &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
    {
        _tcscpy(pszKey, _T(""));
        return FALSE;	// No subkeys
    }

    return TRUE;
}

bool WindowsRegistry::GetNextSubkey(TCHAR* pszKey)
{
    DWORD	dwNameSize = MAX_PATH;

    m_nSubkeyIndex++;

    if (RegEnumKeyEx(m_hkey, m_nSubkeyIndex, pszKey, &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
    {
        _tcscpy_s(pszKey, MAX_PATH, _T(""));
        return FALSE;	// No more subkeys
    }

    return TRUE;
}

bool WindowsRegistry::GetSubkey(TCHAR* pszKey, int nOffset)
{
    DWORD	dwNameSize = MAX_PATH;
    LONG	rc;

    m_nSubkeyIndex = nOffset;

    rc = RegEnumKeyEx(m_hkey, m_nSubkeyIndex, pszKey, &dwNameSize, 0, NULL, NULL, NULL);

    if (rc != ERROR_SUCCESS)
    {
        if (rc != ERROR_NO_MORE_ITEMS)
        {
        }
        _tcscpy_s(pszKey, MAX_PATH, _T(""));
        return FALSE;	// No subkeys
    }

    return TRUE;
}

bool WindowsRegistry::GetFirstValue(TCHAR* pszValueName)
{
    DWORD	dwNameSize = MAX_PATH;

    m_nSubkeyIndex = 0;

    if (RegEnumValue(m_hkey, m_nSubkeyIndex, pszValueName, &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
    {
        return FALSE;	// No subkeys
    }
    return TRUE;
}

bool WindowsRegistry::GetNextValue(TCHAR* pszValueName)
{
    DWORD	dwNameSize = MAX_PATH;

    m_nSubkeyIndex++;

    if (RegEnumValue(m_hkey, m_nSubkeyIndex, pszValueName, &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
    {
        return FALSE;	// No subkeys
    }

    return TRUE;
}
