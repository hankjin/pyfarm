#!/usr/bin/python
import MySQLdb;

if __name__=='__main__':
	conn=MySQLdb.Connection('202.114.215.73','movie','eivom123','movie');
	cur=conn.cursor();
	cur.execute('select * from reminder where hwhen between % and %',[3,4]);
	
