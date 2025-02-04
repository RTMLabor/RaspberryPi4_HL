/******************************************************************************
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_assert.h
*
* @addtogroup common_assert_apis Assert APIs and Macros
*
* The bcm_assert.h file contains assert related functions and macros.
* Assert APIs/Macros specifies that a application program satisfies certain
* conditions at particular points in its execution. These function can be
* used by application programs to ensure that, application code is satisfying
* certain conditions.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a hbm  07/14/09 First release
* 6.0   kvn  05/31/16 Make Xil_AsserWait a global variable
* </pre>
*
******************************************************************************/

#ifndef BCM_ASSERT_H	/* prevent circular inclusions */
#define BCM_ASSERT_H	/* by using protection macros */

#include "bcm_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************** Include Files *********************************/


/************************** Constant Definitions *****************************/

#define BCM_ASSERT_NONE     0U
#define BCM_ASSERT_OCCURRED 1U
#define XNULL NULL

extern u32 Bcm_AssertStatus;
extern s32 Bcm_AssertWait;
extern void Bcm_Assert(const char8 *File, s32 Line);
void BNullHandler(void *NullParameter);

/**
 * This data type defines a callback to be invoked when an
 * assert occurs. The callback is invoked only when asserts are enabled
 */
typedef void (*Bcm_AssertCallback) (const char8 *File, s32 Line);

/***************** Macros (Inline Functions) Definitions *********************/

#ifndef NDEBUG

/*****************************************************************************/
/**
* @brief    This assert macro is to be used for void functions. This in
*           conjunction with the Bcm_AssertWait boolean can be used to
*           accomodate tests so that asserts which fail allow execution to
*           continue.
*
* @param    Expression: expression to be evaluated. If it evaluates to
*           false, the assert occurs.
*
* @return   Returns void unless the Bcm_AssertWait variable is true, in which
*           case no return is made and an infinite loop is entered.
*
******************************************************************************/
#define Bcm_AssertVoid(Expression)                \
{                                                  \
    if (Expression) {                              \
        Bcm_AssertStatus = BCM_ASSERT_NONE;       \
    } else {                                       \
        Bcm_Assert(__FILE__, __LINE__);            \
        Bcm_AssertStatus = BCM_ASSERT_OCCURRED;   \
        return;                                    \
    }                                              \
}

/*****************************************************************************/
/**
* @brief    This assert macro is to be used for functions that do return a
*           value. This in conjunction with the Bcm_AssertWait boolean can be
*           used to accomodate tests so that asserts which fail allow execution
*           to continue.
*
* @param    Expression: expression to be evaluated. If it evaluates to false,
*           the assert occurs.
*
* @return   Returns 0 unless the Bcm_AssertWait variable is true, in which
* 	        case no return is made and an infinite loop is entered.
*
******************************************************************************/
#define Bcm_AssertNonvoid(Expression)             \
{                                                  \
    if (Expression) {                              \
        Bcm_AssertStatus = BCM_ASSERT_NONE;       \
    } else {                                       \
        Bcm_Assert(__FILE__, __LINE__);            \
        Bcm_AssertStatus = BCM_ASSERT_OCCURRED;   \
        return 0;                                  \
    }                                              \
}

/*****************************************************************************/
/**
* @brief     Always assert. This assert macro is to be used for void functions.
*            Use for instances where an assert should always occur.
*
* @return    Returns void unless the Bcm_AssertWait variable is true, in which
*	         case no return is made and an infinite loop is entered.
*
******************************************************************************/
#define Bcm_AssertVoidAlways()                   \
{                                                  \
   Bcm_Assert(__FILE__, __LINE__);                 \
   Bcm_AssertStatus = BCM_ASSERT_OCCURRED;        \
   return;                                         \
}

/*****************************************************************************/
/**
* @brief   Always assert. This assert macro is to be used for functions that
*          do return a value. Use for instances where an assert should always
*          occur.
*
* @return Returns void unless the Bcm_AssertWait variable is true, in which
*	      case no return is made and an infinite loop is entered.
*
******************************************************************************/
#define Bcm_AssertNonvoidAlways()                \
{                                                  \
   Bcm_Assert(__FILE__, __LINE__);                 \
   Bcm_AssertStatus = BCM_ASSERT_OCCURRED;        \
   return 0;                                       \
}


#else

#define Bcm_AssertVoid(Expression)
#define Bcm_AssertVoidAlways()
#define Bcm_AssertNonvoid(Expression)
#define Bcm_AssertNonvoidAlways()

#endif

/************************** Function Prototypes ******************************/

void Bcm_AssertSetCallback(Bcm_AssertCallback Routine);

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */
/**
* @} End of "addtogroup common_assert_apis".
*/