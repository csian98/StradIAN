#!/usr/bin/env python3

""" chatdb_client_main.py
chatdb_client_main for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, os
sys.path.append("pylib/")
from stradian.chatdb_client import ChatDBClient

if __name__ == "__main__":
    chatDBclient = ChatDBClient("etc/c2c/stradian.crt")
    chatDBclient.run_client()
    