#!/usr/bin/env python
# -*- coding: utf_8 -*-
import os
import sys
import copy

EXCLUDE_DIRS = [
    os.path.abspath(os.sep.join([os.getcwd(), "components"])),
    os.path.abspath(os.sep.join([os.getcwd(), "build"])),
    os.path.abspath(os.sep.join([os.getcwd(), "bsp", "stm32", "bearpi_stm32l431", "soc"])),
]

def getCHppFiles(rootpath):
    hfiles = []
    cppfiles = []
    cfiles = []
    for root, dirs, files in os.walk(rootpath) :
        if '.git' not in root:
            for f in files:
                fpath = os.path.abspath(os.sep.join([root, f]))

                isExclude = False
                for d in EXCLUDE_DIRS:
                    if fpath.startswith(d):
                        isExclude = True
                if isExclude:
                    # print(fpath)
                    continue
                if fpath.endswith(".h"):
                    hfiles.append(fpath)
                elif fpath.endswith(".cpp") or f.endswith(".c"):
                    cppfiles.append(fpath)
    return hfiles, cppfiles

def getHeaderInfo(headerpath):
    print(headerpath)
    if os.path.exists(headerpath):
        with open(headerpath, 'r') as f:
            content = f.read()
        return content
    else:
        return None


def updateHeader(fpath, headerInfo):
    with open(fpath, 'r+') as fd:
        hcontent = fd.read()
        index = hcontent.rfind('*' * 76 + "/")
        if index != -1:
            index += 77
            validContent = hcontent[index:].lstrip()
        else:
            validContent = hcontent
            print(validContent[:50])
        content = headerInfo + "\n" + validContent
        with open(fpath, 'w') as fd:
            fd.write(content)
        print("update %s success" % fpath)

def updateEvmHeader():
    headerInfo = getHeaderInfo(os.sep.join([os.getcwd(), "tools", "evm.header"]))
    hfiles, cfiles = getCHppFiles(os.getcwd())
    cfiles.extend(hfiles)
    for f in cfiles:
        updateHeader(f, headerInfo)




def main():
    updateEvmHeader()


if __name__ == '__main__':
    main()
