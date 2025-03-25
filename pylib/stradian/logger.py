#!/usr/bin/env python3

""" logger.py
Logger(py) for Stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

import sys, time
sys.path.append("pylib/")
from stradian.slack import Slack

class Logger:
    def __init__(self, message,
                 is_slack = False, log_file = "var/log/system.log"):
        self.message = message
        self.is_slack = is_slack
        self.log_file = log_file
    
    def what(self):
        return self.message
    
    def log(self, level):
        log_content = "[%s] (%s) %s\n" %(self.get_time(), level, self.message)
        fp = open(self.log_file, 'a')
        fp.write(log_content)
        fp.close()
        
        if self.is_slack:
            slack = Slack()
            slack.post_message(log_content)

    def get_time(self):
        return time.strftime("%c", time.localtime(time.time()))
