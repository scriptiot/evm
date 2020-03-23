import os
import sys
import subprocess

import logging
from logging.handlers import RotatingFileHandler

#log write in console
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
#log formatter
formatter = logging.Formatter(
    '%(asctime)s %(levelname)8s [%(filename)25s%(funcName)20s%(lineno)06s] %(message)s')
ch.setFormatter(formatter)

logger = logging.root
logger.setLevel(logging.INFO)
logger.addHandler(ch)


def getFiles(rootpath, suffix=".png"):
    _files = []
    for root, dirs, files in os.walk(os.path.abspath(rootpath)):
        if '.git' not in root:
            for f in files:
                fpath = os.path.abspath(os.sep.join([root, f]))
                if fpath.endswith(suffix):
                    _files.append(fpath)
    return _files

qmlcode = '''Rectangle {
    id: root
    x: 0
    y: 0
    width: 800
    height: 600

    style:Style{
        main_color: 'lightblue'
        grad_color: '%s'
    }

    ScrollArea{
        x: 0; y:0;
        width: 800
        height:600

        %s
    }
}
'''
imagescode = '''
        Image {
            source: '%s'
        }
'''

def generatorCode(name, path, color):
    files = getFiles(path, ".bin")
    codes = ""
    for f in files:
        codes += imagescode % (os.path.basename(f))
    code  = qmlcode % (color, codes)

    with open("icons_%s.qml" % name, "wb") as f:
        f.write(code)

def main():
    generatorCode("dark", "./images/bin/dark", "green")
    generatorCode("light", "./images/bin/light", "white")


if __name__ == '__main__':
    main()