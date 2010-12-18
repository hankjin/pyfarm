#encoding: utf-8
import cgi, os, sys, traceback
import wsgiref.handlers
##os.environ['DJANGO_SETTINGS_MODULE'] = 'settings'
##from django.conf import settings
##settings._target = None
os.environ['DJANGO_SETTINGS_MODULE'] = 'settings'
from django.utils.translation import check_for_language, activate, to_locale, get_language
from django.conf import settings
settings._target = None

from google.appengine.ext.webapp import template, \
	WSGIApplication
from google.appengine.api import users
from google.appengine.api import mail
#import app.webapp as webapp2
from google.appengine.ext import db
from google.appengine.ext import zipserve
from google.appengine.api import urlfetch
from google.appengine.api import memcache
from google.appengine.api.labs import taskqueue
from datetime import datetime ,timedelta
import base64,random,math
from django.utils import simplejson
import pickle
from model import *
from base import *
from PyFetion import *

rules=[timedelta(hours=+24),
	timedelta(days=+4),
	timedelta(days=+7),
	timedelta(minutes=+30),
	timedelta(minutes=+5),
	timedelta(days=+15),
	timedelta(hours=+12),
	timedelta(days=+2)]
class Memo(db.Model):
    due=db.DateTimeProperty()
    type=db.StringProperty()
    desc=db.StringProperty(multiline=True)
    content=db.TextProperty()
    @classmethod
    def add(cls, memos):
	descLen=100
	for memo in memos:
	    if len(memo.content)>descLen:
		memo.desc=memo.content[:descLen]+'...'
	    else:
		memo.desc=memo.content
	    db.Model.put(memo)
    @classmethod
    def getMemos(cls, type):
	return Memo.filter(type=type)
    @classmethod
    def rm(cls,memos):
	for memo in memos:
	    memo.delete()
    @classmethod
    def getTodos(cls):
	now=datetime.utcnow()
	memos=Memo.all().filter('due <',now)
	return memos

    @classmethod
    def genNormal(cls, content, times):
    	memos=[]
	now=datetime.utcnow()
	for i in range(times):
	    memo=Memo(type='normal', 
		    due=now+rules[i], 
		    content=content)
	    memos.append(memo)
	return memos
    @classmethod
    def genDaily(cls, content, hour):
	now=datetime(2000,1,1,0,0)
	memo=Memo(type='daily',
		due=now+timedelta(hours=hour-g_blog.timedelta),
		content=content)
	return memo
    @classmethod
    def genRandom(cls, content, hour):
	now=datetime.utcnow()
	today=datetime(now.year,now.month,now.day,0,0)
	memo=Memo(type='random',
		due=today+timedelta(hours=hour-g_blog.timedelta),
		content=content)
	return memo
class MemoHandler(BaseRequestHandler):
    def showMemos(self):
	config=MemoConfig.getConfig()
	memos=Memo.all().order('type').order('due')
	now=datetime.utcnow()+timedelta(minutes=+(60*g_blog.timedelta+5))
	years=range(now.year,now.year+3)
	months=range(1,12+1)
	days=range(1,31+1)
	hours=range(0,23+1)
	minutes=range(0,60,5)
	self.render('memo', {'memos':memos, 'config':config,
	    'years':years, 'months':months, 'days':days,
	    'hours':hours, 'minutes':minutes, 'now':now,
	    'defyear':now.year,'defmonth':now.month,'defday':now.day,
	    'defhour':now.hour,'defminute':now.minute/5*5})

class DailyMemoHandler(MemoHandler):
    def post(self):
	content=self.param('dailyContent')
	hour=int(self.param('dailyHour'))
	Memo.add([Memo.genDaily(content, hour)])
	self.showMemos()
class RandomMemoHandler(MemoHandler):
    def post(self):
	content=self.param('randomContent')
	hour=int(self.param('randomHour'))
	memo=Memo.genRandom(content, hour)
	Memo.add([Memo.genRandom(content, hour)])
	self.showMemos()
	
