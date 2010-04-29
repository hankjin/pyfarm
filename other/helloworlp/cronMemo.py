#!/usr/bin/python
#--encoding: utf-8 --
import urllib,smtplib,base64,pickle,time,os
import os,time,pickle
from PyFetion import *

def saveNotes(notes):
    pickle.Pickler(open(noteFile,'w')).dump(notes)

def inform(content):
    print content
    phone=PyFetion("13407100014","mypassword","HTTP")
    phone.login(FetionOnline)
    phone.send_sms(content,"13407100014")
if __name__=='__main__':
    inform('hello')

