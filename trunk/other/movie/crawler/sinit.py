import os,MySQLdb;
# Step1. clear old tables
# Next: sgetinfo.py
#

if __name__=='__main__':
	os.popen('mysqldump -umovie -peivom123 movie>/home/movie/db/db$(date +%Y-%m-%d-%H).sql')
	conn=MySQLdb.Connection('202.114.215.73','movie','eivom123','movie');
	cur=conn.cursor();
	cur.execute('truncate movie');
	cur.execute('truncate movieinfo');
	cur.execute('truncate comment');


