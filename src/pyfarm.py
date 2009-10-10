#!/usr/bin/env python
#encoding: utf-8
#2009-8-21 升级后
import urllib,urllib2,cookielib
import time,zlib,re,md5
import cStringIO,gzip
import json
import pickle
import thread,sys,os
class Farm:
		debug=False
		status={} #self's farm status
		friends={} #list of friends
		shop={} #information of seeds
		bag={} #背包
		uid=0 #user id
		autoweed=True #自动除草
		autonorm=True #自动杀虫
		autowater=True #自动浇水
		autofert=True #自动施肥
		autoplant=True #自动种植
		autosteal=True #自动收获
		#创建Cookie
		def __init__(self):
				cj=cookielib.LWPCookieJar()
				opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
				opener.addheaders=[('User-agent','Mozilla')]
				urllib2.install_opener(opener)
		# 请求网页的工具函数，直接返回网页内容
		def req(self,url,body=()):
				while True:
						try:
								if len(body)==0:
										req=urllib2.Request(url)
								else:
										req=urllib2.Request(url,urllib.urlencode(body))
								raw=urllib2.urlopen(req).read()
								if len(raw) > 0:
										break
								else:
										print 'Req Empty: %s %s' % (url,body)
						except:
								print 'ReqError url:%s' % url
								pass
			
				if self.debug: #输出调试信息
						print url
						print body
				return raw

		def param(self): # build params farmKey and farmTime
				farmTime=str(time.time())[0:10]
				farmKey=md5.md5(farmTime+'15l3h4kh').hexdigest()
				return (farmKey, farmTime)
		def buySeed(self,n):
				for i in range(n):
						print i
						url="http://xn.hf.fminutes.com/api.php?mod=shop&act=buy&farmKey=%s&farmTime=%s&inuId=" % self.param()
						body={"type":1,"id":2,"number":99}
						self.req(url,body)
		def buyFert(self,n):
				for i in range(n):
						print i
						url="http://xn.hf.fminutes.com/api.php?mod=shop&act=buy&farmKey=%s&farmTime=%s&inuId=" % self.param()
						body={"type":3,"number":100,"id":1}
						self.req(url,body)
		# 登录校内网 获取校内网到开心网的链接地址,并取得Cookie
		def login(self, email, password):
				url="http://passport.renren.com/PLogin.do"
				body=(("email",email),("password",password),("autoLogin","true"),
						("domain","renren.com"),("origURL","http://www.renren.com/SysHome.do"))
				ret=self.req(url,body)
				uids=re.findall('http\:\/\/renren\.com\/profile\.do\?id\=(\d+)',ret)
				if len(uids)>0:
						self.uid=int(uids[0])
				else:
						print ret
				# build cookie to faminutes
				url='http://apps.renren.com/happyfarm?origin=104'
				raw = self.req(url)
				urls = re.findall('iframe_canvas" src="([^"]+)"', raw)
				if len(urls)==0:
					return raw
				url=re.findall('iframe_canvas" src="([^"]+)"',self.req(url))[0]
				url=url.replace('?','/?').replace('&amp;','&') # fuck url, it lacks a '/'
				raw=self.req(url)
				# read current status
				url='http://xn.hf.fminutes.com/api.php?mod=user&act=run&farmKey=%s&farmTime=%s&inuId=' % self.param()
				self.status=json.read(self.req(url))
		# 添加某个好友
		def addFriend(self,fid):
				url='http://friend.renren.com/ajax_request_friend.do'
				body={'from':'undefined','id':fid,'why':''}
				return json.read(self.req(url,body)) #添加

		# 删除无农场的好友
		def clearFriends(self):
				self.listFriends()
				reserved = []
				for x in self.friends:
						reserved.append(int(x['userId']))
				print 'reserved:%d' % len(reserved)
				url = 'http://friend.renren.com/myfriendlistx.do'
				renrens = json.read(re.findall(' friends=(.*);',self.req(url))[0])
				for x in renrens:
						if not x['id'] in reserved:
								url = 'http://icode.renren.com/getcode.do?t=delfriend_&temp=%d' % int(time.time()*1000)
								raw=self.req(url)
								open('test.bmp','w').write(raw) #用于人工观察识别
								open('test2.bmp','w').write(raw) #用于保留记录，分析识别方法
								verify = str.rstrip(sys.stdin.readline())
								url = 'http://friend.renren.com/DelFriend.do'
								body = {'id':x['id'],'code':verify}
								raw=json.read(self.req(url,body))
								print x['id'],raw
								if raw['ret']==0:
										os.rename('test2.bmp',verify+".bmp")

		# 添加新好友
		def addFriends(self):
				if os.path.exists('main.pic'):
						added=pickle.load(open('main.pic'))
				else:
						added=[]
				url='http://friend.renren.com/myfriendlistx.do'
				friends=json.read(re.findall(' friends=(.+)\;',self.req(url))[0])
				scanedN=0
				addedN=0
				for friend in friends:#自己的每个好友
						curpage=0
						while True:
								url='http://friend.renren.com/GetFriendList.do?curpage=%s&id=%s' % (curpage,friend['id'])
								raw=self.req(url)
								ffs=re.findall('\(\'addFriend(\d+)',raw)
								for ff in ffs:#好友的每个好友
										scanedN+=1
										if ff in added:
												print '%s Added' % ff
												continue
										addedN+=1
										ret=self.addFriend(ff)
										added.append(ff)
										print ff,ret['message'].encode('utf-8')
										if ret['message'].startswith(u'短时间内不要加太多好友'):
												pickle.dump(added,open('main.pic','w'))
												return
										time.sleep(5)
								if len(re.findall('下一页',raw))==0:
										print 'One Friend OK, scaned:%d\t Added:%d' % (scanedN,addedN)
										pickle.dump(added,open('main.pic','w'))
										break
								else:
										curpage+=1
										print 'Page:%d' % curpage
		# 获取所有好友
		def listFriends(self):
				url="http://xn.hf.fminutes.com/api.php?mod=friend&farmKey=%s&farmTime=%s&inuId=" % self.param()
				self.friends=json.read(self.req(url))['data']
		# 获取某好友信息
		def showFriend(self,fid):
				url='http://xn.hf.fminutes.com/api.php?mod=user&act=run&flag=1&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid}
				raw=self.req(url,body)
				return json.read(raw)
		# 浇水 fid: 用户ID place:土地编号
		def water(self,fid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=water&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid,'place':place}
				return json.read(self.req(url,body))
		# 除草 fid:用户ID place:土地编号
		def clearWeed(self,fid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=clearWeed&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid,'place':place}
				return json.read(self.req(url,body))
		# 杀虫 fid:用户ID place: 土地编号
		def spraying(self,fid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=spraying&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid,'tId':0,'place':place}
				return json.read(self.req(url,body))
		# 获取商店列表
		def scanShop(self):
				url='http://xn.hf.fminutes.com/api.php?mod=shop&act=getShopInfo&type=1&farmKey=%s&farmTime=%s&inuId=' % self.param()
				print url
				self.shop=json.read(self.req(url))
				return self.shop
		# 查看自己购买的物品
		def getBag(self):
				url='http://xn.hf.fminutes.com/api.php?mod=Package&act=getPackageInfo&farmKey=%s&farmTime=%s&inuId=' % self.param()
				self.bag=json.read(self.req(url))
				return self.bag
		# 翻土
		def scarity(self,fid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=scarify&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid,'place':place}
				return json.read(self.req(url,body))

		# 种植 cid: 种子编号 uid: 用户编号 place: 土地编号
		def plant(self,cid,uid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=planting&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'cId':cid,'ownerId':uid,'place':place}
				return json.read(self.req(url,body))
		# 施肥 
		def fert(self,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=fertilize&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':self.uid,'tId':1,'place':place}
				return json.read(self.req(url,body))
		# 某好友的果实成熟时间列表
		def find(self,fid):
				self.scanShop()
				url='http://xn.hf.fminutes.com/api.php?mod=user&act=run&flag=1&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':fid}
				ret=json.read(self.req(url,body))
				lands=ret['farmlandStatus']
				result={}
				i=0
				for land in lands:
						seeds=[x for x in self.shop['1'] if x['cId']==land['a']]
						if len(seeds) == 1:
								durtime=int(seeds[0]['growthCycle'])+land['q']
								if durtime > time.time():#尚未成熟
										result[i]='%s:\t%s' % (seeds[0]['cName'],str(durtime));
								else:#已经成熟
										if 1==land['n']: #已经偷过
												pass
										else:#
												result[i]='%s:\tReady' % seeds[0]['cName']
						i+=1
				return result

		# 偷好友的果子 fid:好友编号 place:土地编号
		def steal(self,fid,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=scrounge&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={"ownerId":fid,"place":place}
				raw=self.req(url,body)
				return json.read(raw)
		def harvest(self,place):
				url='http://xn.hf.fminutes.com/api.php?mod=farmlandstatus&act=harvest&farmKey=%s&farmTime=%s&inuId=' % self.param()
				body={'ownerId':self.uid,'place':place}
				return json.read(self.req(url,body))
	
		# 查找果实列表
		def autofind(self):
				self.listFriends()
				result={}
				for f in farm.friends: #输出所有好友
						result[f['userId']]=farm.find(f['userId'])
				return result
		# 升级
		def moneyGo(self,n=1):
				for i in range(n):
						print i
						self.scarity(self.uid,0)
						self.plant(2,self.uid,0)
		def myfarm(self,interval=20*60):
				if len(self.shop)==0:
						self.scanShop()
				lands = self.showFriend(self.uid)
				if len(lands)==0:
						return
				else:
						lands = lands['farmlandStatus']
				i = -1
				for land in lands:
						i += 1
						s=[x for x in self.shop['1'] if x['cId']==land['a']]
						if len(s)==0:
								durtime = land['q']+10*60*60
						else:
								durtime = land['q']+int(s[0]['growthCycle'])
						leftT = durtime - time.time()
						if leftT > 3*60*60 or leftT < 1*60*60:
								if land['o']==1 and leftT < 9*60*60 and leftT > 8*60*60:
										pass
								elif land['o']==2 and leftT < 7*60*60 and leftT > 6*60*60:
										pass
								elif land['o']==3 and leftT < 5*60*60 and leftT > 3*60*60:
										pass
								else:
										temp=self.fert(i)
										print 'Fert %d' % i
										if temp.has_key('direction') and not temp['direction'] == '':
												pass
										else:
												leftT -= 60*60
								
						if leftT < 0 and not land['b']==7:
								land['b']=6
						if land['b']==6:
								ret=self.harvest(i)
								if ret.has_key('direction') and not ret['direction']=='':
										print 'harvest failed'
										print temp
										print leftT
										print ret
										print 'end harvest failed'
								else:
										land['b']=7
										print 'Harvest %d %s' % (i,ret)
						if land['b']==7:
								self.scarity(self.uid,i)
								self.plant(2,self.uid,i)
								self.fert(i)
								print 'Plant %d' % i
						if (durtime-int(time.time())) < interval:
								thread.start_new_thread(threadSteal,
												(self,durtime,-1,i))


		# 自动运行 默认最小间隔20分钟
		def autorun(self,interval=20*60):
				if len(self.friends)==0:
						self.listFriends()
				if len(self.shop)==0:
						self.scanShop()
				self.bag=self.getBag() #背包
				ret={} #下次运行时间
				
				for friend in self.friends:
						print (time.ctime()+friend['userName']).encode('utf-8')
						lands=self.showFriend(friend['userId'])
						if len(lands)==0:
								continue
						else:
								dogs = lands['dog']
								lands = lands['farmlandStatus']
						i=-1
						for land in lands:#每块地
								i+=1
								s=[x for x in self.shop['1'] if x['cId']==land['a']]
								if len(s)==0:#空地
										print u'\t%d\t空地'.encode('utf-8') % i
								else:#不是空地
										durtime=land['q']+int(s[0]['growthCycle'])
										if durtime < time.time(): #已经成熟
												if friend['userId']==self.uid: #自己的地，收获
														if land['b']==6: #成熟
																self.harvest(i)
																stealed=u'收获'
														elif land['b']==7: #已经收获
																stealed=u'已收'
														else:
																stealed=u'未知'
														if self.autoplant: #自动种植
																if land['b'] == 6 and land['j']==s[0]['maturingTime'] or land['b']==7:
																		self.scarity(self.uid,i) #翻地
																		for seed in self.bag['1']: #种值 if seed['type']==1:
																						self.plant(seed['cId'],self.uid,i)
																						stealed+=u'已种'
																						self.getBag()
																						break
												else:#别人的地
														if 1==land['n']:
																stealed=u'偷过'
														elif land['b']==7:
																stealed=u'已收'
														elif land['m']==land['l']:
																stealed=u'偷光'
														elif dogs['dogId']==0 or time.time()>dogs['dogUnWorkTime'] and self.autosteal : #偷
																temp=self.steal(friend['userId'],i)
																if not temp==None and temp.has_key('harvest'):
																	stealed=u'偷得%d' % temp['harvest']
																elif temp['direction']=='':
																	print temp
																else:
																	print temp
																	self.listFriends()
																	return
										else: #未成熟
												if (durtime-int(time.time())) < interval:#开新线程偷
														print 'set Thread %s' % time.ctime(durtime)
														if friend['userId']==self.uid:
																temp = -1
														else:
																temp = friend['userId']
														thread.start_new_thread(threadSteal,
																		(self,durtime,temp,i))
												if not ret.has_key(friend['userId']) or ret[friend['userId']] > durtime:
														ret[friend['userId']]=durtime
												stealed=time.ctime(durtime)
										if land['f'] > 0:
												weed=str(land['f'])+u'草,已除'
												while self.autoweed and land['f']>0:
														self.clearWeed(friend['userId'],i)
														land['f']-=1
										else:
												weed=''
										if land['h']==0:
												han=u'干旱,已浇水'
												if self.autowater:
														self.water(friend['userId'],i)
										else:
												han=''
										if land['t']>0 or land['g']>0:
												if self.autonorm:
														while land['g']>0: #杀小虫
																self.spraying(friend['userId'],i)
																land['g']-=1
																norm=u'捉虫1'
														if land['t'] > 0:  #杀大虫
																self.spraying(friend['userId'],i)
																norm=u'杀虫1/%d' % land['t']
										else:
												norm=''

										temp=u'\t%d\t%s\t%s\t%s\t%s\t%s' % (
														i,
														s[0]['cName'],
														stealed,
														weed,
														han,
														norm)
										print temp.encode('utf-8')
				return ret.values()
		def test(self):
				self.login('hackjin@yeah.net','jinhank')
				self.temp=pickle.load(open('noffs.pic'))
		def delFriend(self,fid):
				url='http://icode.renren.com/getcode.do?t=delfriend_&temp=%d' % int(time.time()*1000)
				open('test.bmp','w').write(self.req(url))
				verify=sys.stdin.readline()
				url='http://friend.renren.com/DelFriend.do'
				body={'code':verify[0:len(verify)-1],'id':fid}
				return json.read(self.req(url,body))
#在时间T自动去偷
def threadSteal(farm, T, fid, place):
		sleepT = T - int(time.time()+2)
		if sleepT < 0:
				sleepT = 0
		time.sleep(sleepT)
		if fid==-1:
				temp=farm.harvest(place)
		else:
				temp=farm.steal(fid,place)
		if temp.has_key('harvest'):
				print u'Thread 偷得 %d at %s:%d'.encode('utf-8') % (temp['harvest'],fid,place)
		else:
				print u'Thread Failed %s at %s:%d'.encode('utf-8') % (temp,fid,place)
def scanMyFarm(farm):
		interval=20*60
		while True:
				farm.myfarm(interval)
				print 'my farm scan Sleeping %s ' % time.ctime()
				time.sleep(interval)

if __name__=='__main__':
		import sys
		if True:
				pass
#				sys.stdout=open('main.log','w')
#				sys.stderr=open('main.err','w')
		farm = Farm()
#		farm.debug=True
		print 'Login...'
		#friend farm
		while True:
#				try:
						if True:
								print u'请输入校内网用户邮箱:'.encode('utf-8')
								email=sys.stdin.readline()
								print u'请输入密码:'.encode('utf-8')
								password=sys.stdin.readline()
								farm.login(email[0:len(email)-1],password[0:len(password)-1])
						else:
								farm.login('test@test.net','xiaoneiPassword')
						#		farm.addFriends();
						interval=10*60
						thread.start_new_thread(scanMyFarm,(farm,))
						while True:
								print 'Auto run...'
								farm.autorun(interval=60*60) #自动运行
								print u'%s等待下次运行:%d分钟后'.encode('utf-8') % (time.ctime(),interval/60)
								sys.stdout.flush()
								time.sleep(interval)
#				except:
						continue

