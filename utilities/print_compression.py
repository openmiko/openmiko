#!/usr/bin/python3

import lzma
import os
import sys
import csv

from os import listdir
from os.path import isfile, join
from os import walk

mypath = sys.argv[1]



outputcsv = open("compression_ratios.csv", "w")
writer = csv.writer(outputcsv, delimiter=',')

writer.writerow(["Path", "Uncompressed Bytes", "Compressed Bytes"])

for (dirpath, dirnames, filenames) in walk(mypath):
    for f in filenames:
         fullpath = os.path.join(dirpath, f)
         if(os.path.islink(fullpath)):
             continue

         bytes = open(fullpath, "rb").read()

         lzma_compressor = lzma.LZMACompressor(preset=9)
         compressed_bytes = lzma_compressor.compress(bytes)
         compressed_bytes = compressed_bytes + lzma_compressor.flush()

         row = [fullpath, len(bytes), len(compressed_bytes)]
         print(row)
         writer.writerow(row)
