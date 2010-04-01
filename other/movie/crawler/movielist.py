# This Python file uses the following encoding: utf-8
from sgmllib import SGMLParser
import urllib, urllib2

class MovieList(SGMLParser):
	def __init__(self):
		SGMLParser.__init__(self)
		self.moviename = ''
		self.moviehref = ''
		self.movielist = {}
		self.bMoviename = False
		self.bRdata = False

	def start_a(self, attrs):
		for (k, v) in attrs:
			if k == 'href' and self.bMoviename == True:
				if v.find('http://www.mtime.com') == -1:
					v = 'http://www.mtime.com' + v
				self.moviehref = v
				self.bRdata = True

	def end_a(self):
		if self.bRdata == True:
			self.movielist[self.moviename] = self.moviehref
			self.bRdata = False

	def handle_data(self, text):
		if text == '预告片':
			self.bMoviename = True
		else:
			if self.bMoviename == True:
				self.moviename = text
				self.bMoviename = False
			else:
				pass

	def GetMovieList(self):
		return self.movielist

if __name__ == '__main__':
	link = 'http://www.mtime.com/showtime/'
	req = urllib2.Request(link)
	data = urllib2.urlopen(req)
	x = MovieList()
	x.feed(data.read())
	for (k, v) in x.GetMovieList().items():
		print k, v
