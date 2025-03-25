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
from stradian.mariadb import read_json, attributes_format
from stradian.logger import Logger

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.ie.options import Options
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

import wget
import zipfile
from bs4 import BeautifulSoup
import pandas as pd
import time
import datetime
from io import StringIO

class CurrencyWebCrawler:
    def __init__(self, db = "currency_1d"):
        self.maria = MariaDB(db)
        self.wget_path = "var/tmp/"
        self.db = db
    
    def day_from(self, symbol):
        table_format = read_json("etc/json/currency_1d/currency_format.json")
        sub = attributes_format(table_format["attributes"],
                                table_format["primary"])
        sql = f"CREATE TABLE IF NOT EXISTS {symbol} ({sub});"
        self.maria.query(sql);
        
        last = self.maria.query_fetchone(f"SELECT MAX(opentime) FROM {symbol};");
        
        crt_date = datetime.datetime(1970, 1, 1, 1, 0, 0)
        if last == None:
            return crt_date
        else:
            crt_date = last + datetime.timedelta(days = -1)

            return crt_date
    
    def read_df(self, symbol, day_from):
        url = "https://finance.yahoo.com/quote/%s%%3DX/history/?period1=%d&period2=%d" %(symbol, day_from.timestamp(), time.time())
        
        # driver_path = "/usr/bin/chromedriver"
        options = webdriver.ChromeOptions()
        options.add_argument("headless")
        options.add_argument("user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36")
        options.add_argument("--remote-debugging-pipe")
        # service = Service(executable_path = driver_path)
        service = Service(executable_path = ChromeDriverManager().install())
        
        driver = webdriver.Chrome(service = service, options = options)
        driver.implicitly_wait(5)
        
        driver.get(url)
        xpath = '//*[@id="nimbus-app"]/section/section/section/article/div[1]/div[3]/table/tbody'
        
        try:
            element = WebDriverWait(driver, 5).until(
                EC.presence_of_element_located((By.XPATH, xpath))
            )
        except:
            logger = Logger("CurrencyWebCrawler: Currency Web Crawling Failed")
            logger.log("ERROR")
            
        html = driver.page_source
        soup = BeautifulSoup(html, "lxml")
        html_table = soup.select("table")
        
        df = pd.read_html(StringIO(str(html_table)))[0]
        df = df.iloc[:, :6]
        
        df.iloc[:, 0] = df.iloc[:, 0].map(lambda tstr : datetime.datetime.strptime(tstr, "%b %d, %Y").strftime("%Y-%m-%d %H:%M:%S"))
        df.iloc[:, 1:] = 1 / df.iloc[:, 1:]
        
        return df
  
    def upload_mariaDB(self, symbol, df):
        self.maria.upload_df(symbol, df)
        
    def get_symbols(self):
        systemDB = MariaDB("system")
        sql = "SELECT symbol FROM currency_market WHERE trade = True;"
        results = systemDB.query_fetchall(sql)
        symbols = list()
        for result in results:
            symbols.append(result[0])

        return symbols
  
    def upload(self, symbol):
        Logger("CurrencyWebCrawler: " + symbol + " crawler started").log("INFO")
        day_from = self.day_from(symbol)
        df = self.read_df(symbol, day_from)
        self.upload_mariaDB(symbol, df)
        Logger("CurrencyWebCrawler: " + symbol + " crawler terminated").log("INFO")
