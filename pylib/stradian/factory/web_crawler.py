#!/usr/bin/env python3

""" web_crawler.py
WebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
from stradian.mariadb import MariaDB
from stradian.logger import Logger

class WebCrawler:
    def __init__(self, dbs):
        self.MARKETCODE = ""
        self.items_table = ""
        self.mariadbs = list()
        self.intervals = list()
        
        for db in dbs:
            self.mariadbs.append(MariaDB(db))

    def __del__(self):
        for mariadb in self.mariadbs:
            mariadb.__del__()

    def run(self, crawling_function):
        Logger(MARKETCODE + ": WebCrawler started").log("INFO")

        symbols = self.check_trading_items()

        for symbol in symbols:
            crawling_function(symbol)
        
        Logger(MARKETCODE + ": WebCrawler terminated").log("INFO")

    def check_trading_items(self):
        systemDB = MariaDB("system")
        sql = "SELECT symbol FROM %s WHERE trade = True" %(self.items_table)
        results = systemDB.query_fetchall(sql)
        symbols = list()
        
        for result  in results:
            symbols.append(result[0])

        return symbols

    def chrome_webdriver(self):
        self.driver_path = "/usr/bin/chromedriver"
        options = webdriver.ChromeOptions()
        options.add_argument("headless")
        options.add_argument("user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36")
        service = Service(executable_path = self.driver_path)
        driver = webdriver.Chrome(service = service, options = options)
        
        return driver

