#!/usr/bin/env python
# -*- coding: utf_8 -*-

import logging
from logging.handlers import RotatingFileHandler
import os

# logPath = os.getcwd() + os.path.sep + "logs"
# if not os.path.exists(logPath):
#     os.makedirs(logPath)

# fh = RotatingFileHandler("logs/scons.log",
#                          maxBytes=10 * 1024 * 1024, backupCount=100)
# fh.setLevel(logging.DEBUG)
# log write in console
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
# log formatter
formatter = logging.Formatter(
    '[%(asctime)s][%(levelname)7s] [%(filename)15s%(funcName)15s%(lineno)06s] %(message)s')
# fh.setFormatter(formatter)
ch.setFormatter(formatter)

logger = logging.root
logger.setLevel(logging.INFO)
# logger.addHandler(fh)
logger.addHandler(ch)
