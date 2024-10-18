#!/usr/bin/env python3

""" chatdb_client.py
chatdb_client for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")

import socket, ssl
import threading
from getpass import getpass
from inputimeout import inputimeout

class ChatDBClient:
    def __init__(self, crt_file = "etc/c2c/stradian.crt"):
        self.HOST = "localhost"
        self.PORT = 50231
        self.context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        self.context.load_verify_locations(crt_file)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket = self.context.wrap_socket(
            self.socket, server_hostname = "Sian")
        self.socket.connect((self.HOST, self.PORT))
        
        self.quit = False
        return
    
    def run_client(self):
        t_recv = threading.Thread(target = self.sync_recv)
        t_recv.start()
        ###
        t_recv.join()

    def sync_recv(self):
        while not self.quit:
            msg = self.socket.recv(1024).decode("utf-8")
            if msg[0] == "#":
                self.parser(msg)
            else:
                print(msg, end = "")
        return
            
    def send(self, msg):
        self.socket.sendall(msg.encode("utf-8"))
        return

    def parser(self, msg):
        msg_list = msg.split("#")
        command = msg_list[1]
        
        match command:
            case "login":
                self.login()
            case "quit":
                self.quit = True
            case "ask":
                self.answer()
            case _:
                self.send("INVALID")
        return

    def login(self):
        user = input("user: ")
        self.send(user)
        pswd = getpass("pswd: ")
        self.send(pswd)
        return

    def answer(self):
        msg = input()
        self.send(msg)
