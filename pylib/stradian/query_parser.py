#!/usr/bin/env python3

""" query_parser.py
query_parser for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
from stradian.logger import Logger

import re
import pickle

class QueryParser:
    def __init__(self):
        self.db_structure = dict()
        self.warn = "NOT VALID"
        self.query_hash_file = "etc/pkl/query_hash_file.pkl"
        self.query_hash = self.load_query_hash()

    def load_query_hash(self):
        with open(self.query_hash_file, "rb") as fp:
            return pickle.load(fp)
        
    def dump_query_hash(self):
        with open(self.query_hash_file, "wb") as fp:
            pickle.dump(self.query_hash, fp)
        
    def update(self, structure):
        self.db_structure = structure
    
    def exist(self, db, table = None, attribute = None, dtype = None):
        if db not in self.db_structure.keys():
            return False
        
        if not table:
            return True
        
        if table not in self.db_structure[db].keys():
            return False

        if not attribute:
            return True
        
        if attribute not in self.db_structure[db][table].keys():
            return False

        if dtype and self.db_structure[db][table][attribute][0] != dtype:
            return False

        return True

    # QNT | QLT
    def get_dtype(self, db, table, attribute):
        return self.db_structure[db][table][attribute][0]

    def get_type(self, db, table, attribute):
        return self.db_structure[db][table][attribute][1]
    
    # PRI | MUL | None
    def get_ktype(self, db, table, attribute):
        return self.db_structure[db][table][attribute][2]

    def get_databases(self):
        return list(self.db_structure.keys())

    def get_tables(self, db):
        if not self.exist(db):
            return None
        return list(self.db_structure[db].keys())

    def get_attrs(*args):
        if len(args) == 0:
            return "*"
        
        sql = str()
        for attr in args:
            sql += attr + ", "
        return sql[: -2]
    
    def explore_sql(self, db, table, *args,
                    where_cond = None, limit = None, offset = None):
        attrs = self.get_attrs(args)
        sql = f"SELECT {attr} FROM {table}"
        
        if where_cond:
            sql += f" WHERE {where_cond}"

        if limit:
            sql += f" LIMIT {limit}"

        if offset:
            sql += f" OFFSET {offset}"
            
        return sql + ';'

    def build_sql(self, db, table, *args,
                group_by = None, having_cond = None,
                where_cond = None, limit = None, offset = None):
        sql = "SELECT @attr FROM @table GROUP BY @qtl HAVING @attr @cond WHERE @attr @cond LIMIT @int OFFSET @int"

        return sql + ';'
