
#pragma once

#include "config_win.h"

class WindowsRegistry
{
public:
    WindowsRegistry();
    ~WindowsRegistry();

    bool Open(HKEY hkeyParent, const TCHAR* pcszSubkey, bool fCreateIfNew = TRUE);
    bool Close();
    bool ReadBinary(const TCHAR* pcszKey, LPBYTE pData, LPDWORD lpcbData);
    DWORD ReadDWORD(const TCHAR* pcszKey, DWORD dwDefault = 0xDEF0);
    TCHAR* ReadString(const TCHAR* pcszKey, TCHAR* value, const TCHAR* pcszDefault = NULL);
    bool WriteBinary(const TCHAR* pcszKey, LPBYTE pData, DWORD cbData);
    bool WriteDWORD(const TCHAR* pcszKey, DWORD dwValue);
    bool WriteString(const TCHAR* pcszKey, const TCHAR* pcszValue);
    bool DeleteSubkey(LPCTSTR pcszSubkey);
    bool DeleteValue(LPCTSTR pcszValueName);

    bool GetFirstSubkey(TCHAR* pszKey);
    bool GetFirstValue(TCHAR* pszValueName);
    bool GetNextSubkey(TCHAR* pszKey);
    bool GetNextValue(TCHAR* pszValueName);
    bool GetSubkey(TCHAR* pszKey, int nOffset);

    operator HKEY()	{ return m_hkey; }
    operator HKEY*() { return &m_hkey; }

protected:
    HKEY m_hkey;
    DWORD m_dwDisposition;
    int m_nSubkeyIndex;
    TCHAR* m_pszKey;
};
