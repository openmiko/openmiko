# Modules

Due to space limitations on the cameras we created a module system that allows you to choose the extra functionality that you would like to add to your devices.This approach also makes it easier for developers to contribute to the project without needing to create pull requests back to the main repositiory.

## Module compatibilty

The module system has no built-in approach to ensuring modules are compatible with updates to the core system. It is each developer's responsibility to test and document the compatibility of their modules.

## Module list

| Name | Description |
| --- | --- | --- |
| [ddns](https://github.com/martinbutt/openmiko-module-ddns) | Adds ddclient for dynamic DNS |
| [fsck.vfat](https://github.com/martinbutt/openmiko-module-fsck-vfat) | Adds fsck.vfat for checking FAT partitions on the SD card |
| [fsck.ext](https://github.com/martinbutt/openmiko-module-fsck-ext) | Adds e2fsck for checking ext2/3/4 partitions on the SD card |

## Installing modules

To install a module, add the `.tar.xz` to the modules folder of the SD card. It will be installed when the camera starts up.

## Uninstalling modules

To uninstall a module, SSH onto the device and `cd /etc/openmiko.d/modules`. Here you will find all uninstall scripts for the modules. `cd` into the module folder and run `uninstall.sh`.

## Creating modules

Modules a contained in `.tar.xz` files. Inside you will find a files structure:
```
overlay/ (optional)
pre-install.sh (optional)
post-install.sh (optional)
uninstall.sh
```

Inside the `overlay/` folder you can place files that need to be installed onto the file system into their respective locations, e.g.
```
overlay/usr/bin/fsck.vfat
```

If you want a script to run before or after the files have been copied, then you can add `pre-install.sh` or `post-install.sh`.

You must include an `uninstall.sh` to revert the changes.
