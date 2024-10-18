#!/usr/bin/env python3

""" currency_web_crawler_main.py
CurrencyWebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys,os
sys.path.append("pylib/")

from stradian.currency_web_crawler import CurrencyWebCrawler
from stradian.logger import Logger

if __name__ == "__main__":
    currency_1d = CurrencyWebCrawler("currency_1d")
    symbols_1d = currency_1d.get_symbols()
    for symbol in symbols_1d:
        currency_1d.upload(symbol)

