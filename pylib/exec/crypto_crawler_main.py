#!/usr/bin/env python3

""" crypto_web_crawler_main.py
CryptoWebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys,os
sys.path.append("pylib/")

from stradian.crypto_web_crawler import CryptoWebCrawler
from stradian.logger import Logger

if __name__ == "__main__":
    crawler_1d = CryptoWebCrawler("crypto_market_1d", "1d")
    symbols_1d = crawler_1d.get_symbols()
    for symbol in symbols_1d:
        crawler_1d.upload(symbol)

    crawler_1h = CryptoWebCrawler("crypto_market_1h", "1h")
    symbols_1h = crawler_1h.get_symbols()
    for symbol in symbols_1h:
        crawler_1h.upload(symbol)
