ccflags-y += -Iarch/arm/mach-msm

obj-$(CONFIG_MACH_LGE) += devices_lge.o
obj-$(CONFIG_MACH_MSM8226_E7WIFI) += board-8226-e7wifi.o board-8226-e7wifi-gpiomux.o

obj-$(CONFIG_LGE_PM_CHARGING_TEMP_SCENARIO) += lge_charging_scenario.o
obj-$(CONFIG_MACH_LGE) += bms-batterydata-LGE_BL_54SH_2540mAh_LG_Chem_Final.o
obj-$(CONFIG_LGE_PM_BATTERY_CAPACITY_4000mAh) += bms-batterydata_LGE_BL_T12_4000mAh_TOCAD_Final.o bms-batterydata_LGE_BL_T12_4000mAh_LG_Chem_Final.o

CFLAGS_devices_lge.o += -Idrivers/staging/android
