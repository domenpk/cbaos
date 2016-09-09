#include <string.h>
#include <types.h>
#include <mach/lpc13xx_iap.h>
#include <arch/cm3_regs.h>


static void (*iap)(u32 *cmd, u32 *result) = (void*)0x1fff1ff1;
/*
typedef void (*IAP)(uint32_t [], uint32_t []);
IAP iap_entry = (IAP)0x1fff1ff1;
uint32_t command[5], result[4];

  iap_entry(command, result);
*/

#define IAP_CMD_PREPAREWRITE	50
#define IAP_CMD_COPYRAMTOFLASH	51
#define IAP_CMD_ERASE		52
#define IAP_CMD_BLANKCHECK	53
#define IAP_CMD_READPARTID	54
#define IAP_CMD_READBOOTCODE	55
#define IAP_CMD_COMPARE		56
#define IAP_CMD_INVOKEISP	57
#define IAP_CMD_READUUID	58


/* IAP/ISP reserved ram area is top 32B, but that's for flash operations only */
/*
 * I THINK there might be a problem with IAP+systick interrupt. I get freezes
 * after a few hours (watchdog restarts, but watchdog irq doesn't enter).
 *
 * 22:30:54.060: iap_readuuid: Hard Fault was called, dumping regs:
 * 22:30:54.060: new_lr    0xfffffff1
 * 22:30:54.060: r0        0x5     5
 * 22:30:54.060: r1        0x10000da0      268438944
 * 22:30:54.060: r2        0x4003c000      1073987584
 * 22:30:54.060: r3        0x4d114f5c      1292980060
 * 22:30:54.060: r4        0x40    64
 * 22:30:54.060: r5        0x0     0
 * 22:30:54.060: r6        0x6060606       101058054
 * 22:30:54.060: r7        0x7070707       117901063
 * 22:30:54.082: r8        0x8080808       134744072
 * 22:30:54.082: r9        0x9090909       151587081
 * 22:30:54.082: r10/sl    0xa0a0a0a       168430090
 * 22:30:54.082: r11/fp    0xb0b0b0b       185273099
 * 22:30:54.082: r12/ip    0x1fff0283      536806019
 * 22:30:54.082: sp        0x10001fe0 (newstack:0x10001fbc)
 * 22:30:54.082: lr        0xfffffffd
 * 22:30:54.082: pc        0xfffffffe
 * 22:30:54.082: xPSR      0x1000047
 * 22:30:54.082: HFSR      0x40000000
 */
int iap_readuuid(u8 *uuid)
{
	u32 cmd[1] = { IAP_CMD_READUUID };
	u32 result[5];
	iap(cmd, result);
	if (result[0] != 0)
		return -result[0];

	memcpy(uuid, &result[1], 4*4);
	return 0;
}

int iap_invokeisp(void)
{
	u32 cmd[1] = { IAP_CMD_INVOKEISP };

	NVIC->ICER[0] = 0xffffffff;
	NVIC->ICER[1] = 0xffffffff;

	iap(cmd, NULL);
	return -1; /* never returns */
}

// note for flash programming: MUST NOT use top 32B of RAM, also flash is not accessible -> disable interrupts
