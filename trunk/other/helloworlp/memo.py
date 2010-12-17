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
    content=db.StringProperty()
    @classmethod
    def add(cls, content, times):
	now=datetime.utcnow()
	for i in range(times):
	    memo=Memo(due=now+rules[i], content=content)
	    db.Model.put(memo)
    @classmethod
    def getTodos(cls):
	now=datetime.utcnow()
	memos=Memo.all().filter('due <',now)
	return memos
    @classmethod
    def rm(cls,memos):
	for memo in memos:
	    memo.delete()

class MemoHandler(BaseRequestHandler):
    def get(self,id):
	if not id=='': #delete a memo
	    id=int(id)
	    memo=Memo.get_by_id(id)
	    memo.delete()
        self.showMemos()
    def post(self,id=-1):
	content=self.param('content')
	times=self.param('times')
	if not times=='':
	    Memo.add(content, int(times))
	else:
	    memo=Memo(content=content)
	    year=int(self.param('year'))
	    month=int(self.param('month'))
	    day=int(self.param('day'))
	    hour=int(self.param('hour'))
	    minute=int(self.param('minute'))
	    memo.due=datetime(year,month,day,hour,minute)+timedelta(hours=-g_blog.timedelta)

	    db.Model.put(memo)
	self.showMemos()
    def showMemos(self):
	config=MemoConfig.getConfig()
	memos=Memo.all().order('due')
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

class MemoConfig(BaseRequestHandler):
    @classmethod
    def setConfig(cls,fetionNo, fetionPwd, phoneNo):
	OptionSet.setValue('fetionNo',fetionNo)
	OptionSet.setValue('fetionPwd',fetionPwd)
	OptionSet.setValue('phoneNo',phoneNo)

    @classmethod
    def getConfig(cls):
	return {'fetionNo': OptionSet.getValue('fetionNo'),
		'fetionPwd': OptionSet.getValue('fetionPwd'),
		'phoneNo': OptionSet.getValue('phoneNo')}
    def get(self):
	pass
    def post(self):
	fetionNo=self.param("fetionNo")
	fetionPwd=self.param("fetionPwd")
	phoneNo=self.param("phoneNo")
	MemoConfig.setConfig(fetionNo, fetionPwd, phoneNo)
	self.redirect("/memo/")

class MemoChecker(BaseRequestHandler):
    def get(self):
	memos=Memo.getTodos()
	if memos.count()>0:
	    body=''
	    for memo in memos:
		body+=memo.content+'\n'
	    self.informUser(body)
	    Memo.rm(memos)
	else:
	    self.response.out.write('no memo')
    def post(self):
	pass
    def informUser(self,memos):
	self.informMail(memos)
    def informPhone(self,memos):
	config=MemoConfig.getConfig()
	phone=PyFetion(config['fetionNo'],config['fetionPwd'],'HTTP')
	phone.login(FetionOnline)
	phone.send_sms(memos,config['phoneNo'])
	self.response.out.write('Fetioned')
    def informMail(self,memos):
	config=MemoConfig.getConfig()
	mail.send_mail(sender=users.get_current_user().email(), 
		to='%s@139.com' % config['phoneNo'],
		subject='Memo',
		body=memos)
	self.response.out.write('Emailed')

def main():
	webapp.template.register_template_library('filter')
	application = webapp.WSGIApplication(
					[
					('/memo/config',MemoConfig),
					('/memo/chk', MemoChecker),
					('/memo/(.*)',MemoHandler),
					 ],debug=True)
	wsgiref.handlers.CGIHandler().run(application)

if __name__ == "__main__":
	main()
