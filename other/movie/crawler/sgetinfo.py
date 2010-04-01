#This Python File is using following encoding: utf-8
#get info from mtime and put into database
# prev Step sinit.py
# next Step sfinal.py
import urllib, urllib2
from MySQLdb import *
from movie import *
from movielist import *
from movietime import *
from moviemtime import *

def GetMovieMTodayhref():
	import time
	x = time.localtime()
	str = "%i"%x.tm_year + "%i"%(x.tm_mon / 10) + "%i"%(x.tm_mon % 10) + "%i"%(x.tm_mday / 10) + "%i"%(x.tm_mday % 10)
	str = str[2:]
	href = 'http://www.mtime.com/showtime/China_Hubei_Province_Wuhan/list/' + str + '.html'
	return href

try:
	connection = Connection("202.114.215.73", "movie", "eivom123", "movie")
	cursor = connection.cursor()
	date = ''
	movielink = 'http://www.mtime.com/showtime/'
	req = urllib2.Request(movielink)
	urldata = urllib2.urlopen(req)
	mlist = MovieList()
	commentdic = {}
	mlist.feed(urldata.read())
	for (name, href) in mlist.GetMovieList().items():
		countmovie = 0
		while True:
			try:
				if countmovie >= 2:
					break
				extrareq = urllib2.Request(href)
				extradata = urllib2.urlopen(extrareq)
				extrad = MovieExtraInfo(name)
				extrad.feed(extradata.read())
				date = extrad.GetMovieDate()
				moviereq = urllib2.Request(extrad.GetMovieHref())
				moviedata = urllib2.urlopen(moviereq)
				movie = MtimeMovie(extrad.GetMovieHref())
				movie.feed(moviedata.read())
				sql = 'select count(id) from movie where id = %s'
				cursor.execute(sql, movie.GetMovieID())
				num = cursor.fetchone()[0]
				if num != 0:
					break
				desreq = urllib2.Request(movie.GetMovieDescriptionhref())
				desdata = urllib2.urlopen(desreq)
				moviedes = MovieDescription()
				moviedes.feed(desdata.read())
				commentlist = movie.GetMovieCommenthref()
				movieid = movie.GetMovieID()
				sqlcommand = 'insert into movie(id, name, director, actor, category, description, poster, preview, zone) values (%s, %s, %s, %s, %s, %s, %s, %s, %s)'
				sqlpara = []
				sqlpara.append(movie.GetMovieID())
				sqlpara.append(name)
				sqlpara.append(movie.GetMovieDirector())
				sqlpara.append(movie.GetMovieActor())
				sqlpara.append(movie.GetMovieCategory())
				sqlpara.append(moviedes.GetDescription())
				sqlpara.append(movie.GetMoviePosterhref())
				sqlpara.append(movie.GetMoviePreviewhref())
				sqlpara.append(movie.GetMovieArea())
				cursor.execute(sqlcommand, sqlpara)
				commentdic[movieid] = commentlist
				print name
				break
			except:
				countmovie += 1
				print "infomation error occurs"
	for (k, v) in commentdic.items():
		for comhref in v:
			countcom = 0
			while True:
				try:
					if countcom >= 2:
						break
					commentreq = urllib2.Request(comhref)
					commentdata = urllib2.urlopen(commentreq)
					comment = MovieComment(k)
					comment.feed(commentdata.read())
					sqlcom = "insert into comment(movie, user, comment, hwhen, title) values (%s, %s, %s, %s, %s)"
					sqlp = []
					sqlp.append(comment.GetMovieCMovieID())
					sqlp.append(comment.GetMovieCUserid())
					sqlp.append(comment.GetMovieComment())
					sqlp.append(comment.GetMovieCTime())
					sqlp.append(comment.GetMovieCCommenttitle())	
					cursor.execute(sqlcom, sqlp)
					print "finish one comment"
					break
				except:
					countcom += 1
					print "comment error accurs"
	sqldel = 'delete from movieinfo where htime is null'
	cursor.execute(sqldel)
	print 'delete all tomorrow&aftomorrow movieinfo'
	timelink = 'http://www.google.cn/movies?hl=zh-CN&sort=1&near=武汉市'
	while timelink:
		counttime = 0
		while True:
			try:
				if counttime >= 2:
					break
				timereq = urllib2.Request(timelink)
				timereq.add_header('User-agent', 'Mozilla/5.0')
				timedata = urllib2.urlopen(timereq)
				timetable = MovieTime()
				timetable.feed(timedata.read())
				sqltime = "insert into movieinfo(hdate, htime, cinema, movie, cinemaname) values(%s, %s, null, %s, %s)"
				sqlcinema = "select id from cinema where name = %s"
				sqlmovie = "select id from movie where name = %s"
				movieid = 0
				cinemaid = 0
				for (name, tandw) in timetable.GetMovieTimetable().items():
					for (cinema, time) in tandw.items():
						if cursor.execute(sqlmovie, name) != 0:
							movieid = cursor.fetchone()[0]
						else:
							break
						#if cursor.execute(sqlcinema, cinema) != 0:
						#	cinemaid = cursor.fetchone()[0]
						#else:
							#break
						para = []
						para.append(date)
						para.append(time)
						#para.append(cinemaid)
						para.append(movieid)
						para.append(cinema)
						cursor.execute(sqltime, para)
				timelink = timetable.GetMovieNextTtable()
				if timelink and timelink.find('http://www.google.cn') == -1:
					timelink = 'http://www.google.cn' + timelink
				print "finish time table"
				break
			except:
				counttime += 1
				print "time page error accurs"
	count = 0
	tomorrowhref = ''
	aftomorrowhref = ''
	while True:
		try:
			if count >= 2:
				break
			todayhref = GetMovieMTodayhref()
			req = urllib2.Request(todayhref)
			data = urllib2.urlopen(req)
			todaymovie = MovieMtimeTime()
			todaymovie.feed(data.read())
			tomorrowhref = todaymovie.GetMovieTomorrowhref()
			aftomorrowhref = todaymovie.GetMovieAftomorrowhref()
			break
		except:
			count += 1
	counttime = 0
	while True:
		try:
			if tomorrowhref == '':
				break
			if counttime >= 2:
				break
			req = urllib2.Request(tomorrowhref)
			data = urllib2.urlopen(req)
			tomorrowmovie = MovieMtimeTime()
			tomorrowmovie.feed(data.read())
			date = tomorrowmovie.GetMovieDate()
			for (cinema, movie) in tomorrowmovie.GetMovieTable().items():
				sqltime = "insert into movieinfo(hdate, cinema, movie, cinemaname) values(%s, null, %s, %s)"
				sqlmovie = "select id from movie where name = %s"
				movielist = movie.split('|')
				for moviename in movielist:
					movieid = 0
					moviename = moviename[2:]
					if cursor.execute(sqlmovie, moviename) != 0:
						movieid = cursor.fetchone()[0]
					else:
						break
					para = []
					para.append(date)
					para.append(movieid)
					para.append(cinema)
					cursor.execute(sqltime, para)
			print "finish tomorrow page"
			break
		except:
			counttime += 1
			print "tomorrow page error accus"
	counttime = 0
	while True:
		try:
			if aftomorrowhref == '':
				break
			if counttime >= 2:
				break
			req = urllib2.Request(aftomorrowhref)
			data = urllib2.urlopen(req)
			aftomorrowmovie = MovieMtimeTime()
			aftomorrowmovie.feed(data.read())
			date = aftomorrowmovie.GetMovieDate()
			for (cinema, movie) in aftomorrowmovie.GetMovieTable().items():
				sqltime = 'insert into movieinfo(hdate, cinema, movie, cinemaname) values(%s, null, %s, %s)'
				sqlmovie = 'select id from movie where name = %s'
				movielist = movie.split('|')
				for moviename in movielist:
					movieid = 0
					moviename = moviename[2:]
					if cursor.execute(sqlmovie, moviename) != 0:
						movieid = cursor.fetchone()[0]
					else:
						break
					para = []
					para.append(date)
					para.append(movieid)
					para.append(cinema)
					cursor.execute(sqltime, para)
			print "finish aftomorrow page"
			break
		except:
			counttime += 1
			print "aftomorrow page error accurs"
	cursor.close()
	connection.close()
except:
	print "数据库连接错误"
