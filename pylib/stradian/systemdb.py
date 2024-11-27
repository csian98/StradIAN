#!/usr/bin/env python3

""" systemdb_init.py
systemdb init code for Stradian
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

import hashlib
import random

class SystemDB:
    def __init__(self, db = "system"):
        self.mariadb = dict()
        self.structure = dict()
        self.mariadb[db] = MariaDB(db)
        self.structure[db] = self.get_structure(db)
        market_db = self.get_market_db()
        for mdb in market_db:
            self.mariadb[mdb] = MariaDB(mdb)
            self.structure[mdb] = self.get_structure(mdb)
    
    def create_table(self, json_file_name, db = "system"):
        json_object = read_json(json_file_name)
        sql = "CREATE TABLE `%s` (%s)" %(json_object["name"],
                                         attributes_format(
                                             json_object["attributes"],
                                             json_object["primary"],
                                             json_object["foreign"]))
        return self.mariadb[db].query(sql)
    
    def get_structure(self, db = "system"):
        o_cursor = self.mariadb[db].query("SHOW TABLES")
        tables = o_cursor.fetchall()
        
        structure = dict()
        for table in tables:
            table = table[0]
            schema = dict()
            i_cursor = self.mariadb[db].query("DESC `%s`" %table)
            
            fetch = i_cursor.fetchone()
            
            while fetch is not None:
                dtype = "QLT"
                if (fetch[1].find("char") == -1) and (fetch[1].find("text") == -1) and (fetch[1].find("time")):
                    dtype = "QNT"
                
                key = None
                if (fetch[3] != ""):
                    key = fetch[3]
                
                schema[fetch[0]] = [dtype, fetch[1], key]
                fetch = i_cursor.fetchone()
            
            structure[table] = schema
        
        return structure

    def get_schema(self, db, table):
        seperate = '+' + ('-' * 20) + '+' + ('-' * 20) + '+' + ('-' * 5) + '+' + ('-' * 5) + '+' + ('-' * 9)  + '+' + ('-' * 7) + "+\n"
        sql = f"DESC '{table}'"
        cursor = self.mariadb[db].query(
            "DESC `%s`" %table)
        fetch = cursor.fetchone()
        schema = seperate
        schema += "|%20s|%20s|%5s|%5s|%9s|%7s|\n" %(
            "Field", "Type", "Null", "Key", "Default", "Extra")
        schema += seperate
        
        while fetch is not None:
            schema += "|%20s|%20s|%5s|%5s|%9s|%7s|\n" %(
                fetch[0], fetch[1], fetch[2],
                fetch[3], fetch[4], fetch[5])
            fetch = cursor.fetchone()
        schema += seperate
        return schema

    def get_raw_data(self, db, table,
                     limit = None, offset = None):
        sql = f"SELECT * FROM `{table}`"
        
        if limit:
            sql += f" LIMIT {limit}"

        if offset:
            sql += f" OFFSET {offset}"
        
        cursor = self.mariadb[db].query(sql)
        return cursor.fetchall()
    
    def update_uid(self):
        sql = "SELECT uid FROM user ORDER BY uid DESC LIMIT 1"
        response = self.mariadb["system"].query(sql).fetchone()
        if response == None:
            return 1
        else:
            return response[0] + 1

    def insert_table(self, db, table, *args):
        sql = "INSERT INTO `%s` VALUES(%s)" %(table, args)
        return self.mariadb[db].query(sql)

    def login(self, user, pswd):
        pswd = hashlib.sha256(pswd.encode("utf-8")).hexdigest()
        cursor = self.mariadb["system"].query(
            f"SELECT COUNT(*) FROM user WHERE user = '{user}' and passwd = '{pswd}'")
        sz = cursor.fetchone()[0]
        if sz == 1:
            return True
        else:
            return False

    def get_user_info(self, user):
        cursor = self.mariadb["system"].query(
            "SELECT uid, auth, name FROM user WHERE user = '%s'" %user)
        fetch = cursor.fetchone()
        return (fetch[0], fetch[1], fetch[2])

    def get_market_type(self, trade = True):
        market_type = list()

        sql = "SELECT type FROM `market_type`"
        if trade is not None:
            sql += " WHERE trade = %s" %trade
        
        cursor = self.mariadb["system"].query(sql)
        for mtype in cursor.fetchall():
            market_type.append(mtype[0])

        return market_type

    def get_market_db(self, market_type = True):
        sql = "SELECT type, period FROM `market`"

        db_name = list()
        cursor = self.mariadb["system"].query(sql)
        for mtype, period in cursor.fetchall():
            db_name.append(mtype + "_" + period)

        return db_name

    def get_symbols(self, market_type, trade = True):
        sql = "SELECT symbol FROM %s_market" %market_type
        if trade is not None:
            sql += " WHERE trade = %s" %trade

        symbols = list()
        cursor = self.mariadb["system"].query(sql)
        fetch = cursor.fetchone()
        while fetch is not None:
            symbols.append(fetch[0])
            fetch = cursor.fetchone()

        return symbols

    def query(self, db, sql):
        return self.mariadb[db].query(sql)

    def get_minmax(self, db, table, attribute):
        min_sql = f"SELECT MIN({attribute}) FROM table"
        max_sql = f"SELECT MAX({attribute}) FROM table"
        
        cursor = self.mariadb[db].query(min_sql)
        min_value = cursor.fetchone()[0]
        cursor = self.mariadb[db].query(max_sql)
        max_value = cursor.fetchone()[0]

        return (min_value, max_value)

    def get_random_value(self, db, table, attribute):
        sql = f"SELECT DISTINCT {attribute} FROM {table}"

        cursor = self.mariadb[db].query(sql)
        values = cursor.fetchall()
        select = random.randint(0, len(values) - 1)
        return values[select][0]

    def get_level(self, db, table, attribute):
        sql = f"SELECT DISTINCT {attribute} FROM table"

        cursor = self.mariadb[db].query(sql)
        return cursor.fetchall()
