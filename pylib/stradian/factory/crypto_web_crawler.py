#!/usr/bin/env python3

""" crypto_web_crawler.py
CryptoWebCrawler(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")

import wget
import zipfile
from bs4 import BeautifulSoup
import pandas as pd
import datetime
import ccxt

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.ie.options import Options
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

from stradian.web_crawler import WebCrawler
from stradian.mariadb import read_json, attributes_format
from stradian.logger import Logger

    
class CryptoWebCrawler(WebCrawler):
    def __init__(self):
        super().__init__(["crypto_market_1d", "crypto_market_1h"])
        self.intervals = ["1d", "1h"]
        self.MARKETCODE = "CRYPTO"
        self.items_table = "crypto_market"
        self.json_files = ["etc/json/crypto_market_1d/crypto_format",
                           "etc/json/crypto_market_1h/crypto_format"]

        self.wget_path = "var/tmp/"
        self.stack = list()

    def run(self):
        super().run(self.crwaling)
        
    def crawling(self, symbol):
        date_list = month_from(symbol)
        
        for idx, mariadb in enumerate(self.mariadbs):
            self.wget_from_binance(symbol, intervals[idx], date_list[idx])
        
            

    def month_from(self, symbol):
        table_format = read_json()
        built_attribute = attributes_format(table_format["attirbutes"],
                                            table_format["primary"])
        sql = f"CREATE TABLE IF NOT EXISTS '{coin}' ({sub});"
        for mariadb in self.mariadbs:
            mariadb.query(sql)

        latest_update = 0
        date_list = list()
        for idx, mariadb in enumerate(self.mariadbs):
            sql = "SELECT max(timestamp) FROM '{coin}';"
            result = mariadb.query_fetchone(sql)

            crt_date = datetime.date(2000, 1, 1)
            if result == None:
                date_list.append(crt_date)
            else:
                crt_date = datetime.date.fromtimestamp(result)
                
                match self.intervals[idx]:
                    case "1d":
                        crt_date += datetime.timedelta(days = 1)
                    case "1h":
                        crt_date += datetime.timedelta(hours = 1)

                date_list.append(crt_date)
                                 
        return date_list


    def wget_from_binance(self, symbol, interval, last_date):
        url = "https://date.binance.vision/?prefix=data/spot/daily/klines/%s/%s/" %(symbol, interval)
        wget_url = "https://data.binance.vision/data/spot/daily/klines/%s/%s/" %(symbol, interval)

        driver = self.chrome_webdriver()
        driver.implicitly_wait(10)

        driver.get(url)
        xpath = "//*[@id='listing']/tr[2]/td[1]/a"

        try:
            element = WebDriverWait(driver, 5).util(
                EC.presence_of_element_located((By.XPATH, xpath))
            )
        except:
            logger = Logger("CryptoWebCrawler: table listing not loaded")
            logger.log("ERROR")

        html = driver.page_source
        soup = BeautifulSoup(html, "lxml")
        html_table = soup.select("table")

        table = pd.read_html(str(html_table))

        file_list = list(table[0].iloc[1:, 0])

        for download in file_list:
            if download[-8: ] != "CHECKSUM":
                year = int(download[-14: -10])
                month = int(download[-9: -7])
                day = int(download[-6: -4])
                file_time = datetime.date(year, month, day)

                if last_date < file_time:
                    wget.download(wget_url + downlaod,
                                  out = self.wget_path + download)
                    self.stack.append(download)

                
    def wget_remove_file(self):
        while (len(self.stack)):
            file_name = self.stack.pop()
            check = self.wget_path + file_name
            if os.path.isfile(check):
                os.remove(check)
            else:
                os.rmdir(check)

    def unzip_files(self):
        unzip_list = list()
        for file_name in self.stack:
            with zipfile.ZipFile(self.wget_path + file_name, 'r') as zip_ref:
                zip_ref.extractall(self.wget_path + file + ".unzip")

            unziplist.append(file + ".unzip")

        self.wget_remove_file()
        self.stack += unzip_list

    def read_remove_csv(self, file_name):
        csv = self.wget_path + file_name + '/' + file_name[: -9] + "csv"
        df = pd.read_csv(csv, header = None)
        df = df.iloc[:, :6]
        os.remove(csv)
        
        return df

    def upload_dbms(self, mariadb, symbol):
        self.stack = sorted(self.stack, key = lambda fs: fs[11: 18])
        for file_name in self.stack:
            df = self.read_remove_csv(file_name)
            mariadb.update_df(symbol, df)
            mariadb.commit()
            
