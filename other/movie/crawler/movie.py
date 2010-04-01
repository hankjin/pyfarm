# This Python file uses the following encoding: utf-8
from sgmllib import SGMLParser
import urllib, urllib2

class MtimeMovie(SGMLParser):
	def __init__(self, movieHref):
		SGMLParser.__init__(self)
		self.name = ''
		self.movieid = movieHref.split('/')[-2]
		self.href = movieHref
		self.director = ''
		self.actor = ''
		self.area = ''
		self.category = ''
		self.descriptionhref = ''
		self.posterhref = ''
		self.commenthref = []
		self.commentcount = 0
		self.previewhref = ''
		self.tmphref = ''
		self.bName = False
		self.bDirector = False
		self.bActor = False
		self.bArea = False
		self.bCategory = False
		self.bRCate = False
		self.bPoster = False
		self.bComment = False
		self.bTCom = False
		self.bPreview = False

	def start_a(self, attrs):
		for (k, v) in attrs:
			if k == 'href':
				self.tmphref = v
				if self.tmphref == self.href:
					self.bName = True	
				if self.bComment == True and self.commentcount < 3:
					if v.find('http://www.mtime.com') == -1:
						v = 'http://www.mtime.com' + v
					self.commenthref.append(v)
					self.bComment = False
					self.commentcount += 1
				if self.bPreview == True and v.find('trailer') != -1:
					if v.find('http://www.mtime.com') == -1:
						self.previewhref = 'http://www.mtime.com' + v
					self.bPreview = False
				else:
					self.bPreview = False

	def end_a(self):
		if self.bName == True:
			self.bName = False

	def start_img(self, attrs):
		for (k, v) in attrs:
			if k == 'src':
				if v.find(self.href.split('/')[-2] + '.jpg') != -1:
					self.posterhref = v

	def start_body(self, attrs):
		pass

	def end_body(self):
		self.formatinfo()

	def handle_data(self, text):
		if text == '导演：':
			self.bDirector = True
		elif text == '更多':
			self.bActor = False
		elif text == '主演：':
			self.bDirector = False
			self.bActor = True
		elif text == '类型：':
			self.bArea = False
			if self.bRCate == False:
				self.bCategory = True
		elif text == '上映日期：':
			self.bCategory = False
			self.bRCate = True
		elif text == '国家/地区：':
			self.bArea = True
		elif text == '回复数：':
			self.bTCom = True
		elif self.bTCom == True:
			self.bTCom = False
			self.bComment = True
		elif text == '预告片':
			self.bPreview = True
		elif text == '更多剧情':
			if self.tmphref.find('http://www.mtime.com'):
				self.tmphref = 'http://www.mtime.com' + self.tmphref
			self.descriptionhref = self.tmphref
		else:
			if self.bName == True:
				self.name += text + ' '
			elif self.bDirector == True:
				self.director += text
			elif self.bActor == True:
				self.actor += text + ' '
			elif self.bCategory == True:
				self.category += text + ''
			elif self.bArea == True:
				self.area += text
			else:
				pass

	def formatinfo(self):
		self.director = self.director.replace('\t', '')
		self.actor = self.actor.replace('\t', '')
		self.category = self.category.replace('\t', '')
		self.area = self.area.replace('\t', '')

	def GetMovieName(self):
		return self.name

	def GetMovieID(self):
		return self.movieid

	def GetMovieDirector(self):
		return self.director
	
	def GetMovieActor(self):
		return self.actor

	def GetMovieArea(self):
		return self.area
	
	def GetMovieCategory(self):
		return self.category

	def GetMoviePosterhref(self):
		return self.posterhref

	def GetMovieCommenthref(self):
		return self.commenthref

	def GetMovieDescriptionhref(self):
		return self.descriptionhref

	def GetMoviePreviewhref(self):
		return self.previewhref

class MovieDescription(SGMLParser):
	def __init__(self):
		SGMLParser.__init__(self)
		self.description = ''
		self.bDescription = False
		self.bTime = False

	def start_body(self, attrs):
		pass

	def end_body(self):
		self.formatdescrib()

	def handle_data(self, text):
		if text == '剧情':
			self.bDescription = True
		elif text == '我来添加剧情':
			self.bDescription = False
		elif text.find('发布者：') != -1:
			self.bTime = True
		elif text.find('作者：') != -1:
			self.bTime = True
		elif self.bTime == True:
			self.bTime = False
		else:
			if self.bDescription == True:
				self.description += text + '\n'

	def formatdescrib(self):
		self.description = self.description.replace('\t', '')
		item = self.description.split('\n')
		for i in range(item.count('')):
			item.remove('')
		self.description = '\n'.join(item)

	def GetDescription(self):
		return self.description

