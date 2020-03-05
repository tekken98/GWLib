#!/usr/bin/env python
import struct
import sys
import os
alpha='0123456789ABCDEF'
def getHex(num):
    return alpha.find(num.upper())
def mysplit(s):
    l = []
    i = 0
    while(i < len(s)):
        if s[i] == '%':
            l.append(s[i:i+3])
            i += 3
        else:
            l.append(s[i])
            i += 1
    return l
def convert(filename):
    c = mysplit(filename)
    l=""
    for a in c:
        h = 0
        if a[0] == '%':
            n = getHex(a[1]) * 16
            n += getHex(a[2])
            try:
                data=struct.pack(">B",n)
                l += data
            except:
                l += ""
        else:
            l += a
    os.rename(filename,l)
convert(sys.argv[1])
    
