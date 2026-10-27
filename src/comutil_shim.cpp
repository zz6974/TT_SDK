// MinGW 没有 MSVC 的 comsupp.lib，_com_util::ConvertStringToBSTR / ConvertBSTRToString
// 只在 comutil.h 中声明、未提供实现。此处补齐实现，供 _bstr_t / _variant_t 使用。
#include <comutil.h>

namespace _com_util {

// 返回的 BSTR 由调用方用 SysFreeString 释放（与 comutil.h 内联代码的约定一致）
BSTR WINAPI ConvertStringToBSTR(const char* pSrc)
{
    if (pSrc == NULL)
        return NULL;

    // 含结尾 NUL 的宽字符数
    int cwch = ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
    if (cwch <= 0)
        return NULL;

    BSTR wsOut = ::SysAllocStringLen(NULL, cwch - 1);
    if (wsOut != NULL)
        ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, wsOut, cwch);
    return wsOut;
}

// _bstr_t 内部用 delete[] 释放返回值，因此必须用 new[] 分配
char* WINAPI ConvertBSTRToString(BSTR pSrc)
{
    if (pSrc == NULL)
        return NULL;

    int cb = ::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
    if (cb <= 0)
        return NULL;

    char* szOut = new char[cb];
    if (szOut != NULL)
        ::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, szOut, cb, NULL, NULL);
    return szOut;
}

} // namespace _com_util