class NormalMemoHandler(MemoHandler):
    def get(self,id):
	if not id=='': #delete a memo
	    id=int(id)
	    memo=Memo.get_by_id(id)
	    memo.delete()
        self.showMemos()
    def post(self,id=-1):
	content=self.param('content')
	times=self.param('times')
	if not times=='': #1. a todo
	    Memo.add( Memo.genNormal(content, int(times)) )
	else: #2. memo
	    memo=Memo(type='normal',content=content)
	    year=int(self.param('year'))
	    month=int(self.param('month'))
	    day=int(self.param('day'))
	    hour=int(self.param('hour'))
	    minute=int(self.param('minute'))
	    memo.due=datetime(year,month,day,hour,minute)+timedelta(hours=-g_blog.timedelta)
	    Memo.add([memo])
	self.showMemos()
'''
Memo Config Section
'''
class MemoConfig(BaseRequestHandler):
    @classmethod
    def setConfig(cls,fetionNo, fetionPwd, phoneNo,sender,receiver):
	OptionSet.setValue('fetionNo',fetionNo)
	OptionSet.setValue('fetionPwd',fetionPwd)
	OptionSet.setValue('phoneNo',phoneNo)
	OptionSet.setValue('sender',sender)
	OptionSet.setValue('receiver',receiver)

    @classmethod
    def getConfig(cls):
	return {'fetionNo': OptionSet.getValue('fetionNo'),
		'fetionPwd': OptionSet.getValue('fetionPwd'),
		'phoneNo': OptionSet.getValue('phoneNo'),
		'sender': OptionSet.getValue('sender'),
		'receiver': OptionSet.getValue('receiver')
		}

    def get(self):
	pass
    def post(self):
	fetionNo=self.param("fetionNo")
	fetionPwd=self.param("fetionPwd")
	phoneNo=self.param("phoneNo")
	sender=self.param('sender')
	receiver=self.param('receiver')
	MemoConfig.setConfig(fetionNo, fetionPwd, phoneNo, sender, receiver)
	self.redirect("/memo/")

class MemoChecker(BaseRequestHandler):
    def get(self):
	memos=Memo.getTodos()
	now=datetime.utcnow()
	if memos.count()>0:
	    body=''
	    for memo in memos:
		if memo.type=='daily' or memo.type=='random':
		    #happen at same hour and 0-5 min
		    if memo.due.hour==now.hour and now.minute<=5:
			if memo.type=='daily':
			    body+=memo.content+'\n'
			elif memo.type=='random':
			    tips=memo.content.split('\n')
			    index=(now-memo.due).days #days
			    if len(tips)>0:
				body += tips[index%len(tips)]
		elif memo.type=='normal':
		    body+=memo.content+'\n'
	    self.informUser(body)
	    if memo.type=='normal':
		Memo.rm(memos)
	else:
	    self.response.out.write('no memo')
    def post(self):
	pass
    def informUser(self,memos):
	if len(memos)>0:
	    self.informMail(memos)
    def informPhone(self,memos):
	config=MemoConfig.getConfig()
	phone=PyFetion(config['fetionNo'],config['fetionPwd'],'HTTP')
	phone.login(FetionOnline)
	phone.send_sms(memos,config['phoneNo'])
	self.response.out.write('Fetioned')
    def informMail(self,memos):
	config=MemoConfig.getConfig()
	mail.send_mail(sender=config['sender'],
		to=config['receiver'],
		subject='Memo from helloworlp',
		body=memos)
	self.response.out.write('mail send')

def main():
	webapp.template.register_template_library('filter')
	application = webapp.WSGIApplication(
					[
					('/memo/config',MemoConfig),
					('/memo/chk', MemoChecker),
					('/memo/daily',DailyMemoHandler),
					('/memo/random',RandomMemoHandler),
					('/memo/(.*)',NormalMemoHandler),
					 ],debug=True)
	wsgiref.handlers.CGIHandler().run(application)

if __name__ == "__main__":
	main()
