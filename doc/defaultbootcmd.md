

# Default T20 boot command breakdown

Ingenic's modified source of uboot-v2013.07 includes a default boot command for booting the kernel.

This default command is in uboot-isvp_t20.h.patch in the `patches` directory.
It patches the file `uboot/include/configs/isvp_t20.h`

Here is the actual define:

```
	#define CONFIG_BOOTCOMMAND " \
	if printenv soctype; then \
	  echo \"Settings are already stored!\"; \
	else \
	  jzsoc; saveenv; \
	fi; \
	if test \"${soctype}\" = \"T20L\" && test \"${flashtype}\" = \"EN25QH128A\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; sleep 3; gpio clear 43; if printenv hwversion; then true; else setenv hwversion SENISC5; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20X\" && test \"${flashtype}\" = \"GD25Q128\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; if printenv hwversion; then true; else setenv hwversion DF3; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20L\" && test \"${flashtype}\" = \"W25Q128\" ; then \
	  gpio clear 75; gpio clear 76; gpio set 60; if printenv hwversion; then true; else setenv hwversion SXJ02ZM; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20X\" && test \"${flashtype}\" = \"EN25QH128A\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; sleep 3; gpio clear 43; if printenv hwversion; then true; else setenv hwversion WYZEC2; saveenv; fi; \
	  \
	else \
	  if printenv count; then \
	    if test \"${count}\" = \"1\"; then \
	      echo \"ping: ${count}\"; setenv count 2; saveenv; \
	      gpio set 43; gpio clear 48; sleep 3; gpio clear 43; \
	    else \
	      echo \"pong: ${count}\"; setenv count 1; saveenv; gpio set 43; gpio clear 48; \
	    fi; \
	  else \
	    echo \"pong: ${count}\"; setenv count 1; saveenv; \
	    gpio set 43; gpio clear 48; \
	  fi; \
	fi; fi; fi; fi; \
	echo Hardware found: ${hwversion}; \
	\
	\
	if mmc rescan; then \
	  echo \"MMC Found\"; \
	  if ext4load mmc 0:1 0x80600000 uEnv.txt; then \
	    env import -t 0x80600000 ${filesize}; setenv bootargs \"${bootargs} hwversion=${hwversion}\"; boot; \
	  fi; \
	  else \
	    echo \"MMC not found...\"; \
	fi; \
	gpio clear 38; \
	sf probe; \
	sf read 0x80600000 0x40000 0x280000; \
	bootm 0x80600000;"
```

---

```
	if printenv soctype; then \
	  echo \"Settings are already stored!\"; \
	else \
	  jzsoc; saveenv; \
	fi; \
```

This section checks if the environment variable `soctype` is set. If it isn't set then it runs a custom
command called `jzsoc`. This command is mostly used to determine what system we are working with and then
saves it to the environment.

```
isvp_t20# jzsoc
SOC: T20X
```
---

```
	if test \"${soctype}\" = \"T20L\" && test \"${flashtype}\" = \"EN25QH128A\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; sleep 3; gpio clear 43; if printenv hwversion; then true; else setenv hwversion SENISC5; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20X\" && test \"${flashtype}\" = \"GD25Q128\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; if printenv hwversion; then true; else setenv hwversion DF3; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20L\" && test \"${flashtype}\" = \"W25Q128\" ; then \
	  gpio clear 75; gpio clear 76; gpio set 60; if printenv hwversion; then true; else setenv hwversion SXJ02ZM; saveenv; fi; \
	  \
	else if test \"${soctype}\" = \"T20X\" && test \"${flashtype}\" = \"EN25QH128A\" && i2c probe 0x40 ; then \
	  gpio set 38; gpio set 39; gpio set 47; gpio set 43; gpio clear 48; sleep 3; gpio clear 43; if printenv hwversion; then true; else setenv hwversion WYZEC2; saveenv; fi; \
	  \
```

From here we can see a few `soctype`'s that we test for and the associated flash memory type that is used.


T20L | EN25QH128A or W25Q128
T20X | GD25Q128 or EN25QH128A

Testing this on a Wyze Cam 2 (which shoould be WYZEC2) I get `T20X` and `EN25QH128A`.

When I execute `i2c probe 0x40` I don't see any valid chip addresses.

```
isvp_t20# i2c probe 0x40
Valid chip addresses:
```

However `i2c probe 0x40` returns 1 which makes the boolean statement true.
