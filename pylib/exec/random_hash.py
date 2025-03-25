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
import json
sys.path.append("pylib/")
from stradian.logger import Logger
from stradian.chatdb_server import ChatDBServer

if __name__ == "__main__":
    chatdb_server = ChatDBServer()
    random_size = 1000

    json_object = json.load(open("etc/query/query_explain.json", 'r'))
    chatdb_server.query_parser.query_hash.clear()
    print("before hash size: %d" %len(chatdb_server.query_parser.query_hash))
    
    db_structure = chatdb_server.query_parser.db_structure
    for db in db_structure.keys():
        for table in db_structure[db].keys():
            keywords = ["default", "where", "group_by",
                        "having", "join", "order_by"]

            for query_type in keywords:
                for i in range(random_size):
                    query, _, _, _ = chatdb_server.chat_random_query(None, db, table, query_type)
                    if query:
                        sql, explain = chatdb_server.query_parser.make_explain(query, True)
#                        explain = chatdb_server.query_parser.make_explain(query, False)
                        # pattern, kilt = chatdb_server.query_parser.simple_patternize(explain)

                        #for key in kilt.keys():
                        #    value = kilt[key]
                        #    query = query.replace(value, key)
                        
                        chatdb_server.query_parser.save_hash(sql, explain)


    for query in json_object.keys():
        explains = json_object[query]
        for explain in explains:
            chatdb_server.query_parser.save_hash(query, explain)

    print("after hash size: %d" %len(chatdb_server.query_parser.query_hash))
    chatdb_server.query_parser.dump_query_hash()
    chatdb_server.query_parser.dump_query_dictionary()
