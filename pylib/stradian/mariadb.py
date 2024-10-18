#!/usr/bin/env python3

""" mariadb.py
MariaDB(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
import pymysql, json
from stradian.logger import Logger

def read_file(file_name):
    fp = open(file_name, 'r')
    text = fp.readline()
    fp.close()
    return text

def read_json(json_file_name):
    fp = open(json_file_name, 'r')
    json2dict = json.load(fp)
    return json2dict

def attributes_format(attributes, primary, foreign = None):
    keys = attributes.keys()
    sql = ""
    for key in keys:
        sql += "%s %s, " %(key, attributes[key])
    
    if len(primary):
        sub = ""
        for pk in primary:
            sub += "%s, " %pk
        
        sql += "PRIMARY KEY (%s), " %sub[: -2]
    
    if foreign:
        sub = ""
        keys = foreign.keys()
        for key in keys:
            sub += "FOREIGN KEY (%s) REFERENCES %s, " %(key, foreign[key])
        
        sql += sub
    
    sql = sql[: - 2]
    return sql

class MariaDB:
    def __init__(self, db,
                 user = read_file("etc/dbms/mariadb_user"),
                 host = "localhost",
                 password = read_file("etc/dbms/mariadb_passwd"),
                 port = 3306, autocommit = True):
        self.user = user
        self.host = host
        self.password = password
        self.db = db
        self.port = port
        self.autocommit = autocommit
        self.connection = self.connect()
    
    def connect(self):
        return pymysql.connect(
            user = self.user,
            host = self.host,
            passwd = self.password,
            port = self.port,
            db = self.db,
            autocommit = self.autocommit,
            charset = "utf8"
        )
    
    def disconnect(self):
        self.connection.close()

    def commit(self):
        self.connection.comit()

    def query(self, sql):
        cursor = self.connection.cursor()
        cursor.execute(sql)
        return cursor

    def query_fetchone(self, sql):
        return self.query(sql).fetchone()[0]

    def query_fetchall(self, sql):
        return self.query(sql).fetchall()

    def skip_cursor(self, cursor, n):
        for i in range(n):
            cursor.fetchone()

        return cursor
    
    def upload_df(self, table, df):
        data = list(df.itertuples(index = False, name = None))
        sql = f"REPLACE `{table}` VALUES ("

        for i in range(df.shape[1] - 1):
            sql += "%s, "

        sql += "%s)"
        
        cursor = self.connection.cursor()
        cursor.executemany(sql, data)
