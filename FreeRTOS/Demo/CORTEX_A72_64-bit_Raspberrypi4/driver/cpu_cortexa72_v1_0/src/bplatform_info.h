/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bplatform_info.h
*
* @addtogroup common_platform_info APIs to Get Platform Information
*
* The bplatform_info.h file contains definitions for various available Xilinx&reg;
* platforms. Also, it contains prototype of APIs, which can be used to get the
* platform information.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date    Changes
* ----- ---- --------- -------------------------------------------------------
* 6.4    ms   05/23/17 Added PSU_PMU macro to support XGetPSVersion_Info
*                      function for PMUFW.
* </pre>
*
******************************************************************************/

#ifndef BPLATFORM_INFO_H		/* prevent circular inclusions */
#define BPLATFORM_INFO_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "bcm_types.h"

/************************** Constant Definitions *****************************/

#define RPI_VERSION_4 0x4

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


u32 BGetPlatform_Info();

/************************** Function Prototypes ******************************/


#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_platform_info".
*/
