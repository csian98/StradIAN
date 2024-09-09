#!/usr/bin/env python3

""" indices_web_crawler_main.py
IndicesWebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys,os
sys.path.append("pylib/")

from stradian.indices_web_crawler import IndicesWebCrawler
from stradian.logger import Logger

if __name__ == "__main__":
    indices_1d = IndicesWebCrawler("indices_1d")
    symbols_1d = indices_1d.get_symbols()
    for symbol in symbols_1d:
        indices_1d.upload(symbol)

