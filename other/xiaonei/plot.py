import Image
import MySQLdb
import math
import os
	
def export2File(path,step):
	if os.path.exists(path):
		os.remove(path)
	conn=MySQLdb.connect(host="localhost",user="root",passwd="851017", db="xiaonei",charset="utf8",use_unicode=False)
	cur=conn.cursor()
	cur.execute('select friend, user into outfile "'+path+'" from log where friend % '+str(step)+' = 0')
def getFromDB(step):
	conn=MySQLdb.connect(host="localhost",user="xiaoneiu",passwd="xiaoneip", db="xiaonei",charset="utf8",use_unicode=False)
	cur=conn.cursor()
	cur.execute('select friend, user from log where friend % '+str(step)+' = 0')
	recs=cur.fetchall()
	xs,ys=[],[]
	for rec in recs:
		xs.append(rec[0])
		ys.append(rec[1])
	return xs,ys
def readFile(path):
	recs=open(path).read().split('\n')
	xs=[]
	ys=[]
	for rec in recs:
		xy=rec.split('\t')
		if len(xy)==2:
			xs.append(int(xy[0]))
			ys.append(int(xy[1]))
	return xs,ys
def plot(xys, size):
	im=Image.new('RGB', (size+1, size+1))
	xmax,ymax=0,0
	for xs,ys in xys:
		xmax=max( xmax, max(xs) )
		ymax=max( ymax, max(ys) )
	print 'XMax:%d, YMax:%d' % (xmax,ymax)
	for i in range(size):
		for j in range(size):
			im.putpixel((i,j), (256,256,256))
	for xs,ys in xys:
		for i in range(len(xs)):
			x=xs[i]*size/xmax
			y=ys[i]*size/ymax
			im.putpixel((x,size-y), (0,0,0))
	im.save('hjz.jpg')
	im.show()

def mydraw(step):
	xs,ys=getFromDB(step)	#read from database
	xmax=max(xs)
	xmin=min(xs)
	ymax=max(ys)
	dist=ymax-xmax
	y3=range(xmin, xmax+1, step)
	for i in range(len(y3)):
		y3[i]=y3[i]+dist
	plot([(xs,ys), (xs,xs), (xs, y3)], imSize)	#y=f(x) VS y=x VS y=x+step

path = r"E:\\code\\python\\xiaonei\\hankjin.log"
imSize = 800				#image size
step=10000

export2File(path,step)

#1. create the dataset
xs,ys=getFromDB(step)	#read from database
#xs,ys=readFile(path)		#read from file

#2. draw
#plot([(xs,ys)], imSize)			#y=f(x)
#plot([(xs,ys),(xs,xs)], imSize)	#y=f(x) VS y=x
#mydraw(step)					#y=f(x) VS y=x VS y=x+k

line=range(1, 45000000, step)	#y=x function 
plot([(xs, ys), (line,line)], imSize)