#!/usr/bin/env python
#encoding: utf-8
from sgmllib import SGMLParser
import re

class FriendParser(SGMLParser):
    def reset(self):
        self.friend={'info':''}
        self.friends=[]
        self.status=0
        SGMLParser.reset(self)
    
    #一个好友开始
    def start_p(self,attrs):
        if len(attrs) == 1 and attrs[0][0]=='class' and attrs[0][1]=='avatar':
            self.status = 1
    def start_div(self,attrs):
        pass
    #一个好友结束
    def end_div(self):
        if self.status>0:
            self.friends.append(self.friend.copy())
            self.status=0
    #照片
    def start_img(self,attrs):
        if self.status>0:
            for attr in attrs:
                if attr[0]=='src':
                    self.friend['pic']=attr[1]
    #ID
    def start_a(self,attrs):
        if self.status>1:
            self.friend['id']=re.findall('(\d+)',attrs[0][1])[0]
	    self.friend['id']=int(self.friend['id'])
    #姓名和信息
    def handle_data(self,text):
        if self.status>0 and len(text.strip())>0:
            text=text.strip().replace('\n','')
            if self.status==1: #姓名
                self.status=2
            elif self.status==2: #胡进
                self.friend['name']=text
                self.status=3
            elif self.status==3: #学校
                self.friend['info']=text
                self.status=4
            elif self.status==4: #华中科技
                self.friend['info']+=text
if __name__=='__main__':
    parser=FriendParser()
    for i in range(1000):
        parser.feed(open('test').read())
        print len(parser.friends)
        parser.reset()
#print parser.friends
