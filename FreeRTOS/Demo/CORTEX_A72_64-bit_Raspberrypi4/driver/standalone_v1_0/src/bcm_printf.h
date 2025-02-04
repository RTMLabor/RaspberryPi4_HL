 #ifndef BCM_PRINTF_H
 #define BCM_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "bcm_types.h"
#include "bparameters.h"

/*----------------------------------------------------*/
/* Use the following parameter passing structure to   */
/* make bcm_printf re-entrant.                        */
/*----------------------------------------------------*/

struct params_s;


/*---------------------------------------------------*/
/* The purpose of this routine is to output data the */
/* same as the standard printf function without the  */
/* overhead most run-time libraries involve. Usually */
/* the printf brings in many kilobytes of code and   */
/* that is unacceptable in most embedded systems.    */
/*---------------------------------------------------*/

typedef char8* charptr;
typedef s32 (*func_ptr)(int c);

/*                                                   */

void bcm_printf( const char8 *ctrl1, ...);
void print( const char8 *ptr);
extern void outbyte (char8 c);
extern char8 inbyte(void);

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */
