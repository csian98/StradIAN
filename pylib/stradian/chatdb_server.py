#!/usr/bin/env python3

""" chatdb_server.py
chatdb_server for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
from stradian.systemdb import SystemDB
from stradian.query_parser import QueryParser
from stradian.logger import Logger

import socket, ssl
import threading
from queue import Queue
from getpass import getpass

import re

class ThreadClient:
    def __init__(self, client_socket, addr):
        self.client_socket = client_socket
        self.addr = addr
        self.uid = None
        self.user = None
        self.auth = None
        self.prompt = None
        self.login = False
        self.quit = None

class ChatDBServer:
    def __init__(self):
        self.systemdb = SystemDB("system")
        self.marketdbs = dict()
        self.query_parser = QueryParser()

        self.query_parser.update(self.systemdb.structure)
        return

    def send_client(self, tclient, msg):
        tclient.client_socket.sendall(msg.encode("utf-8"))
        return

    def recv_client(self, tclient):
        return tclient.client_socket.recv(1024).decode("utf-8")

    def chat_init(self, client):
        motd = ("\n"
                "==================================================\n"
                "\n"
                "                    StradIAN\n"
                "                     ChatDB\n"
                "\n"
                "==================================================\n")
    
        options = ("= MENU ===========================================\n"
                   "1) explore database\n"
                   "2) SQL queries\n"
                   "8) admin\n"
                   "9) logout\n"
                   "0) exit\n")
    
        seperate = "==================================================\n"

        while (not client.quit):
            self.send_client(client, motd)
            if not client.login:
                for i in range(5):
                    client.quit = True
                    if self.chat_login(client):
                        client.quit = False
                        break

                    self.send_client(client, seperate)
                    
                if client.quit:
                    self.send_client(
                        client, "5 Incorrect Password Attemps")
                    continue
            
            self.send_client(client, options)
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            match choice:
                case "1":
                    self.chat_explore_db(client)
                case "2":
                    self.chat_query_db(client)
                case "8":
                    self.chat_admin(client)
                case "9":
                    client.login = False
                case "0":
                    client.quit = True
                case _:
                    self.send_client(client, "\nInvalid Input\n")

        self.send_client(client, seperate)
        self.send_client(client, "#quit")
        return

    def chat_admin(self, client):
        options = ("= ADMIN ==========================================\n"
                   "1) add user\n"
                   "2) show hash\n"
                   "3) terminate server\n")
        
        if not client.auth:
            self.send_client(client, f"Admin Privileges required\n")

        while True:
            self.send_client(client, options)
            self.send_client(client, f"('q' to return)")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            if choice == 'q':
                return

            match choice:
                case "1":
                    print(None)
                case "2":
                    self.send_client(client, str(self.query_parser.query_hash))
                case "3":
                    self.send_client(client, "Terminate Server\n")
                    sys.exit(0)
                case _:
                    self.send_client(client, "\nInvalid Input\n")
        return

    def chat_login(self, client):
        self.send_client(client, "#login")
        user = self.recv_client(client)
        paswd = self.recv_client(client)
        check = self.systemdb.login(user, paswd)

        if check:
            info = self.systemdb.get_user_info(user)
            client.uid = info[0]
            client.auth = info[1]
            client.name = info[2]
            client.login = True
            position = "adm"
            if not client.auth:
                position = "usr"
            client.prompt = f"[${position}-{client.name}] ~>> "
            self.send_client(client, "\nLogin Success\n\n")
            Logger(
                f"ChatDB: {client.name}[{client.uid}] login as {position}", log_file = "var/log/chatdb.log").log("INFO")
            return True
        else:
            self.send_client(client, "\nLogin Failed\n\n")
            return False

    def chat_db_list(self, client, sep = True):
        dbs = self.query_parser.get_databases()
        if sep:
            self.send_client(
                client, "= SHOW DATABASES =================================\n")
        for i in range(len(dbs)):
            self.send_client(client, "%d. %s\n" %(i + 1, dbs[i]))
        self.send_client(client, '\n')
        return len(dbs)

    def chat_tbl_list(self, client, db, sep = True):
        tbls = self.query_parser.get_tables(db, client.auth)
        
        if sep:
            self.send_client(
                client, "= SHOW TABLES ====================================\n")
        for i in range(len(tbls)):
            self.send_client(client, "%d. %s\n" %(i + 1, tbls[i]))
        self.send_client(client, '\n')
        return len(tbls)

    def chat_explore_db(self, client):
        while True:
            self.send_client(
                client, "= EXPLORE DATABASE ===============================\n")
            n_db = self.chat_db_list(client, False)
            self.send_client(
                client, "Select the database to explore ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            if choice == 'q':
                return

            if (not choice.isdigit()) or (int(choice) < 1) or (int(choice) > n_db):
                self.send_client(client, "\nInvalid Answer\n")
                continue
            else:
                self.chat_explore_tbls(
                    client, self.query_parser.get_databases()[int(choice) - 1])

    def chat_explore_tbls(self, client, db):
        while True:
            n_tbls = self.chat_tbl_list(client, db, True)
            self.send_client(
                client, "Select the table to explore ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            if choice == 'q':
                return

            if (not choice.isdigit()) or (int(choice) < 1) or (int(choice) > n_tbls):
                self.send_client(client, "\nInvalid Answer\n")
                continue
            else:
                self.chat_explore_data(
                    client, db, self.query_parser.get_tables(db, client.auth)[int(choice) - 1])

    def chat_explore_data(self, client, db, table):
        options = ("= EXPLORE DATA ===================================\n"
                   "1) Schema\n"
                   "2) All Data <LIMIT;optional> <OFFSET;optional>\n")
        while True:
            self.send_client(client, options)
            self.send_client(
                client, "Select data to explore ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client).split(' ')

            match choice[0]:
                case 'q':
                    return
                case '1':
                    self.chat_schema(client, db, table)
                case '2':
                    offset = None
                    limit = None
                    
                    if len(choice) > 1:
                        limit = choice[1]

                    if len(choice) > 2:
                        offset = choice[2]
                        
                    self.chat_raw_data(client, db, table, limit, offset)
                case _:
                    self.send_client(client, "\nInvalid Input\n")
                    
    
    def chat_schema(self, client, db, table):
        msg = self.systemdb.get_schema(db, table)
        self.send_client(client, msg)
        return

    def chat_raw_data(self, client, db, table, limit = None, offset = None):
        columns = list(self.query_parser.db_structure[db][table].keys())
        line = "|"
        for column in columns:
            line += "%20s|" %column

        line += '\n'
        self.send_client(client, line)     
        
        instances = self.systemdb.get_raw_data(db, table, limit, offset)
        
        for instance in instances:
            line = "|"
            for value in instance:
                if isinstance(value, float):
                    line += "%20.6f|" %value
                elif isinstance(value, int):
                    line += "%20d|" %value
                else:
                    line += "%20s|" %value

            line += '\n'
            
            self.send_client(client, line)

    def chat_random_query(self, client, db, table,
                          query_type = None):
        where = False
        group_by = False
        having = False
        join = False
        order_by = False
        
        match query_type:                
            case "where":
                where = True
            case "group_by":
                group_by = True
            case "having":
                group_by = True
                having = True
            case "join":
                join = True
            case "order_by":
                order_by = True

        query, run_able, where_element, having_element = self.query_parser.random_query(
            db, table, where = where, group_by = group_by, having = having, join = join, order_by = order_by)
        return query, run_able, where_element, having_element
            
    def chat_sample_query(self, client, db, table):
        seperate = "==================================================\n"
        options = ("1) Another queries\n"
                   "2) Run Query   >>> 2 <QUERY NUM> <LIMIT;optional> <OFFSET;optional>\n")
        
        query_types = ["default", "where", "group_by",
                       "having", "join", "order_by"]

        while True:
            samples = "= SAMPLE QUERIES =================================\n"
            queries = dict()
        
            for query_type in query_types:
                query, run_able, where_element, having_element = self.chat_random_query(client, db, table, query_type)
                if query:
                    queries[query_type] = (
                        query, run_able, where_element, having_element)

            for i, query_type in enumerate(list(queries.keys())):
                samples += f"{i + 1}) {query_type}\n  {queries[query_type][0]}\n\n  {self.query_parser.make_explain(queries[query_type][0])}\n"

            while (True):
                self.send_client(client, samples)
                self.send_client(client, seperate)
                self.send_client(client, options)
                self.send_client(
                    client, "Select command to try ('q' to return)\n")
                self.send_client(client, client.prompt)
                self.send_client(client, "#ask")
                choice = self.recv_client(client).split(' ')

                match choice[0]:
                    case 'q':
                        return
                    case '1':
                        break
                    case '2':
                        if len(choice) < 2:
                            continue
                        
                        if int(choice[1]) < 1 or int(choice[1]) > len(queries):
                            self.send_client(client, "\nInvalid Answer\n")
                            continue

                        offset = None
                        limit = None
                    
                        if len(choice) > 2:
                            limit = choice[2]
                        
                        if len(choice) > 3:
                            offset = choice[3]
                        
                        query, run_able, where_element, having_element = queries[list(queries.keys())[
                            int(choice[1]) - 1]]
                        if run_able:
                            self.execute_query(client, db, query, limit, offset)
                        else:
                            where = None
                            having = None
                            if query.find("<#WHERE>") != -1:
                                self.send_client(client, f"{query}\n")
                                self.send_client(
                                    client, "where value (random for 'r')\n")
                                self.send_client(client, client.prompt)
                                self.send_client(client, "#ask")
                                recv = self.recv_client(client)
                                if recv == 'r':
                                    recv = self.random_value(
                                        db, table, where_element)
                                where = recv
                            
                            if query.find("<#HAVING>") != -1:
                                self.send_client(client, f"{query}\n")
                                self.send_client(
                                    client, "having value (random for 'r')\n")
                                self.send_client(client, client.prompt)
                                self.send_client(client, "#ask")
                                recv = self.recv_client(client)
                                if recv == 'r':
                                    recv = self.random_value(
                                        db, table, having_element)
                                having = recv

                            query, run_able = self.query_parser.set_value(
                                query, where, having)
                            self.execute_query(client, db, query, limit, offset)
                    case _:
                        self.send_client(client, "\nInvalid Input\n")
        
        return

    def chat_sample_query_with_keyword(self, client, db, table):
        keywords = "= SAMPLE QUERIES WITH KEYWORD ====================\n"
        seperate = "==================================================\n"
        
        query_types = ["default", "where", "group_by",
                       "having", "join", "order_by"]
        for i, qtype in enumerate(query_types):
            keywords += f"{i + 1}) {qtype}\n"
        
        while True:
            while True:
                self.send_client(client, keywords)
                self.send_client(client, seperate)
                self.send_client(client, "Select keyword to generate example query ('q' to return)\n")
                self.send_client(client, client.prompt)
                self.send_client(client, "#ask")
                recv = self.recv_client(client)

                if recv == 'q':
                    return

                if int(recv) < 1 or int(recv) > len(query_types):
                    self.send_client(client, "\nInvalid Answer\n")
                    continue

                query_type = query_types[int(recv) - 1]
                query, run_able, where_element, having_element = self.chat_random_query(client, db, table, query_type)
                
                self.send_client(client, f"Sample Query with {query_type}:\n  {query}\n")

                options = ("1) Another query\n"
                           "2) Run Query   >>> 2 <LIMIT;optional> <OFFSET;optional>\n")
                while True:
                    self.send_client(client, seperate)
                    self.send_client(client, options)
                    self.send_client(
                        client, "Select command to try ('q' to return)\n")
                    self.send_client(client, client.prompt)
                    self.send_client(client, "#ask")
                    choice = self.recv_client(client).split(' ')

                    match choice[0]:
                        case 'q':
                            return
                        case '1':
                            break
                        case '2':
                            offset = None
                            limit = None
                    
                            if len(choice) > 1:
                                limit = choice[1]
                        
                            if len(choice) > 2:
                                offset = choice[2]
                            
                            if run_able:
                                self.execute_query(client, db, query, limit, offset)
                            else:
                                where = None
                                having = None
                                if query.find("<#WHERE>") != -1:
                                    self.send_client(client, f"{query}\n")
                                    self.send_client(
                                        client, "where value (random for 'r')\n")
                                    self.send_client(client, client.prompt)
                                    self.send_client(client, "#ask")
                                    recv = self.recv_client(client)
                                    if recv == 'r':
                                        recv = self.random_value(
                                            db, table, where_element)
                                    where = recv
                            
                                if query.find("<#HAVING>") != -1:
                                    self.send_client(client, f"{query}\n")
                                    self.send_client(
                                        client, "having value (random for 'r')\n")
                                    self.send_client(client, client.prompt)
                                    self.send_client(client, "#ask")
                                    recv = self.recv_client(client)
                                    if recv == 'r':
                                        recv = self.random_value(
                                            db, table, having_element)
                                    having = recv

                                query, run_able = self.query_parser.set_value(
                                    query, where, having)
                                self.execute_query(client, db, query, limit, offset)
                        case _:
                            self.send_client(client, "\nInvalid Input\n")
        return
    
    def execute_query(self, client, db, sql,
                      limit = None, offset = None):
        if sql[-1] == ";":
            sql = sql[: -1]
        
        if limit:
            sql += f" LIMIT {limit}"

        if offset:
            sql += f" OFFSET {offset}"

        sql += ";"

        self.send_client(client, f">> {sql}\n")
        cursor = self.systemdb.query(db, sql) 
        fetch = cursor.fetchone()
        while fetch is not None:
            line = "|"
            for value in fetch:
                if isinstance(value, float):
                    line += "%20.6f|" %value
                elif isinstance(value, int):
                    line += "%20d|" %value
                else:
                    line += "%20s|" %value

            line += '\n'
            self.send_client(client, line)
            fetch = cursor.fetchone()
        return

    def random_value(self, db, table, attribute):
        value = str()
        value = self.systemdb.get_random_value(db, table, attribute)
        return value

    def chat_query_db(self, client):
        while True:
            self.send_client(
                client, "= QUERY DATABASE =================================\n")
            
            n_db = self.chat_db_list(client, False)
            self.send_client(
                client, "Select the database to query ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            if choice == 'q':
                return

            if (not choice.isdigit()) or (int(choice) < 1) or (int(choice) > n_db):
                self.send_client(client, "\nInvalid Answer\n")
                continue
            else:
                db = self.query_parser.get_databases()[int(choice) - 1]
                self.chat_query_tbls(client, db)

        return

    def chat_query_tbls(self, client, db):
        while True:
            n_tbls = self.chat_tbl_list(client, db, True)
            self.send_client(
                client, "Select the table to query ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            if choice == 'q':
                return

            if (not choice.isdigit()) or (int(choice) < 1) or (int(choice) > n_tbls):
                self.send_client(client, "\nInvalid Answer\n")
                continue
            else:
                self.chat_query(
                    client, db,
                    self.query_parser.get_tables(
                        db, client.auth)[int(choice) - 1])
        return

    def chat_query(self, client, db, table):
        options = ("= QUERY TABLE ====================================\n"
                   "1) Example SQL queries\n"
                   "2) Example SQL query with keyword\n"
                   "3) Execute Query\n"
                   "4) NLP Translate\n"
                   )
        while True:
            self.send_client(client, options)
            self.send_client(
                client, "Select data to explore ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client).split(' ')

            if choice[0] == 'q':
                break

            match choice[0]:
                case '1':
                    self.chat_sample_query(client, db, table)
                case '2':
                    self.chat_sample_query_with_keyword(client, db, table)
                case '3':
                    self.chat_custom_query(client, db, table)
                case '4':
                    self.chat_natural_language(client, db, table)
                case _:
                    self.send_client(client, "\nInvalid Input\n")
        return

    def chat_custom_query(self, client, db, table):
        custom = "= Execute Query ==================================\n"
        self.send_client(client, custom)
        
        while True:
            self.send_client(client, custom)
            self.chat_tbl_list(client, db)
            self.send_client(client, f"Write SQL query to execute ('q' to return)\n")
            self.send_client(client, f"Able to query {db} database tables\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            sql = self.recv_client(client)

            if sql == 'q':
                return
            
            try:
                self.execute_query(client, db, sql)
            except Exception as e:
                self.send_client(client, f"{sql} is not available\n")
        
        return

    def chat_natural_language(self, client, db, table):
        seperate = "= NLP Translate ==================================\n"
        options = ("1) Yes, Execute the query\n"
                   "2) No, Explain again\n")

        while True:
            self.send_client(client, seperate)
            self.chat_tbl_list(client, db)
            self.send_client(client, f"Describe the data you want to obtain ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            recv = self.recv_client(client)

            if recv == 'q':
                return

            nlp = recv
            sql = self.query_parser.find_similar(nlp)
            self.send_client(client, f"Is this query you want?\n{sql}\n")
            self.send_client(client, options)
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            recv = self.recv_client(client)

            if recv == '1':
                sql = self.replace_pattern(client, sql)

                try:
                    self.execute_query(client, db, sql)
                    #self.save_hash(sql, nlp)
                except:
                    self.send_client(client, f"Not allowed query\n{sql}\n")
            else:
                continue
                
        return

    def replace_pattern(self, client, sql):
        patterns = re.findall(r"<#.*?>", sql)
        
        for pattern in patterns:
            self.send_client(client, "Please enter pattern\n")
            self.send_client(client, f"{sql}\n")
            self.send_client(client, f"{pattern} >>> ")
            self.send_client(client, "#ask")
            recv = self.recv_client(client)

            sql = sql.replace(pattern, recv, 3)

        return sql
        
    
    def run_server(self):
        self.HOST = socket.gethostbyname(socket.gethostname())
        self.PORT = 50231
        self.context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        self.context.load_cert_chain(
            "etc/c2c/stradian.crt", "etc/c2c/prvstradian.key")
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((self.HOST, self.PORT))
        self.server_socket.listen()
        self.server_socket = self.context.wrap_socket(self.server_socket, server_side = True)
        self.thread_clients = list()
        Logger(
            "ChatDB: chatdb server started", log_file = "var/log/chatdb.log").log("INFO")

        while True:
            client_socket, addr = self.server_socket.accept()
            client = ThreadClient(client_socket, addr)
            self.thread_clients.append(client)
            t = threading.Thread(
                target = self.chat_init, args = (client,))
            t.start()
        
        return
