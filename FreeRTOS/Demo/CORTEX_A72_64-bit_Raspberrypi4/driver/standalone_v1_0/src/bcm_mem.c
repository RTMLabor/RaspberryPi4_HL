/*
 * An memcpy() in C which does not use the FPU registers, as the default does.
 */


#include <stdint.h>
#include "bcm_mem.h"


#if( SIMPLE_MEMCPY != 0 )
#undef memcpy
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes )
{
unsigned char *pcDest = ( unsigned char * ) pvDest, *pcSource = ( unsigned char * ) pvSource;
size_t x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = *pcSource;
		pcDest++;
		pcSource++;
	}

	return pvDest;
}
/*-----------------------------------------------------------*/
#endif /* SIMPLE_MEMCPY */

union xPointer {
	uint8_t *u8;
	uint16_t *u16;
	uint32_t *u32;
	uint32_t uint32;
};


#if( SIMPLE_MEMCPY == 0 )
#undef memcpy
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes )
{
union xPointer pxDestination;
union xPointer pxSource;
union xPointer pxLastSource;
uint32_t ulAlignBits;

	pxDestination.u8 = ( uint8_t * ) pvDest;
	pxSource.u8 = ( uint8_t * ) pvSource;
	pxLastSource.u8 = pxSource.u8 + ulBytes;

	ulAlignBits = ( pxDestination.uint32 & 0x03 ) ^ ( pxSource.uint32 & 0x03 );

	if( ( ulAlignBits & 0x01 ) == 0 )
	{
		if( ( ( pxSource.uint32 & 1 ) != 0 ) && ( pxSource.u8 < pxLastSource.u8 ) )
		{
			*( pxDestination.u8++ ) = *( pxSource.u8++) ;
		}
		/* 16-bit aligned here */
		if( ( ulAlignBits & 0x02 ) != 0 )
		{
			uint32_t extra = pxLastSource.uint32 & 0x01ul;

			pxLastSource.uint32 &= ~0x01ul;

			while( pxSource.u16 < pxLastSource.u16 )
			{
				*( pxDestination.u16++ ) = *( pxSource.u16++) ;
			}

			pxLastSource.uint32 |= extra;
		}
		else
		{
			int iCount;
			uint32_t extra;

			if( ( ( pxSource.uint32 & 2 ) != 0 ) && ( pxSource.u8 < pxLastSource.u8 - 1 ) )
			{
				*( pxDestination.u16++ ) = *( pxSource.u16++) ;
			}
			// 32-bit aligned
			extra = pxLastSource.uint32 & 0x03ul;

			pxLastSource.uint32 &= ~0x03ul;
			iCount = pxLastSource.u32 - pxSource.u32;
			while( iCount > 8 )
			{
				/* Copy 32 bytes */
				/* Normally it doesn't make sense to make this list much longer because
				the indexes will get too big, and therefore longer instructions are needed. */
				pxDestination.u32[ 0 ] = pxSource.u32[ 0 ];
				pxDestination.u32[ 1 ] = pxSource.u32[ 1 ];
				pxDestination.u32[ 2 ] = pxSource.u32[ 2 ];
				pxDestination.u32[ 3 ] = pxSource.u32[ 3 ];
				pxDestination.u32[ 4 ] = pxSource.u32[ 4 ];
				pxDestination.u32[ 5 ] = pxSource.u32[ 5 ];
				pxDestination.u32[ 6 ] = pxSource.u32[ 6 ];
				pxDestination.u32[ 7 ] = pxSource.u32[ 7 ];
				pxDestination.u32 += 8;
				pxSource.u32 += 8;
				iCount -= 8;
			}

			while( pxSource.u32 < pxLastSource.u32 )
			{
				*( pxDestination.u32++ ) = *( pxSource.u32++) ;
			}

			pxLastSource.uint32 |= extra;
		}
	}
	else
	{
		/* This it the worst alignment, e.g. 0x80000 and 0xA0001,
		only 8-bits copying is possible. */
		int iCount = pxLastSource.u8 - pxSource.u8;
		while( iCount > 8 )
		{
			/* Copy 8 bytes the hard way */
			pxDestination.u8[ 0 ] = pxSource.u8[ 0 ];
			pxDestination.u8[ 1 ] = pxSource.u8[ 1 ];
			pxDestination.u8[ 2 ] = pxSource.u8[ 2 ];
			pxDestination.u8[ 3 ] = pxSource.u8[ 3 ];
			pxDestination.u8[ 4 ] = pxSource.u8[ 4 ];
			pxDestination.u8[ 5 ] = pxSource.u8[ 5 ];
			pxDestination.u8[ 6 ] = pxSource.u8[ 6 ];
			pxDestination.u8[ 7 ] = pxSource.u8[ 7 ];
			pxDestination.u8 += 8;
			pxSource.u8 += 8;
			iCount -= 8;
		}
	}
	while( pxSource.u8 < pxLastSource.u8 )
	{
		*( pxDestination.u8++ ) = *( pxSource.u8++ );
	}
	return pvDest;
}
#endif /* SIMPLE_MEMCPY == 0 */
/*-----------------------------------------------------------*/

