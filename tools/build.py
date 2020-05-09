#!/usr/bin/env python
# -*- coding: utf_8 -*-

import sys
import re, os
import subprocess
import shlex

import logging
from logging.handlers import RotatingFileHandler
import os


logPath = os.getcwd() + os.path.sep + "logs"
if not os.path.exists(logPath):
    os.makedirs(logPath)

fh = RotatingFileHandler("build.log", maxBytes=10 * 1024 * 1024, backupCount=100)
fh.setLevel(logging.DEBUG)
#log write in console
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
#log formatter
formatter = logging.Formatter(
    '%(asctime)s %(levelname)8s [%(filename)25s%(funcName)20s%(lineno)06s] %(message)s')
fh.setFormatter(formatter)
ch.setFormatter(formatter)

logger = logging.root
logger.setLevel(logging.INFO)
logger.addHandler(fh)
logger.addHandler(ch)


def Process(*args, isRealTimeResult=False):

    cmd = args
    cmd_s = " ".join(shlex.quote(word) for word in cmd)  # For errors

    try:
        cmd_process = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except FileNotFoundError:
        logger.error("cmd executable not found (when running '{}'). Check that "
                 "it's in listed in the PATH environment variable"
                 .format(cmd_s))
    except OSError as e:
        logger.error("failed to run '{}': {}".format(cmd_s, e))
        return

    if not isRealTimeResult:

        stdout, stderr = cmd_process.communicate()
        if cmd_process.returncode or stderr:
            logger.error("failed to run '{}': {}".format(
                cmd_s, stdout.decode("utf-8") + stderr.decode("utf-8")))

        return stdout.decode("utf-8").rstrip()
    else:
        while cmd_process.poll() is None:
            line = cmd_process.stdout.readline()
            line = line.strip()
            print(line)


def main():
    os.environ["ZEPHYR_BASE"] = os.environ["ZEPHYR_BASE"]
    os.environ["ZEPHYR_TOOLCHAIN_VARIANT"] = os.environ["ZEPHYR_TOOLCHAIN_VARIANT"]
    os.environ["GNUARMEMB_TOOLCHAIN_PATH"] = os.environ["GNUARMEMB_TOOLCHAIN_PATH"]
    boards = Process('west', 'boards', '-f', '{arch}-{name}')
    armboards = [b[4:] for b in boards.split("\r\n") if b.startswith("arm")]
    os.chdir(os.path.abspath("../"))
    failedBoards = []
    successBoards = []
    for board in armboards:
        logger.info("west build -b %s ejs" % board)
        buildlog = Process('west', 'build', '-b', board, 'ejs')
        if "error:" in buildlog:
            logger.info("build %s failed!" % board)
            failedBoards.append(board)
        if "Linking C executable zephyr\zephyr.elf" in buildlog:
            logger.info("build %s successfully!" % board)
            successBoards.append(board)
        logger.info(failedBoards)
        logger.info(successBoards)
        with open("build/%s/ejs/build.log" % board, "wb") as f:
            f.write(bytes(buildlog, encoding = "utf8"))

    logger.info(failedBoards)
    logger.info(successBoards)


if __name__ == '__main__':
    main()
