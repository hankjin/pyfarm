import os,MySQLdb;
#final step
#download image and update url
#update database

def getcinemaImage():
	cur.execute('select id,environment from cinema');
	row=cur.fetchall()
	for c in row:
		os.popen('wget ' + c[1]);
	cur.execute('update cinema set environment=concat("../image/cinema/",reverse(substring_index(reverse(environment),'/',1)))')


if __name__=='__main__':
	localsite='/home/cbibadmin/tomcat6/moviesite/movie/image/'
	os.popen('rm '+localsite+"movie/*.1");
	conn=MySQLdb.Connection('202.114.215.73','movie','eivom123','movie');
	cur=conn.cursor();
	cur.execute('select poster from movie');
	row=cur.fetchall()
	for c in row:
		os.popen('wget -P '+localsite+"movie " + c[0]);
	cur.execute('update movie set poster=concat("../image/movie/",reverse(substring_index(reverse(poster),"/",1)))')
	cur.execute('update movieinfo set cinema=(select id from cinema where cinema.name=movieinfo.cinemaname)');

