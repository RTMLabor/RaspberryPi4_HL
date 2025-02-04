#ifndef BPARAMETERS_H   /* prevent circular inclusions */
#define BPARAMETERS_H   /* by using protection macros */

/* Definition for CPU ID */
#define PAR_CPU_ID 1U

/******************************************************************/

#define CORE_CLOCK_SPEED 1500000000

/* Canonical definitions for peripheral CORTEXA72 */
#define CPU_CORTEXA72_0_CPU_CLK_FREQ_HZ CORE_CLOCK_SPEED
#define CPU_CORTEXA72_1_CPU_CLK_FREQ_HZ CORE_CLOCK_SPEED
#define CPU_CORTEXA72_2_CPU_CLK_FREQ_HZ CORE_CLOCK_SPEED
#define CPU_CORTEXA72_3_CPU_CLK_FREQ_HZ CORE_CLOCK_SPEED

#define CPU_CORTEXA72_CORE_CLOCK_FREQ_HZ	CPU_CORTEXA72_0_CPU_CLK_FREQ_HZ


/******************************************************************/

#include "cpu_cortexa72.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#ifndef __cplusplus
#ifndef bool
typedef enum { false = 0, true = 1 } bool;
#endif
#endif



//Peripheral Base Address
#define BCM_RPI_PERIPHERAL_BASEADDR 0xFE000000
/*
#define STDIN_BASEADDRESS 0xE0000000
#define STDOUT_BASEADDRESS 0xE0000000

/******************************************************************/

/* Definitions for peripheral PS7_DDR_0 */
//#define XPAR_PS7_DDR_0_S_AXI_BASEADDR 0x00100000
//#define XPAR_PS7_DDR_0_S_AXI_HIGHADDR 0x3FFFFFFF


/******************************************************************/

/* Definitions for driver EMACPS */
//#define XPAR_XEMACPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_ETHERNET_0 */
/*
#define XPAR_PS7_ETHERNET_0_DEVICE_ID 0
#define XPAR_PS7_ETHERNET_0_BASEADDR 0xE000B000
#define XPAR_PS7_ETHERNET_0_HIGHADDR 0xE000BFFF
#define XPAR_PS7_ETHERNET_0_ENET_CLK_FREQ_HZ 125000000
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV1 1
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV1 5
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV0 8
#define XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV1 50
*/

/******************************************************************/

/* Definitions for driver GIC-400*/
#define BCM_RPI_GIC_DEVICE_ID 0U
#define BCM_RPI_GIC_BASEADDR 0xFF840000UL   //only if low peripheral mode is activated (otherwise: 0x4c004000)
#define BCM_RPI_GICD_BASEADDR (BCM_RPI_GIC_BASEADDR + 0x00001000UL)
#define BCM_RPI_GICC_BASEADDR (BCM_RPI_GIC_BASEADDR + 0x00002000UL)
//#define BCM_RPI_GIC_HIGHADDR 0xF8F001FFU

#define BPAR_GIC_SINGLE_DEVICE_ID 0U
#define BPAR_GIC_NUM_INSTANCES 1U

/* The number of IRQs on BCM2711 */
#define BGIC_MAX_NUM_INTR_INPUTS (224U)
#define BGIC_MAX_INTR_PRIO_VAL (0xFFU)

#define BGIC_INTR_PRIO_MASK  (0xF8U)
#define BGIC_PRIORITY_MASK (0xFFU)
#define BGIC_EN_INTR_MASK (0x00000001U)

#define BGIC_INTR_CFG_MASK (0x3U)

/* IRQ number */
#define IRQ_VTIMER (27)
#define IRQ_VC_UART (153)

/******************************************************************/

/* Definitions for driver GPIO */

/* Definitions for peripheral GPIO_0 */

#define BPAR_GPIO_0_DEVICE_ID 0

/******************************************************************/

/* Canonical definitions for peripheral PS7_GPIO_0 */

/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUGIC_0 */

/******************************************************************/

/* Definitions for driver SDPS */

/* Definitions for peripheral PS7_SD_0 */

/******************************************************************/

/* Definitions for driver Generic Timer*/

#define BCM_RPI_GT_BASEADDR (BCM_RPI_PERIPHERAL_BASEADDR + 0x00003000)

/******************************************************************/

/* Definitions for driver UARTPS */

/* Definitions for peripheral PS7_UART_0 */

/******************************************************************/

/* Canonical definitions for peripheral PS7_UART_0 */

/******************************************************************/

#endif  /* end of protection macro */
