#ifndef BCM_MEMCPY_H
#define BCM_MEMCPY_H

#include "bcm_types.h"
#include "bstatus.h"

#ifndef SIMPLE_MEMCPY
	#define SIMPLE_MEMCPY	( 0 )
#endif

#ifndef SIMPLE_MEMSET
	#define SIMPLE_MEMSET	( 0 )
#endif

#ifndef SIMPLE_MEMCMP
	#define SIMPLE_MEMCMP	( 0 )
#endif

// declarations for bcm_mem.c
#if( SIMPLE_MEMSET != 0 )
void *memset( void *pvDest, int iValue, size_t ulBytes );
#endif /* SIMPLE_MEMSET != 0 */

#if( SIMPLE_MEMSET == 0 )
void *memset(void *dest, int c, size_t n);
#endif /* SIMPLE_MEMSET = 0 */


#if( SIMPLE_MEMCPY != 0 )
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes );
#endif

#if( SIMPLE_MEMCPY == 0 )
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes );
#endif

#if( SIMPLE_MEMCMP == 0 )
int memcmp(const void *str1, const void *str2, size_t count);
#endif

#endif