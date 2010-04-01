import os,MySQLdb;

if __name__=='__main__':
	conn=MySQLdb.Connection('202.114.215.73','movie','eivom123','movie');
	cur=conn.cursor();
	cur.execute('select id,environment from cinema');
	row=cur.fetchall()
	for c in row:
		os.popen('wget ' + c[1]);
	cur.execute('update cinema set environment=concat("../image/cinema/",reverse(substring_index(reverse(environment),'/',1)))')
	


