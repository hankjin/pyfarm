#!/usr/bin/env python
#coding: utf-8
'''
create table user(
    id int not null primary key,
    name varchar(80),
    info varchar(200),
    pic varchar(200),
    scaned char(1) default 'N'
) engine=myisam, default charset=utf8;
create table friend(
    uid int not null primary key,
    friends varchar(20480) not null
) engine=myisam, default charset=utf8;
create table log(
    friend int,
    user int
) engine myisam, default charset=utf8;
store notScaned friends in memory
batch add friend to database
dict to save friends
notScaned save no more than 500, if empty get from db
'''

from xiaonei import *
from FriendParser import *
import MySQLdb,md5,re,os,time
import time,sys
import thread

class XNDB(XiaoNei):
    def __init__(self):
        XiaoNei.__init__(self)

	self.newUser={}
	self.todo=[]
	self.todoN=0
	self.todoMax=100000
	self.scanN=0
	self.threadN=0
	self.lock=thread.allocate_lock()
	self.stop=False
	self.flog = None
    def listFriends(self,fid):
        curpage=0
        friends=[]
	parser=FriendParser()
        while True:
            url='http://friend.renren.com/GetFriendList.do?curpage=%s&id=%s' % (curpage,fid)
            raw=self.req(url)
            parser.reset()
            parser.feed(raw)
            friends.extend(parser.friends)
            if len(re.findall('下一页',raw))==0:
                break
            else:
                curpage+=1
        return friends

    def log(self, info):
	if self.flog == None:
		self.flog = open('forkxn.err','a')
	self.flog.write('%s: %s\n' % (time.ctime(), info))
    def opendb(self):
        self.conn=MySQLdb.connect(host="localhost",user="xiaoneiu",passwd="xiaoneip", db="xiaonei",charset="utf8")
        self.cursor=self.conn.cursor()
        #read all  and not scaned users
	self.cursor.execute('select count(*) from friend')
	self.friendN=self.cursor.fetchone()[0]
	self.cursor.execute('select count(*) from user')
	self.scanN=self.cursor.fetchone()[0]
	self.getTodos()
    def getTodos(self):
	self.cursor.execute("select id from user where scaned='N' limit 0,%s" % self.todoMax)
        alldata=self.cursor.fetchall()
        if alldata:
            for rec in alldata:
	        self.todo.append(rec[0])
		self.todoN+=1

    def closedb(self):
        self.conn.close()

    def savefriends(self,uid,strfriend, friend):
	sql = "update user set scaned='Y' where id=%s"
	try:
	    self.cursor.execute(sql,uid)
	except:
	    self.log('[Err]:\t Set of %s failed' % uid)
	    return
	sql = "insert into friend(uid,friends,friendN) values (%s,%s,%s)"
	try:
	    self.cursor.execute(sql,[uid, strfriend, len(friend)])
	except Exception, e:
	    self.log("[Err]:\t set friend ship of %s failed" % uid)
	self.friendN+=1
	if self.friendN%100==0:
	    sql="insert into log(friend,user) values (%s,%s)"
	    self.cursor.execute(sql, [self.friendN, self.scanN])


    def saveuser(self,user):
        sql = "insert into user (id,name,info,pic) values(%s,%s,%s,%s)"
        try:
            n = self.cursor.execute(sql,[user['id'],user['name'],
		user['info'],user['pic']]) #save 2 db
        except Exception, e:
            print e

    def xsaveuser(self,users):
	sql = "insert into user (id,name,info,pic) values(%s,%s,%s,%s)"
	try:
	    self.cursor.executemany(sql,users)
	except Exception, e:
	    print e
	    for user in users:
		self.saveuser(user)

    def newProc(self):
        self.threadN+=1
        thread.start_new_thread(db.scanUser, (self.threadN,))
    def mainLoop(self):
	while not self.stop:
	    while len(self.newUser)==0:
		try:
		    print '[M]:\t ===%d====no newUser, sleep a while===' % len(self.todo)
		    if len(self.todo)<100:
			self.lock.acquire()
			self.getTodos()
			if self.threadN < 10:
				for i in range(50):
					self.newProc()
			self.lock.release()
		    time.sleep(5)
	    	    if os.path.exists('add.hjz'):#add thread dynamically
			os.remove('add.hjz')
			for i in range(5):
			    self.newProc()
		    if os.path.exists('quit.hjz'):#stop process
		        print '[M]:\t Exiting...'
			os.remove('quit.hjz')
			self.stop=True
			time.sleep(self.threadN+10)
			self.lock.acquire()
			for u in self.newUser:
			    self.addUser(u, self.newUser[u])
			self.lock.release()
			break
		except:
		    print 'main loop wake up'
	    self.lock.acquire()
	    uid,friends=self.newUser.popitem()
	    self.lock.release()
	    self.addUser(uid,friends)


    def scanUser(self, threadId):
	while not self.stop:
	    #get a pid
	    self.lock.acquire()
	    if len(self.todo)==0: #no todo left, exit the thread
		self.threadN-=1
	        self.lock.release()
		log('[Err]:\t A thread %s exit because of empty todo' % threadId)
		return
	    uid=self.todo.pop()
	    self.todoN-=1
	    self.lock.release()
	    #fetch data from web
	    print '[%d]:\t Scanning User:%s' % (threadId, uid)
	    friends = self.listFriends(uid) #scan
	    print '[%d]:\t Scan %s finished, got %d' % (threadId, uid, len(friends))
	    #put into dict
	    self.lock.acquire()
	    self.newUser[uid]=friends
	    self.lock.release()

    def addUser(self, uid, friends):
	#get friends from memcache
	print '[M]:\t Add user...%s' % uid
	fs2db=set() #friends
	user2db=[] #new users
	for friend in friends: #each of his friends
	    fs2db.add(str(friend['id']))
        strfs2db=",".join(fs2db)
        if len(fs2db)>0:
	    self.cursor.execute('select id from user where id in (%s)' % strfs2db)
	    scaned=self.cursor.fetchall()
	for friend in friends:	
	    fid=friend['id']
	    if not (fid,) in scaned:
		user2db.append((friend['id'],friend['name'],friend['info'],friend['pic']))
		if self.todoN<self.todoMax:
		    self.todo.append(fid)
		    self.todoN+=1
        self.scanN+=len(user2db)
	self.savefriends(uid, strfs2db, fs2db) #save relationship
	self.xsaveuser(user2db)      #save users

if __name__=='__main__':
    db = XNDB()
    print 'Login...'
    db.login('hackjin@yeah.net','jinhank')
    print 'Running...'
    db.opendb()
    for i in range(50):
        db.newProc()	
    db.mainLoop()
    db.closedb()
    '''
    fs=db.listFriends(db.uid)[0]
    print fs
    db.save2db(fs)
    '''
