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
from stradian.mariadb import read_json

import re, string
import pickle
import random
import heapq

import nltk
import spacy
from spacy.tokenizer import Tokenizer
from spacy.util import compile_infix_regex

class QueryParser:
    def __init__(self):
        self.db_structure = dict()
        self.warn = "NOT VALID"
        self.query_hash_file = "etc/pkl/query_hash_file.pkl"
        self.query_dictionary_file = "etc/pkl/query_dictionary.pkl"
        self.query_hash = self.load_query_hash()
        self.dictionary = self.load_query_dictionary()
        self.nlp = spacy.blank("en")
        self.nlp.tokenizer = self.get_tokenizer(self.nlp)

    def get_tokenizer(self, nlp):
        tokenizer = nlp.tokenizer

        infixes = nlp.Defaults.infixes + [r"<#\w+>"]
        infix_re = compile_infix_regex(infixes)

        tokenizer = Tokenizer(nlp.vocab, infix_finditer = infix_re.finditer)
        
        return tokenizer
        
    def load_query_hash(self):
        with open(self.query_hash_file, "rb") as fp:
            return pickle.load(fp)
    
    def dump_query_hash(self):
        with open(self.query_hash_file, "wb") as fp:
            pickle.dump(self.query_hash, fp)

    def load_query_dictionary(self):
        with open(self.query_dictionary_file, "rb") as fp:
            return pickle.load(fp)

    def dump_query_dictionary(self):
        with open(self.query_dictionary_file, "wb") as fp:
            pickle.dump(self.dictionary, fp)
        
    def update(self, structure):
        self.db_structure = structure
    
    def get_auth(self, db, table):
        json_file_name = "etc/json/" + db + "/" + table + ".json"
        json_object = read_json(json_file_name)
        return json_object["auth"]
    
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

    def get_tables(self, db, auth = True):
        if not self.exist(db):
            return None

        tables = list(self.db_structure[db].keys())
        if auth:
            return tables
        else:
            auth_tables = list()
            for table in tables:
                if self.get_auth(db, table):
                    auth_tables.append(table)

            return auth_tables

    def get_attrs(self, *args):
        if isinstance(args[0], list):
            args = args[0]
        
        if len(args) == 0:
            return "*"
        
        sql = str()
        for attr in args:
            sql += f"{attr}, "
        return sql[: -2]
    
    def explore_sql(self, db, table, *args,
                    where_cond = None, limit = None, offset = None):
        attrs = self.get_attrs(args)
        sql = f"SELECT {attr} FROM `{table}`"
        
        if where_cond:
            sql += f" WHERE {where_cond}"

        if limit:
            sql += f" LIMIT {limit}"

        if offset:
            sql += f" OFFSET {offset}"
            
        return sql + ';'

    def random_attributes(self, db, table, group_by = None):
        json_object = self.db_structure[db][table]
        attributes = list()
        
        if not group_by:
            for key in json_object.keys():
                select = random.randint(0, 1)
                if select:
                    attributes.append(key)
        else:
            attributes.append(group_by)
            aggregates = ["COUNT", "AVG", "SUM", "MAX", "STD"]
            select = random.randint(0, len(aggregates) - 1)

            aggregate = aggregates[select]
            if aggregate == "COUNT":
                attributes.append("COUNT(*)")
            else:
                qnts = [key for key, value in json_object.items() if value[0] == "QNT"]
                if len(qnts):
                    select = random.randint(0, len(qnts) - 1)
                    attributes.append(f"{aggregate}({qnts[select]})")

        sql = self.get_attrs(attributes)
        if sql == "*":
            attributes = list(json_object.keys())
        
        return sql, attributes

    def random_attributes_join(self, db, l_table, r_table, l_attr, r_attr):
        left_object = self.db_structure[db][l_table]
        right_object= self.db_structure[db][r_table]

        attributes = list()
        attributes.append(f"l.{l_attr}")

        for key in left_object.keys():
            select = random.randint(0, 1)
            if select and key != l_attr:
                attributes.append(f"l.{key}")

        for key in right_object.keys():
            select = random.randint(0, 1)
            if select and key != r_attr:
                attributes.append(f"r.{key}")

        sql = self.get_attrs(attributes)
        return sql, attributes

    def random_group_by(self, db, table):
        file_name = f"etc/json/{db}/{table}.json"
        if db != "system":
            file_name = f"etc/json/{db}/{db}_format.json"
        
        foreign_keys = read_json(file_name)["foreign"]
        if not foreign_keys:
            return None

        select = random.randint(0, len(foreign_keys) - 1)
        key = list(foreign_keys.keys())[select]
        sql = f"GROUP BY {key}"
        return sql, key
    
    def random_where(self, db, table, attributes, value = None):
        run_able = True
        if not value:
            value = "<#WHERE>"
            run_able = False
        
        conditions = ["=", "<", "<=", ">", ">="]
        select = random.randint(0, len(conditions) - 1)
        condition = conditions[select]

        select = random.randint(0, len(attributes) - 1)
        attribute = attributes[select]
        
        sql = f"WHERE {attribute} {condition} {value}"
        return sql, run_able, attribute

    def random_having(self, db, table, group_by, value = None):
        run_able = True
        if not value:
            value = "<#HAVING>"
            run_able = False
        
        conditions = ["=", "<", "<=", ">", ">="]
        select = random.randint(0, len(conditions) - 1)
        condition = conditions[select]

        aggregates = ["COUNT", "AVG", "SUM", "MAX", "STD"]
        select = random.randint(0, len(aggregates) - 1)
        aggregate = aggregates[select]

        if aggregate == "COUNT":
            aggregate = "COUNT(*)"
        else:
            json_object = self.db_structure[db][table]
            qnts = [key for key, value in json_object.items() if value[0] == "QNT" and key != group_by]

            if len(qnts):
                select = random.randint(0, len(qnts) - 1)
                aggregate = f"{aggregate}({qnts[select]})"
            else:
                aggregate = "COUNT(*)"
        
        sql = f"HAVING {aggregate} {condition} {value}"
        return sql, run_able, aggregate

    def random_order_by(self, db, table, attributes):
        json_object = self.db_structure[db][table]

        select = random.randint(0, len(attributes) - 1)
        order = "ASC" if random.randint(0, 1) else "DESC"
        sql = f"ORDER BY {attributes[select]} {order}"
        return sql

    def random_join(self, db, table):
        file_name = f"etc/json/{db}/{table}.json"
        if db != "system":
            file_name = f"etc/json/{db}/{db}_format.json"
        
        foreign_keys = read_json(file_name)["foreign"]
        
        if not foreign_keys:
            return None

        select = random.randint(0, len(foreign_keys) - 1)
        left_attribute = list(foreign_keys.keys())[select]
        right_table = foreign_keys[left_attribute].split('(')[0]
        right_attribute = foreign_keys[left_attribute].split('(')[1][: -1]
        
        sql = f"AS l LEFT JOIN `{right_table}` AS r ON l.{left_attribute} = r.{right_attribute}"
        return sql, left_attribute, right_table, right_attribute

    def random_query(self, db, table, where = False, where_value = None,
                     group_by = False, having = False, having_value = None,
                     join = False, order_by = False, limit = None, offset = None):
        sql = "SELECT "
        run_able = True
        where_element = None
        having_element = None
        
        if group_by:
            group = self.random_group_by(db, table)
            if not group:
                return None, False, None, None
            
            group, group_element = group

            attribute, l_atr = self.random_attributes(db, table, group_element)
            sql += attribute
            sql += f" FROM `{table}`"
            sql += f" {group}"

            if having:
                having, run_able, having_element = self.random_having(
                    db, table, group_element, having_value)
                sql += f" {having}"

        elif join:
            join = self.random_join(db, table)

            if not join:
                return None, False, None, None

            join, left_element, right_table, right_element = join

            attribute, l_atr = self.random_attributes_join(db, table, right_table,
                                                           left_element,
                                                           right_element)
            
            sql += attribute
            sql += f" FROM `{table}`"
            sql += f" {join}"
            
        else:
            attribute, l_atr = self.random_attributes(db, table)
            sql += attribute
            sql += f" FROM `{table}`"

        if where:
            where_cond, run_able, where_element = self.random_where(
                db, table, l_atr, where_value)
            sql += f" {where_cond}"

        if order_by:
            order = self.random_order_by(db, table, l_atr)
            sql += f" {order}"
        
        if limit:
            sql += " LIMIT %d" %limit

        if offset:
            sql += " OFFSET %d" %offset

        sql += ';'
        
        return sql, run_able, where_element, having_element
    
    def set_value(self, sql, where = None, having = None):
        if where:
            if isinstance(where, int) or isinstance(where, float):
                sql = sql.replace("<#WHERE>", str(where))
            else:
                sql = sql.replace("<#WHERE>", f"'{where}'")

        if having:
            if isinstance(having, int) or isinstance(having, float):
                sql = sql.replace("<#HAVING>", str(having))
            else:
                sql = sql.replace("<#HAVING>", f"'{having}'")

        return sql, sql.find("<#WHERE>") != -1 and sql.find("<#HAVING>") != -1

    def attribute_parsing(self, sql):
        if sql[-1] == ' ' or sql[-1] == ',':
            sql = sql[: -1]
        
        attributes = sql.split(',')
        sql = str()
        
        if attributes[0] == '*':
            return "all"
        
        functions = {
            "COUNT": "count <#VALUES>",
            "AVG": "average of <#VALUES>",
            "SUM": "sum of <#VALUES>",
            "MAX": "max value of <#VALUES>",
            "MIN": "min value of <#VALUES>",
            "STD": "standard deviation of <#VALUES>"
        }
        
        for attribute in attributes:
            if attribute[0] == ',' or attribute[0] == ' ':
                attribute = attribute[1:]
            
            if attribute.find('(') == -1:
                if attribute[:2] == "l.":
                    attribute = "left table's " + attribute[2:]

                if attribute[:2] == "r.":
                    attribute = "right table's " + attribute[2:]
                
                sql += attribute + ", "
            else:
                f = attribute.find('(')
                b = attribute.find(')')
                function = attribute[: f]
                value = attribute[f + 1: b]
                natural = functions[function]
                natural = natural.replace("<#VALUES>", value)
                sql += natural + ", "

        return sql[: -2]

    def make_explain(self, sql, for_pattern = False):
        invalid = "INVALID QUERY"
        if sql[-1] == ';':
            sql = sql[: -1]

        keywords = {
            "SELECT": "show the <#SELECT>",
            "FROM": "in the <#FROM> table",
            "AS": "(called as <#AS>)",
            "WHERE": "with <#WHERE>",
            "GROUP BY": "by <#GROUPBY> group",
            "HAVING": "that is <#HAVING>",
            "LEFT JOIN": "joining the <#LEFTJOIN> table",
            "ON": "that have <#ON>",
            "ORDER BY": "in ascending order of <#ORDERBY>",
            "LIMIT": "only <#LIMIT> rows",
            "OFFSET": "skip <#OFFSET> rows"
        }

        qsql = str()
        
        has_key = list()
        for key in keywords.keys():
            loc = sql.find(f"{key} ")
            if loc != -1:
                has_key.append([loc, key])
                qsql += f"{key} <#{key.replace(' ', '')}> "

        qsql = qsql[: -1] + ';'

        has_key = sorted(has_key)
        description = str()
        pattern = str()
        
        for i in range(len(has_key)):
            loc, key = has_key[i]
            if len(description) != 0 and description[-1] != ' ':
                description += ' '
                pattern += ' '
            
            value = str()
            if i == len(has_key) - 1:
                value = sql[loc + len(key) + 1: ]
            else:
                value = sql[loc + len(key) + 1: has_key[i + 1][0]]
            
            natural = keywords[key]

            if key == "SELECT":
                value = self.attribute_parsing(value)
            
            if key == "ORDER BY":
                if sql.find("DESC") != -1:
                    natural = natural.replace("ascending", "descending")
                    natural = natural.replace("ASC", '')
                    natural = natural.replace("DESC", '')

            pattern += f"{natural}"
            natural = natural.replace(f"<#{key.replace(' ', '')}>", value)
            description += f"{natural}"

        if description[-1] == ' ':
            description = description[: -1]
            pattern = pattern[: - 1]

        #self.save_hash(qsql, pattern)
        if for_pattern:
            return (qsql, pattern)
            
        return description

    def patternize(self, explain):
        keywords = {"<#SELECT>", "<#FROM>",
                    "<#AS>", "<#WHERE>",
                    "<#GROUPBY>", "<#HAVING>",
                    "<#LEFTJOIN>", "<#ON>",
                    "<#ORDERBY>", "<#LIMIT>"
                    "<#OFFSET>"}
        
        kilt = dict()

        split = explain.split(' ')
        dbs = self.get_databases()
        tables = dict()
        for db in dbs:
            table = self.get_tables(db)
            for t in table:
                tables[t] = db

        flag = True
        for i in range(len(split)):
            table = split[i].lower()
            if table[0] == '`':
                table = table[1:]
            if table[-1] == '`':
                table = table[: -1]
                
            if table in tables and flag:
                kilt["<#FROM>"] = split[i]
                split[i] = "<#FROM>"
                flag = False

                if (len(split) > i + 3 and
                    split[i + 1].lower() in ["as", "like", "likes"]):
                    kilt["<#AS>"] = split[i + 2]
                    split[i + 2] = "<#AS>"

                continue

            if table in tables and not flag:
                kilt["<#LEFTJOIN>"] = split[i]
                split[i] = "<#LEFTJOIN>"
                flag = True

                if (len(split) > i + 3 and
                    split[i + 1].lower() in ["as", "like", "likes"]):
                    kilt["<#AS>"] = [kilt["<#AS>"], split[i + 2]]
                    split[i + 2] = "<#AS>"
                
                break

        if "<#FROM>" not in kilt.keys():
            return None
        
        table = kilt["<#FROM>"]
        if table[0] =='`':
            table = table[1: ]
        if table[-1] == '`':
            table = table[: -1]

        
        attributes = list(self.db_structure[tables[table]][table].keys())
        attributes = list(map(lambda x: x.lower(), attributes))
        
        for i in range(len(split)):
            text = split[i]
            if split[i].find('.') != -1:
                text = split[i].split('.')[1]

            if text[-1] == ',':
                text = text[: -1]
            
            if text.lower() in attributes:
                if ((i != 0 and split[i - 1].find("group") != -1) or
                    (i != 1 and split[i - 2].find("group") != -1)):
                    kilt["<#GROUPBY>"] = split[i]
                    split[i] = "<#GROUPBY>"

                elif ((i != 0 and split[i - 1].find("order") != -1) or
                    (i != 1 and split[i - 2].find("order") != -1)):
                    kilt["<#ORDERBY"] = split[i]
                    split[i] = "<#ORDERBY>"

                elif (len(split) > i + 2 and split[i + 1] in ["=", "<", ">", "<=", ">="]):
                    condition = ["<#LEFTJOIN>", "<#GROUPBY>", "<#FROM>"]
                    queue = list()
                    for j in range(i):
                        if split[j] in condition:
                            queue.append(split[j])

                    keyword = "<#WHERE>"
                    if queue[-1] == "<#LEFTJOIN>":
                        keyword = "<#ON>"
                    elif queue[-1] == "<#GROUPBY>":
                        keyword = "<#HAVING>"

                    kilt[keyword] = ' '.join(split[i: i + 3])
                    for j in range(3):
                        split[i + j] = keyword

                else:    
                    k = i
                    for j in range(i + 1, len(split)):
                        point = split[j]
                        if split[j].find('.') != -1:
                            point = split[j].split('.')[1]

                        if point not in attributes:
                            break
                        else:
                            k = j

                    kilt["<#SELECT>"] = ' '.join(split[i: k + 1])

                    
                    for star in ["all", "entire", "every"]:
                        if kilt["<#SELECT>"].find(star) != -1:
                            kilt["<#SELECT>"] = '*'
                            break
                    
                    for j in range(i, k + 1):
                        split[j] = "<#SELECT>"

            elif text.isdigit():
                if (i != 0 and split[i - 1].find("skip") != -1):
                    kilt["<#OFFSET>"] = split[i]
                    split[i] = "<#OFFSET>"

                if (i != 0 and split[i - 1].find("only") != -1):
                    kilt["<#LIMIT>"] = split[i]
                    split[i] = "<#LIMIT>"

        split_wo_dup = list()
        for i in range(len(split) - 1):
            if split[i] != split[i + 1]:
                split_wo_dup.append(split[i])

        explain = ' '.join(split_wo_dup)
        return explain, kilt

    def simple_patternize(self, explain):
        splits = explain.split(' ')
        sub_string = "<#WHERE>"
        kilt = dict()
        for i in range(len(splits)):
            if splits[i].replace('.', '').isdigit():
                kilt[sub_string] = splits[i]
                splits[i] = sub_string
                sub_string = "<#HAVING>"

        explain = ' '.join(splits)
        return explain, kilt

    def reverse_patternize(self, sql, kilt):
        for key, value in kilt.items():
            sql = sql.replace(key, value)
        
        return sql

    def tokenize(self, sentence):
        doc = self.nlp(sentence)
        # tokens = {token.text.lower() for token in doc if not token.is_stop and not token.is_punct}
        tokens = {token.text.lower() for token in doc if not token.is_punct}
        def lower(token):
            if re.match(r"<#.*>", token.text):
                return token.text
            else:
                text = token.text
                text.translate(str.maketrans('', '', string.punctuation))
                return text.lower()
            
        tokens = {lower(token) for token in doc if not token.is_punct}
        return tokens
        
    def token_similarity(self, tok1, tok2):