#if( SIMPLE_MEMSET != 0 )
#undef *memset
void *memset( void *pvDest, int iValue, size_t ulBytes )
{
unsigned char *pcDest = ( unsigned char * ) pvDest;
size_t x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = ( unsigned char ) iValue;
		pcDest++;
	}

	return pvDest;
}
#endif /* SIMPLE_MEMSET != 0 */
/*-----------------------------------------------------------*/


#if( SIMPLE_MEMSET == 0 )
#undef memset
void *memset(void *dest, int c, size_t n)
{
    unsigned char *s = dest;
    size_t k;

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dest region. */

    if (!n) return dest;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dest;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dest;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    u32 c32 = ((u32)-1)/255 * (unsigned char)c;

    /* In preparation to copy 32 bytes at a time, aligned on
     * an 8-byte bounary, fill head/tail up to 28 bytes each.
     * As in the initial byte-based head/tail fill, each
     * conditional below ensures that the subsequent offsets
     * are valid (e.g. !(n<=24) implies n>=28). */

    *(u32 *)(s+0) = c32;
    *(u32 *)(s+n-4) = c32;
    if (n <= 8) return dest;
    *(u32 *)(s+4) = c32;
    *(u32 *)(s+8) = c32;
    *(u32 *)(s+n-12) = c32;
    *(u32 *)(s+n-8) = c32;
    if (n <= 24) return dest;
    *(u32 *)(s+12) = c32;
    *(u32 *)(s+16) = c32;
    *(u32 *)(s+20) = c32;
    *(u32 *)(s+24) = c32;
    *(u32 *)(s+n-28) = c32;
    *(u32 *)(s+n-24) = c32;
    *(u32 *)(s+n-20) = c32;
    *(u32 *)(s+n-16) = c32;

    /* Align to a multiple of 8 so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as much as is
     * practical without introducing additional branching. */

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been
     * filled, so any remainder when n drops below 32 can be
     * safely ignored. */

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n-=32, s+=32) {
        *(u64 *)(s+0) = c64;
        *(u64 *)(s+8) = c64;
        *(u64 *)(s+16) = c64;
        *(u64 *)(s+24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif

    return dest;
}
#endif /* SIMPLE_MEMSET -= 0 */

#if( SIMPLE_MEMCMP == 0 )
#undef memcmp
int memcmp(const void *str1, const void *str2, size_t count)
{
    const uint8_t *s1 = str1;
    const uint8_t *s2 = str2;

    while (count-- > 0)
    {
       if (*s1++ != *s2++)
          return s1[-1] < s2[-1] ? -1 : 1;
    }

    return BST_SUCCESS;
}
#endif /* SIMPLE_MEMCMP -= 0 */
/*-----------------------------------------------------------*/
