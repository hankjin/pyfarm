import os,MySQLdb;

if __name__=='__main__':
	conn=MySQLdb.Connection('202.114.215.73','movie','eivom123','movie');
	cur=conn.cursor();
	cur.execute('select poster from movie');
	row=cur.fetchall()
	for c in row:
		os.popen('wget ' + c[0]);
	cur.execute('update movie set poster=concat("../image/movie/",reverse(substring_index(reverse(poster),'/',1)))')
	


