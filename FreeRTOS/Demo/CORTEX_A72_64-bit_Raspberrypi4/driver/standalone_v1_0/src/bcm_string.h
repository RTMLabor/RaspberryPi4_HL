#ifndef BCM_STRING_H
#define BCM_STRING_H
#if 0
#include "bcm_types.h"


#define REDERROR()      ((void)0)

size_t strlen(const char *pszStr);
int32_t strncmp(const char *pszStr1, const char *pszStr2);

#ifndef StrLenUnchecked
static uint32_t StrLenUnchecked(const char *pszStr);
#endif
#ifndef StrCmpUnchecked
static int32_t StrCmpUnchecked(const char *pszStr1, const char *pszStr2);
#endif
#ifndef StrNCmpUnchecked
static int32_t StrNCmpUnchecked(const char *pszStr1, const char *pszStr2, uint32_t ulLen);
#endif
#ifndef StrNCpyUnchecked
static void StrNCpyUnchecked(char *pszDst, const char *pszSrc, uint32_t ulLen);
#endif

#endif

#endif