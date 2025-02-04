// Alternative zu Redconf (Reliance-Edge)


#if 0
#include "bcm_string.h"

/** @brief Determine the length (in bytes) of a null terminated string.

    The length does not include the null terminator byte.

    @param pszStr   The null terminated string whose length is to be determined.

    @return The length of the @p pszStr string.
*/
#undef strlen
size_t strlen(
    const char *pszStr)
{
    uint32_t    ulLen;

    if(pszStr == NULL)
    {
        REDERROR();
        ulLen = 0U;
    }
    else
    {
        /*  Cast the result to uint32_t, since StrLenUnchecked() might be
            strlen(), which returns size_t, which is possibly a 64-bit value.
        */
        ulLen = (uint32_t)StrLenUnchecked(pszStr);
    }

    return ulLen;
}


#ifndef StrLenUnchecked
/** @brief Determine the length (in bytes) of a null terminated string.

    @param pszStr   The null terminated string whose length is to be determined.

    @return The length of the @p pszStr string.
*/
static uint32_t StrLenUnchecked(
    const char *pszStr)
{
    uint32_t    ulLen = 0U;

    while(pszStr[ulLen] != '\0')
    {
        ulLen++;
    }

    return ulLen;
}
#endif


/** @brief Compare two null terminated strings.

    @param pszStr1  The first string to compare.
    @param pszStr2  The second string to compare.

    @return Zero if the two strings are the same, otherwise nonzero.

    @retval 0   @p pszStr1 and @p pszStr2 are the same.
    @retval 1   @p pszStr1 is greater than @p pszStr2, as determined by the
                values of the first differing bytes.
    @retval -1  @p pszStr2 is greater than @p pszStr1, as determined by the
                values of the first differing bytes.
*/
#undef strncmp
int32_t strncmp(
    const char *pszStr1,
    const char *pszStr2)
{
    int32_t     lResult;

    if((pszStr1 == NULL) || (pszStr2 == NULL))
    {
        REDERROR();
        lResult = 0;
    }
    else
    {
        lResult = StrCmpUnchecked(pszStr1, pszStr2);
    }

    return lResult;
}


#ifndef StrCmpUnchecked
/** @brief Compare two null terminated strings.

    @param pszStr1  The first string to compare.
    @param pszStr2  The second string to compare.

    @return Zero if the two strings are the same, otherwise nonzero.
*/
static int32_t StrCmpUnchecked(
    const char *pszStr1,
    const char *pszStr2)
{
    int32_t     lResult;
    uint32_t    ulIdx = 0U;

    while((pszStr1[ulIdx] == pszStr2[ulIdx]) && (pszStr1[ulIdx] != '\0'))
    {
        ulIdx++;
    }

    /*  "The sign of a non-zero return value is determined by the sign of the
        difference between the values of the first pair of bytes (both
        interpreted as type unsigned char) that differ in the strings being
        compared."  Use uint8_t instead of unsigned char to avoid MISRA C
        deviations.
    */
    if((uint8_t)pszStr1[ulIdx] > (uint8_t)pszStr2[ulIdx])
    {
        lResult = 1;
    }
    else if((uint8_t)pszStr1[ulIdx] < (uint8_t)pszStr2[ulIdx])
    {
        lResult = -1;
    }
    else
    {
        lResult = 0;
    }

    return lResult;
}
#endif

#endif