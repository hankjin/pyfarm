#encoding: utf8
import urllib,re
import sqlite3
from sgmllib import SGMLParser

def parseBlog(id,raw):
    blog=[id]
    temp=re.findall('<h3 class="title pre fs1">(.*)</h3>',raw)
    if len(temp)==1:
	    blog.append(temp[0].decode('gbk'))
    else:
	    print 'Error in blog title'
	    print temp
    temp=re.findall('<span class="pleft">[ ]*<a[^>]*>([^<]*)</a>', raw)
    if len(temp)==1:
	    blog.append(temp[0].decode('gbk'))
    else:
	    print 'Error in blog catagory'
	    print temp
    temp=re.findall('<span class="pleft">[ ]*<a[^>]*>[^<]*</a>[ ]*<span[^>]*>([^<]*)</span>', raw)
    if len(temp)==1:
	    blog.append(temp[0].decode('gbk'))
    else:
	    print 'Error in blog catagory'
	    print temp
    temp=re.findall('nbw-blog[^"]*">(.*)[ ]*<div class="nbw-act',raw)
    if len(temp)==1:
	    blog.append(temp[0].decode('gbk'))
    else:
	    print 'Error in blog content'
	    print temp
    return blog
    
def getList(start,size):
    url='http://api.blog.163.com/hankjin/dwr/call/plaincall/BlogBeanNew.getBlogs.dwr'
    param={'callCount':1,
	    'scriptSessionId':'${scriptSessionId}187',
	    'c0-scriptName':'BlogBeanNew',
	    'c0-methodName':'getBlogs',
	    'c0-id':0,
	    'c0-param0':'number:33731937',
	    'c0-param1':'number:%d' % start,
	    'c0-param2':'number:%d' % size,
	    'batchId':'578343'
	    }
    raw=urllib.urlopen(url,urllib.urlencode(param)).read()
    ids=re.findall('permaSerial="([^"]*)"',raw)
    titles=re.findall('title="([^"]*)"',raw)
    return (ids,titles)
def getBlog(id):
    url='http://hankjin.blog.163.com/blog/static/%s/'
    try:
	raw=urllib.urlopen(url % id).read()
    except:
	print 'time out'
	return getBlog(id)
    return raw
def saveBlog(c,blog):
    try:
	c.execute('insert into blog values(?,?,?,?,?)',blog)
    except:
	print 'insert into database failed'
def opendb():
    conn=sqlite3.connect('blog.dat')
    c=conn.cursor()
    try:
	c.execute('select * from blog limit 1')
    except:
	print 'no db foudn, new db created'
	c.execute('create table blog(id int, title text, catagory text, time text,content text)')
    return (conn,c)
if __name__=='__main__':
    (conn,c)=opendb()
    urllib.socket.setdefaulttimeout(10)

    start=0
    size=10
    (ids,titles)=getList(start,size)
    for i in range(size):
	raw=getBlog(ids[i*2])
	blog=parseBlog(start+i,raw)
	saveBlog(c,blog)
	print 'Blog%d: %s saved' % (blog[0],blog[1])
    conn.commit()
