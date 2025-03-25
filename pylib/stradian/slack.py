#!/usr/bin/env python3

""" slack.py
MariaDB(py) class for stradian
"""
__author__ = "Jeong Hoon (Sian) Choi"
__copyright__ = "Copyright 2024 Jeong Hoon Choi"
__license__ = "MIT"
__version__ = "1.0.0"
__email__ = "csian7386@gmail.com"

from slack_sdk import WebClient
from slack_cleaner2 import SlackCleaner, match

def read_file(file_name):
    fp = open(file_name, 'r')
    text = fp.readline()
    fp.close()
    return text

class Slack:
    def __init__(self,
                 token = read_file("etc/slack/oauth"),
                 channel = read_file("etc/slack/system")):
        self.token = token
        self.channel = channel
        self.connect()

    def connect(self):
        self.client = WebClient(self.token)

    def post_message(self, text):
        result = self.client.chat_postMessage(
            channel = self.channel,
            text = text
        )
        return result
        
    def upload_file(self, file_path, title):
        result = self.client.files_upload(
            channels = self.channel,
            file = file_path,
            title = title
        )
        return result

    def clean_channel(self):
        cleaner = SlackCleaner(self.token)
        chat = 0
        for msg in cleaner.msgs(filters(match(channel_name), cleaner.conversations)):
            msg.delete(replies = True, files = True)
            chat += 1
        return chat
