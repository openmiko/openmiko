#!/usr/bin/env python
# coding=utf-8
import click
import os

@click.command()
@click.argument('inputfile', default="dump.bin", type=click.Path(exists=True))

def cli(inputfile):
    dic = [
        ("boot", 0x40000),
        ("kernel", 0x280000),
        ("rootfs", 0x6F0000),
        ("data", 0x630000),
        ("config", 0x10000),
        ("factory", 0x10000),
    ]
    inputfile = click.format_filename(inputfile)

    newpath = r'flash/' 
    if not os.path.exists(newpath):
      os.makedirs(newpath)

    fullflash = open(inputfile, 'rb')
    #fullflash.seek(64)

    for name, size in dic:
        filename = "flash/" + name + ".bin"
        buffer = fullflash.read(size)
        f = open(filename, "wb")
        f.write(buffer)


if __name__ == '__main__':
    cli()