#        tok1 &= self.dictionary
#        tok2 &= self.dictionary       
        jacard = len(tok1 & tok2) / len(tok1 | tok2) if tok1 | tok2 else 0.0
        return jacard

    def save_hash(self, sql, explain):
        token = self.tokenize(explain)
        new_set = {tok for tok in token}
#        self.dictionary |= new_set
        self.query_hash[frozenset(token)] = sql

    def find_similar(self, explain, simple = False):
        patterned = self.patternize(explain)
        if simple:
            patterned = self.simple_patternize(explain)
        
        if not patterned:
            return "SELECT * FROM <#TABLE>;"
            
        pattern, kilt = patterned
        
        max_jacard = 0.0
        max_sql = str()
        tokens = self.query_hash.keys()
        token = self.tokenize(pattern)

        for other_token in tokens:
            jacard = self.token_similarity(token, other_token)
            if jacard > max_jacard:
                max_jacard = jacard
                max_sql = self.query_hash[other_token]

        sql = self.reverse_patternize(max_sql, kilt)
        return sql

    def find_similars(self, explain, size = 10, simple = False):
        patterned = self.patternize(explain)
        if simple:
            patterned = self.simple_patternize(explain)

        if not patterned:
            return "SELECT * FROM <#TABLE>;"

        pattern, kilt = patterned
        heap = list()

        tokens = self.query_hash.keys()
        token = self.tokenize(pattern)

        for other_token in tokens:
            jacard = self.token_similarity(token, other_token)
            heapq.heappush(heap, (jacard, other_token))

        sqls = list()
        for _ in range(size):
            popped = heapq.heappop(heap)
            if not popped:
                break

            token = popped[1]
            sql = self.reverse_patternize(self.query_hash[token], kilt)
            if not sql:
                return None
            sqls.append(sql)
            
        return sqls
