# This Python file uses the following encoding: utf-8
from sgmllib import SGMLParser
import urllib, urllib2

class MovieTime(SGMLParser):
	def __init__(self):
		SGMLParser.__init__(self)
		self.timetable = {}
		self.moviename = ''
		self.cinema = ''
		self.time = ''
		self.cinematable = {}
		self.nextpage = ''
		self.dcount = 3
		self.tmphref = ''
		self.bMovieName = False
		self.bTime = False
		self.bCinema = False
		self.bNextpage = False

	def start_img(self, attrs):
		count = 0
		for (k, v) in attrs:
			if k == 'width' and v == '60':
				count += 1
			if k == 'height' and v == '80':
				count += 1
		if count == 2:
			self.dcount = 0
			if self.moviename:
				self.timetable[self.moviename] = self.cinematable
				self.cinematable = {}

	def start_a(self, attrs):
		for (k, v) in attrs:
			if k == 'href':
				self.tmphref = v

	def start_b(self, attrs):
		if self.dcount == 0:
			self.bMovieName = True
			self.dcount += 1
		elif self.dcount == 1:
			self.bCinema = True
			self.dcount += 1
		elif self.dcount == 2:
			self.bCinema = True
		else:
			pass

	def end_b(self):
		if self.bMovieName == True:
			self.bMovieName = False
		elif self.bCinema == True:
			self.bCinema = False

	def start_font(self, attrs):
		pass

	def end_font(self):
		if self.bTime == True:
			self.bTime = False
			self.cinematable[self.cinema] = self.time
			self.time = ''

	def start_body(self, attrs):
		pass

	def end_body(self):
		self.timetable[self.moviename] = self.cinematable

	def handle_data(self, text):
		if text.find('地图') != -1:
			self.bTime = True
		elif text == '资讯':
			self.bTime = False
		elif text == '下一页':
			self.nextpage = self.tmphref
		else:
			text = text.replace('\r', '')
			text = text.replace('\n', '')
			text = text.replace('\t', '')
			text = text.replace(' ', '')
			if self.bMovieName == True and text:
				self.moviename = text
			elif self.bCinema == True and text:
				self.cinema = text
			elif self.bTime == True and text:
				if self.time:
					self.time += '|' + text
				else:
					self.time += text
			else:
				pass

	def GetMovieTimetable(self):
		return self.timetable
	
	def GetMovieNextTtable(self):
		return self.nextpage

if __name__ == '__main__':
	link = 'http://www.google.cn/movies?hl=zh-CN&sort=1&near=武汉市'
	req = urllib2.Request(link)
	req.add_header('User-agent', 'Mozilla/5.0')
	data = urllib2.urlopen(req)
	x = MovieTime()
	x.feed(data.read())
	for (k, v) in x.GetMovieTimetable().items():
		print k, '-----------------------'
		for (y, z) in v.items():
			print y
			print z
	print x.GetMovieNextTtable()