class MovieComment(SGMLParser):
	def __init__(self, movieid):
		SGMLParser.__init__(self)
		self.movieid = movieid
		self.commenttitle = ''
		self.comment = ''
		self.userid = ''
		self.time = ''
		self.bTitle = False
		self.bUserid = False
		self.bTime = False
		self.bTTime = False
		self.bComment = False

	def start_span(self, attrs):
		for (k, v) in attrs:
			if k == 'class' and v == 'editblog':
				self.bTitle = True
			if k == 'class' and v == 'time':
				self.bTTime = True

	def start_div(self, attrs):
		for (k, v) in attrs:
			if k == 'class' and v == 'clear':
				self.bComment = False
	
	def handle_data(self, text):
		if text == '回复':
			pass
		else:
			text = text.replace('\r', '')
			text = text.replace('\n', '')
			if self.bTitle == True and text:
				self.commenttitle = text
				self.bTitle = False
				self.bUserid = True
			elif self.bUserid == True and text:
				self.userid = text
				self.bUserid = False
				self.bTime = True
			elif self.bTime == True and self.bTTime == True:
				self.time = text
				self.bTime = False
				self.bComment = True
			elif self.bComment == True and text:
				self.comment += '\t' + text + '\n'
			else:
				pass

	def GetMovieCMovieID(self):
		return self.movieid

	def GetMovieCUserid(self):
		return self.userid

	def GetMovieCCommenttitle(self):
		return self.commenttitle

	def GetMovieCTime(self):
		return self.time

	def GetMovieComment(self):
		return self.comment

class MovieExtraInfo(SGMLParser):
	def __init__(self, moviename):
		SGMLParser.__init__(self)
		self.moviename = moviename
		self.moviehref = ''
		self.timetable = {}
		self.timeprice = {}
		self.cinema = {}
		self.place = ''
		self.date = ''
		self.time = ''
		self.timehref = []
		self.tmphref = ''
		self.bData = False
		self.bPlace = False
		self.bTime = False
		self.bTimeref = False

	def start_a(self, attrs):
		for (k, v) in attrs:
			if k == 'href':
				self.tmphref = v
				if self.bTimeref == True:
					if v.find('http://www.mtime.com') == -1:
						v = 'http://www.mtime.com' + v
					self.timehref.append(v)

	def start_dt(self, attrs):
		if self.bData == True:
			self.bPlace = True
	
	def end_dt(self):
		if self.bData == True:
			self.cinema[self.place] = self.tmphref
			self.bPlace = False
			self.bTime = True
			self.bTimeref = True

	def start_dl(self, attrs):
		pass

	def end_dl(self):
		if self.bData == True:
			self.timetable[self.place] = self.time
			self.timeprice[self.place] = self.timehref
			self.timehref = []
			self.time = ''
			self.bTimeref = False

	def start_script(self, attrs):
		if self.bData == True:
			self.bData = False

	def handle_data(self, text):
		if text == self.moviename:
			if self.tmphref.find('http://www.mtime.com') == -1:
				self.tmphref = 'http://www.mtime.com' + self.tmphref
			self.moviehref = self.tmphref
		elif text == '返回顶部':
			self.bData = True
		elif text.find('今天，') != -1:
			self.date = text.split('，')[-1]
		else:
			if self.bPlace == True:
				self.place = text
			elif self.bTime == True:
				text = text.replace('\t', '')
				text = text.replace('\n', '')
				text = text.replace('\r', '')
				self.time += text
			else:
				pass

	def GetMovieHref(self):
		return self.moviehref

	def GetMovieTimetable(self):
		return self.timetable

	def GetMovieCinemahref(self):
		return self.cinema

	def GetMovieDate(self):
		return self.date

	def GetMoviePricehref(self):
		return self.timeprice

if __name__ == "__main__":
	#timelink = 'http://www.mtime.com/showtime/45897/China_Hubei_Province_Wuhan/'
	#timereq = urllib2.Request(timelink)
	#timedata = urllib2.urlopen(timereq)
	#time = MovieExtraInfo('梅兰芳')
	#time.feed(timedata.read())
	#print time.GetMovieHref()
	#for (k, v ) in time.GetMovieCinemahref().items():
	#	print k, v
	#print time.GetMovieDate()

	#link = 'http://www.mtime.com/movie/45897/'
	#req = urllib2.Request(link)
	#data = urllib2.urlopen(req)
	#info = MtimeMovie('/movie/45897/')
	#info.feed(data.read())
	#print info.GetMovieName()
	#print info.GetMovieDirector()
	#print info.GetMovieActor()
	#print info.GetMovieArea()
	#print info.GetMovieCategory()
	#print info.GetMoviePosterhref()
	#print info.GetMovieCommenthref()
	#print info.GetMovieDescriptionhref()
	#print info.GetMoviePreviewhref()

	#deslink = 'http://www.mtime.com/movie/45897/plots.html'
	#dreq = urllib2.Request(deslink)
	#ddata = urllib2.urlopen(dreq)
	#des = MovieDescription()
	#des.feed(ddata.read())
	#print des.GetDescription()
	
	from MySQLdb import *
	connection = Connection('202.114.215.73', 'movie', 'eivom123', 'movie')
	cursor = connection.cursor()
	comlink = 'http://www.mtime.com/my/someone/blog/419540'
	comreq = urllib2.Request(comlink)
	comdata = urllib2.urlopen(comreq)
	com = MovieComment('12345')
	com.feed(comdata.read())
	sql = "insert into comment(movie, user, comment, hwhen, title) values (%s, %s, %s, %s, %s)"
	para = []
	para.append(com.GetMovieCMovieID())
	para.append(com.GetMovieCUserid())
	para.append(com.GetMovieComment())
	para.append(com.GetMovieCTime())
	para.append(com.GetMovieCCommenttitle())
	cursor.execute(sql, para)
	cursor.close()
	connection.close()
