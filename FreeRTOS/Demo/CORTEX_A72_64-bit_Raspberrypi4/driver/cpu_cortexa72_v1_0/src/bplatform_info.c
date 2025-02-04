/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bplatform_info.c
*
* This file contains information about hardware for which the code is built
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 5.00  pkp  12/15/14 Initial release

*
******************************************************************************/

/***************************** Include Files *********************************/

#include "bcm_types.h"
#include "bcm_io.h"
#include "bplatform_info.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* @brief    This API is used to provide information about platform
*
* @param    None.
*
* @return   The information about platform defined in bplatform_info.h
*
******************************************************************************/
u32 BGetPlatform_Info()
{

	return RPI_VERSION_4;

}