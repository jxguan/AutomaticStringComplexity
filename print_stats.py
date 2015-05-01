#!/usr/bin/env python
import os
from sys import stdout
MAX_LENGTH = 8
OUTPUT_FOLDER_PATTERN = "output/length_"
OUTPUT_FILE_PATTERN = "size"
OUTPUT_FILE_EXTENSION = ".out"
for i in range(2, MAX_LENGTH + 1):
  print "Strings of Length " + str(i) + ": " 
  k = 2
  path = OUTPUT_FOLDER_PATTERN + str(i) + "/"
  filename = path + OUTPUT_FILE_PATTERN + str(k) + OUTPUT_FILE_EXTENSION
  while os.path.isfile(filename):
    print "\tSize " + str(k) + ": ",
    stdout.flush()
    os.system("grep -c \"Unique String\" " + filename)
    k += 1
    filename = path + OUTPUT_FILE_PATTERN + str(k) + OUTPUT_FILE_EXTENSION
  print
#os.system("ls " + outputdi)
