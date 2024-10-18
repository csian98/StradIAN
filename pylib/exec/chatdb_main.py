#!/usr/bin/env python3

""" chatdb_main.py
CryptoWebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
from stradian.chatdb import ChatDB
from stradian.logger import Logger

if __name__ == "__main__":
    chatdb = ChatDB()
    chatdb.run()
