#!/usr/bin/python
#encoding: utf-8
import urllib,urllib2,cookielib,re

class XiaoNei:
	retry = True
	def __init__(self):
		cj = cookielib.CookieJar()
		opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
		opener.addheaders = [('User-Agent','Mozilla')]
		urllib2.install_opener(opener)
	def login(self,email,password):
		url = "http://passport.renren.com/PLogin.do"
		body = {"email":email,"password":password,"autoLogin":"true",
			"domain":"renren.com","origURL":"http://www.renren.com/SysHome.do"}
		ret = self.req(url,body)
		uids = re.findall('http\:\/\/renren\.com\/profile\.do\?id\=(\d+)',ret)
		if len(uids) == 0:
			print 'Uid not found'
		else:
			self.uid = int(uids[0])
		return self._login()

	def _login(self):
		pass

	def head(self,url,body={}):
		try:
			if len(body) == 0:
				req = urllib2.Request(url)
			else:
				req = urllib2.Request(url,urllib.urlencode(body))
			return urllib2.urlopen(req).headers
		except:
			print 'Req head Error %s' % url

	def req(self,url,body={},trys=0):
		if trys > 5:
			print url,body
			return ''
		try:
			if len(body) == 0:
				req=urllib2.Request(url)
			else:
				req=urllib2.Request(url,urllib.urlencode(body))
			return urllib2.urlopen(req).read()
		except:
			print 'Req Error url: %s' % url
			if self.retry:
				return self.req(url,body, trys+1)

if __name__ == '__main__':
	f = XiaoNei()
	f.login('hackjin@yeah.net','jinhank')
	print f.uid
