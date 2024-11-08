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
                   "1) show databases\n"
                   "2) explore database\n"
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
                    self.chat_db_list(client, True)
                case "2":
                    self.chat_explore_db(client)
                case "9":
                    client.login = False
                case "0":
                    client.quit = True
                case _:
                    self.send_client(client, "\nInvalid Input\n")

        self.send_client(client, seperate)
        self.send_client(client, "#quit")
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
                   "2) Raw Data\n")
        while True:
            self.send_client(client, options)
            self.send_client(
                client, "Select data to explore ('q' to return)\n")
            self.send_client(client, client.prompt)
            self.send_client(client, "#ask")
            choice = self.recv_client(client)

            match choice:
                case 'q':
                    return
                case '1':
                    self.chat_schema(client, db, table)
                case '2':
                    self.chat_raw_data(client, db, table)
    
    def chat_schema(self, client, db, table):
        msg = self.systemdb.get_schema(db, table)
        self.send_client(client, msg)
        return

    def chat_raw_data(self, client, db, table):
        columns = list(self.query_parser.db_structure[db][table].keys())
        line = "|"
        for column in columns:
            line += "%20s|" %column

        line += '\n'
        self.send_client(client, line)        
        
        instances = self.systemdb.get_raw_data(db, table)
        
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
