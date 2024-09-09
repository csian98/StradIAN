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
import datetime
from io import StringIO

class CryptoWebCrawler:
    def __init__(self, db, interval="1d"):
        self.maria = MariaDB(db)
        self.interval = interval
        self.wget_path = "var/tmp/"
        self.stack = list()
        self.db = db
    
    def month_from(self, symbol):
        table_format = read_json("etc/json/crypto_market_%s/crypto_format.json" %(self.interval))
        sub = attributes_format(table_format["attributes"],
                                table_format["primary"])
        sql = f"CREATE TABLE IF NOT EXISTS {symbol} ({sub});"
        self.maria.query(sql);
        
        last = self.maria.query_fetchone(f"SELECT MAX(opentime) FROM {symbol};");
        
        crt_date = datetime.datetime(2000, 1, 1, 1, 0, 0)
        if last == None:
            return crt_date
        else:
            if self.interval=="1d":
                crt_date = last + datetime.timedelta(days = 1)
            elif self.interval=="1h":
                crt_date = last + datetime.timedelta(hours = 1)

            return crt_date
    
    def wget_from_binance(self, symbol, month_from):
        url = "https://data.binance.vision/?prefix=data/spot/daily/klines/%s/%s/" %(symbol, self.interval)
        wget_url = "https://data.binance.vision/data/spot/daily/klines/%s/%s/" %(symbol, self.interval)
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
        xpath = '//*[@id="listing"]/tr[2]/td[1]/a'
        
        try:
            element = WebDriverWait(driver, 5).until(
                EC.presence_of_element_located((By.XPATH, xpath))
            )
        except:
            logger = Logger("CryptoWebCrawler: Binance Web Crawling Failed")
            logger.log("ERROR")
            
        html = driver.page_source
        soup = BeautifulSoup(html, "lxml")
        html_table = soup.select("table")
        
        table = pd.read_html(StringIO(str(html_table)))
        
        file_list = list(table[0].iloc[1:, 0])
        
        for download in file_list:
            if download[-8:] != "CHECKSUM":
                year = int(download[-14: -10])
                month = int(download[-9: -7])
                day = int(download[-6: -4])

                file_time = datetime.datetime(year, month, day, 1, 0, 0)
            
                if month_from < file_time:
                    wget.download(wget_url + download,
                                  out = self.wget_path + download)
                    self.stack.append(download)
  
    def wget_remove_files(self):
        while(len(self.stack)):
            file_name = self.stack.pop()
            check = self.wget_path + file_name
            
            if os.path.isfile(check):
                os.remove(check)
            else:
                os.rmdir(check)
  

    def unzip_all_files(self):
        unzip_list = list()
        for file in self.stack:
            with zipfile.ZipFile(self.wget_path + file, 'r') as zip_ref:
                zip_ref.extractall(self.wget_path + file+".unzip")
      
            unzip_list.append(file + ".unzip")
    
        self.wget_remove_files()
        self.stack += unzip_list

    def series_typecast(self, utf_nano_series):
        dts = list(map(datetime.datetime.fromtimestamp, utf_nano_series / 1e3))
        format_list = list()
        for dt in dts:
            format_list.append(dt.strftime("%Y-%m-%d %H:%M:%S"))
        
        return pd.Series(format_list)
  
    def read_remove_csv(self, file):
        csv = self.wget_path + file + "/" + file[:-9] + "csv"
        df = pd.read_csv(csv, header = None)
        df = df.iloc[:, :9]

        opentime = self.series_typecast(df[0])
        closetime = self.series_typecast(df[6])
        
        df = df.drop([0, 6], axis = 1)
        df.insert(0, 0, opentime)
        df.insert(6, 6, closetime)
        os.remove(csv)
    
        return df
  
    def upload_mariaDB(self, symbol):
        self.stack = sorted(self.stack, key = lambda file : file[11: 18])
        for file in self.stack:
            df = self.read_remove_csv(file)
            self.maria.upload_df(symbol, df)

    def get_symbols(self):
        systemDB = MariaDB("system")
        sql = "SELECT symbol FROM crypto_market WHERE trade = True;"
        results = systemDB.query_fetchall(sql)
        symbols = list()
        for result in results:
            symbols.append(result[0])

        return symbols
  
    def upload(self, symbol):
        Logger("CryptoWebCrawler: " + symbol + " crawler started").log("INFO")
        month = self.month_from(symbol)
        self.wget_from_binance(symbol, month)
        self.unzip_all_files()
        self.upload_mariaDB(symbol)
        self.wget_remove_files()
        Logger("CryptoWebCrawler: " + symbol + " crawler terminated").log("INFO")
