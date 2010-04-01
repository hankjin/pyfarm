# This Python file uses the following encoding: utf-8
from sgmllib import SGMLParser
import urllib, urllib2
import time

def GetMovieTodayhref():
	x = time.localtime()
	str = "%i"%x.tm_year + "%i"%(x.tm_mon / 10) + "%i"%(x.tm_mon % 10) + "%i"%(x.tm_mday / 10) + "%i"%(x.tm_mday % 10)
	str = str[2:]
	href = 'http://www.mtime.com/showtime/China_Hubei_Province_Wuhan/list/' + str + '.html'
	return href

class MovieMtimeTime(SGMLParser):
	def __init__(self):
		SGMLParser.__init__(self)
		self.movietable = {}
		self.date = ''
		self.cinema = ''
		self.movie = ''
		self.tomorrowhref = ''
		self.aftomorrowhref = ''
		self.tmphref = ''
		self.bDate = False
		self.bTomorrowhref = False
		self.bAftomorrowhref = False
		self.bCinema = False
		self.bMovie = False
		self.bRCinema = False

	def start_a(self, attrs):
		for (k, v) in attrs:
			if k == 'href':
				self.tmphref = v

	def end_a(self):
		if self.bTomorrowhref == True:
			self.tomorrowhref = self.tmphref
			self.bTomorrowhref = False
		if self.bAftomorrowhref == True:
			self.aftomorrowhref = self.tmphref
			self.bAftomorrowhref = False

	def start_dt(self, attrs):
		if self.bRCinema == True:
			self.bMovie = True

	def end_dt(self):
		self.bMovie = False

	def start_body(self, attrs):
		pass

	def end_body(self):
		if self.tomorrowhref.find('http://www.mtime.com') == -1:
			self.tomorrowhref = 'http://www.mtime.com' + self.tomorrowhref
		if self.aftomorrowhref.find('http://www.mtime.com') == -1:
			self.aftomorrowhref = 'http://www.mtime.com' + self.aftomorrowhref

	def handle_data(self, text):
		if text == '收 藏':
			self.bCinema = True
			if self.cinema:
				self.movietable[self.cinema] = self.movie
		elif text == '图例说明：':
			self.movietable[self.cinema] = self.movie
		elif text.find('请选择时间：') != -1:
			self.bDate = True
		elif text == '明天':
			self.bTomorrowhref = True
		elif text == '后天':
			self.bAftomorrowhref = True
		else:
			text = text.replace('\n', '')
			text = text.replace('\t', '')
			text = text.replace('\r', '')
			text = text.replace(' ', '')
			if self.bCinema == True and text:
				self.cinema = text
				self.bCinema = False
				self.bRCinema = True
				self.movie = ''
			elif self.bMovie == True and text:
				if self.movie:
					self.movie += '|' + text
				else:
					self.movie += text
			elif self.bDate == True and text:
				self.date = text.split('，')[-1]
				self.bDate = False
			else:
				pass
	
	def GetMovieTomorrowhref(self):
		return self.tomorrowhref

	def GetMovieAftomorrowhref(self):
		return self.aftomorrowhref

	def GetMovieDate(self):
		return self.date

	def GetMovieTable(self):
		return self.movietable

if __name__ == '__main__':
	#print GetMovieTodayhref()
	link = GetMovieTodayhref() 
	req = urllib2.Request(link)
	data = urllib2.urlopen(req)
	time = MovieMtimeTime()
	time.feed(data.read())
	print time.GetMovieDate()
	print time.GetMovieTomorrowhref()
	print time.GetMovieAftomorrowhref()
	#for (cinema, movie) in time.GetMovieTable().items():
	#	print cinema
	#	print movie
