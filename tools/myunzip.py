#!/usr/bin/env python
import os
import sys
import zipfile
print "Processing sys.argv[1]"
file = zipfile.ZipFile(sys.argv[1],"r")
for name in file.namelist():
    utf8name=name.decode('gbk')
    print "Extracting " + utf8name
    pathname = os.path.dirname(utf8name)

    if not os.path.exists(pathname) and pathname != "":
        os.makedirs(pathname)
    data = file.read(name)
    if not os.path.exists(utf8name):
        fo = open(utf8name,"w")
        fo.write(data)
        fo.close()
