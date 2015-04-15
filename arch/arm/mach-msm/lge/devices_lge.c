/*
 * arch/arm/mach-msm/lge/device_lge.c
 *
 * Copyright (C) 2012,2013 LGE, Inc
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/persistent_ram.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <asm/setup.h>
#include <asm/system_info.h>
#include <mach/board_lge.h>
#include <ram_console.h>

#ifdef CONFIG_ANDROID_PERSISTENT_RAM
#include <mach/msm_memtypes.h>
#endif

#ifdef CONFIG_ANDROID_RAM_CONSOLE
#define LGE_RAM_CONSOLE_SIZE (128 * SZ_1K * 2)
static char bootreason[128] = {0,};

int __init lge_boot_reason(char *s)
{
	int n;

	if (*s == '=')
		s++;
	n = snprintf(bootreason, sizeof(bootreason),
			"Boot info:\n"
			"Last boot reason: %s\n", s);
	bootreason[n] = '\0';
	return 1;
}
__setup("bootreason", lge_boot_reason);

struct ram_console_platform_data ram_console_pdata = {
	.bootinfo = bootreason,
};

static struct platform_device ram_console_device = {
	.name = "ram_console",
	.id = -1,
	.dev = {
		.platform_data = &ram_console_pdata,
	}
};
#endif

#ifdef CONFIG_PERSISTENT_TRACER
static struct platform_device persistent_trace_device = {
	.name = "persistent_trace",
	.id = -1,
};
#endif

#ifdef CONFIG_ANDROID_PERSISTENT_RAM
static struct persistent_ram_descriptor lge_pram_descs[] = {
#ifdef CONFIG_ANDROID_RAM_CONSOLE
	{
		.name = "ram_console",
		.size = LGE_RAM_CONSOLE_SIZE,
	},
#endif

#ifdef CONFIG_PERSISTENT_TRACER
	{
		.name = "persistent_trace",
		.size = LGE_RAM_CONSOLE_SIZE,
	},
#endif
};

static struct persistent_ram lge_persist_ram = {
	.size = LGE_PERSISTENT_RAM_SIZE,
	.num_descs = ARRAY_SIZE(lge_pram_descs),
	.descs = lge_pram_descs,
};

void __init lge_add_persist_ram_devices(void)
{
	int ret;
	struct memtype_reserve *mt = &reserve_info->memtype_reserve_table[MEMTYPE_EBI1];

	lge_persist_ram.start = mt->start - LGE_PERSISTENT_RAM_SIZE;
	pr_info("PERSIST RAM CONSOLE START ADDR : 0x%x\n", lge_persist_ram.start);

	ret = persistent_ram_early_init(&lge_persist_ram);
	if (ret)
		pr_err("%s: failed to initialize persistent ram\n", __func__);
}
#endif /* CONFIG_ANDROID_PERSISTENT_RAM */

void __init lge_reserve(void)
{
#if defined(CONFIG_ANDROID_PERSISTENT_RAM)
	lge_add_persist_ram_devices();
#endif
}

void __init lge_add_persistent_device(void)
{
#ifdef CONFIG_ANDROID_RAM_CONSOLE
	platform_device_register(&ram_console_device);
#ifdef CONFIG_LGE_HANDLE_PANIC
	/* write ram console addr to imem */
	lge_set_ram_console_addr(lge_persist_ram.start,
			LGE_RAM_CONSOLE_SIZE);
#endif
#endif
#ifdef CONFIG_PERSISTENT_TRACER
	platform_device_register(&persistent_trace_device);
#endif

}

/* get boot mode information from cmdline.
 * If any boot mode is not specified,
 * boot mode is normal type.
 */
static enum lge_boot_mode_type lge_boot_mode = LGE_BOOT_MODE_NORMAL;
int __init lge_boot_mode_init(char *s)
{
	if (!strcmp(s, "charger"))
		lge_boot_mode = LGE_BOOT_MODE_CHARGER;
#ifdef CONFIG_LGE_PM_CHARGING_CHARGERLOGO
	else if(!strcmp(s, "chargerlogo"))
		lge_boot_mode = LGE_BOOT_MODE_CHARGERLOGO;
#endif
	else if (!strcmp(s, "qem_56k"))
		lge_boot_mode = LGE_BOOT_MODE_QEM_56K;
	else if (!strcmp(s, "qem_130k"))
		lge_boot_mode = LGE_BOOT_MODE_QEM_130K;
	else if (!strcmp(s, "qem_910k"))
		lge_boot_mode = LGE_BOOT_MODE_QEM_910K;
	else if (!strcmp(s, "pif_56k"))
		lge_boot_mode = LGE_BOOT_MODE_PIF_56K;
	else if (!strcmp(s, "pif_130k"))
		lge_boot_mode = LGE_BOOT_MODE_PIF_130K;
	else if (!strcmp(s, "pif_910k"))
		lge_boot_mode = LGE_BOOT_MODE_PIF_910K;
	else
		lge_boot_mode = LGE_BOOT_MODE_NORMAL;

#ifdef CONFIG_LGE_PM_CHARGING_CHARGERLOGO
        lge_boot_mode_for_touch = (int)lge_boot_mode;
#endif

	return 1;
}
__setup("androidboot.mode=", lge_boot_mode_init);

enum lge_boot_mode_type lge_get_boot_mode(void)
{
	return lge_boot_mode;
}

/* for board revision */
static hw_rev_type lge_bd_rev = HW_REV_A;

/* CAUTION: These strings are come from LK. */
char *rev_str[] = {"rev_0", "rev_a", "rev_b", "rev_c", "rev_d",
	"rev_e", "rev_10", "rev_11", "revserved"};

static int __init board_revno_setup(char *rev_info)
{
	int i;

	for (i = 0; i < HW_REV_MAX; i++) {
		if (!strncmp(rev_info, rev_str[i], 6)) {
			lge_bd_rev = (hw_rev_type) i;
			/* it is defined externally in <asm/system_info.h> */
			system_rev = lge_bd_rev;
			break;
		}
	}

	printk(KERN_INFO "BOARD : LGE %s \n", rev_str[lge_bd_rev]);
	return 1;
}
__setup("lge.rev=", board_revno_setup);

hw_rev_type lge_get_board_revno(void)
{
    return lge_bd_rev;
}
