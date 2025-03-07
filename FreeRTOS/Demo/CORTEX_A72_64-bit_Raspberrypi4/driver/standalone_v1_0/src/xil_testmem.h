/******************************************************************************
*
* Copyright (C) 2009 - 2015 Xilinx, Inc. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xil_testmem.h
* @addtogroup common_test_utils
*
* <h2>Memory test</h2>
*
* The xil_testmem.h file contains utility functions to test memory.
* A subset of the memory tests can be selected or all of the tests can be run
* in order. If there is an error detected by a subtest, the test stops and the
* failure code is returned. Further tests are not run even if all of the tests
* are selected.
* Following list describes the supported memory tests:
*
*  - XIL_TESTMEM_ALLMEMTESTS: This test runs all of the subtests.
*
*  - XIL_TESTMEM_INCREMENT: This test
* starts at 'XIL_TESTMEM_INIT_VALUE' and uses the incrementing value as the
* test value for memory.
*
*  - XIL_TESTMEM_WALKONES: Also known as the Walking ones test. This test
* uses a walking '1' as the test value for memory.
* @code
*          location 1 = 0x00000001
*          location 2 = 0x00000002
*          ...
* @endcode
*
*  - XIL_TESTMEM_WALKZEROS: Also known as the Walking zero's test.
* This test uses the inverse value of the walking ones test
* as the test value for memory.
* @code
*       location 1 = 0xFFFFFFFE
*       location 2 = 0xFFFFFFFD
*       ...
*@endcode
*
*  - XIL_TESTMEM_INVERSEADDR: Also known as the inverse address test.
* This test uses the inverse of the address of the location under test
* as the test value for memory.
*
*  - XIL_TESTMEM_FIXEDPATTERN: Also known as the fixed pattern test.
* This test uses the provided patters as the test value for memory.
* If zero is provided as the pattern the test uses '0xDEADBEEF".
*
* @warning
* The tests are <b>DESTRUCTIVE</b>. Run before any initialized memory spaces
* have been set up.
* The address provided to the memory tests is not checked for
* validity except for the NULL case. It is possible to provide a code-space
* pointer for this test to start with and ultimately destroy executable code
* causing random failures.
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundry of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver    Who    Date    Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a hbm  08/25/09 First release
* </pre>
*
******************************************************************************/

#ifndef XIL_TESTMEM_H	/* prevent circular inclusions */
#define XIL_TESTMEM_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "bcm_types.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

/* xutil_memtest defines */

#define XIL_TESTMEM_INIT_VALUE	1U

/** @name Memory subtests
 * @{
 */
/**
 * See the detailed description of the subtests in the file description.
 */
#define XIL_TESTMEM_ALLMEMTESTS     0x00U
#define XIL_TESTMEM_INCREMENT       0x01U
#define XIL_TESTMEM_WALKONES        0x02U
#define XIL_TESTMEM_WALKZEROS       0x03U
#define XIL_TESTMEM_INVERSEADDR     0x04U
#define XIL_TESTMEM_FIXEDPATTERN    0x05U
#define XIL_TESTMEM_MAXTEST         XIL_TESTMEM_FIXEDPATTERN
/* @} */

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/* xutil_testmem prototypes */

extern s32 Xil_TestMem32(u32 *Addr, u32 Words, u32 Pattern, u8 Subtest);
extern s32 Xil_TestMem16(u16 *Addr, u32 Words, u16 Pattern, u8 Subtest);
extern s32 Xil_TestMem8(u8 *Addr, u32 Words, u8 Pattern, u8 Subtest);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_test_utils".
*/
