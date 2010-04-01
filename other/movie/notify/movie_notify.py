#!/usr/bin/python
import urllib,smtplib,base64,pickle,time,os
from sgmllib import SGMLParser
class NewsParser(SGMLParser):
        def reset(self):
                self.news={}
                self.bNew=False
                self.bNewLink=''
                self.users=[]
                SGMLParser.reset(self)
        def start_a(self,attr):
                target=[t for t,v in attr if t=='target']
                if target:
                        self.bNew=True
                        self.newLink=[l for h,l in attr if h=='href'][0]
        def handle_data(self,text):
                if self.bNew:
                        self.bNew=False
                        self.news[text]=self.newLink
class HustNews:
        def __init__(self):
                self.oldNews={}
                self.news={}
                self.diff={}
                self._debug=0
        """
        load news user
        """
        def loadUser(self):
                p=pickle.Unpickler(open('/home/cbib/news/news.user'))
                self.users=p.load()
        """
        load old news from disk
        """
        def loadNews(self):
                file=open('/home/cbib/news/news.old')
                unpickle=pickle.Unpickler(file)
                self.oldNews=unpickle.load()
                file.close()
        """
        read news from website
        """
        def readNews(self):
                newsurl='http://gs.hust.edu.cn/listNews.do?where=*&page=1'
                news=urllib.urlopen(newsurl)
                newsParser=NewsParser()
                newsParser.feed(news.read())
                self.news=newsParser.news
        """
        save the new version news to file
        """
        def saveNews(self):
                file=open('/home/cbib/news/news.old','w')
                saver=pickle.Pickler(file)
                saver.dump(self.news)
                file.close()
        """
        notify a user about the new news with email
        """
        def notifyUser(self):
                for i in self.news.keys():
                        if not self.oldNews.has_key(i):
                                self.diff[i]=self.news[i]
                if len(self.diff) == 0:
                        print '%s No new News' % time.localtime()
                        return
                content=''
                for new in self.diff:
                        content += "<a href='%s%s'>%s</a><BR>" %(
                        "http://gs.hust.edu.cn",self.diff[new],new)
                header="MIME-Version: 1.0\r\nContent-Type: text/html\r\n"
                subject='Subject: HustNews(%d)%s\r\n\r\n' % (len(self.diff),
                                self.diff.keys()[0])


                server=smtplib.SMTP('smtp.163.com')
                server.set_debuglevel(self._debug)
                server.docmd("EHLO server")
                server.docmd("AUTH LOGIN")
                server.send(base64.encodestring("hankjin"))
                server.getreply()
                server.send(base64.encodestring("851017"))
                server.getreply()
                server.docmd("MAIL FROM:" + "<hankjin@163.com>")
                for user in self.users:
                        server.docmd("RCPT TO:<%s>" % user)
                server.docmd("DATA")
                server.send(header)
                server.send(subject)
                server.send(content)
                server.send("\r\n.\r\n")
                server.getreply()
                server.quit()
                #sms
                #os.system('/home/cbib/fetion/fetion -u 15171459880 -p 851017 -b
 /home/cbibadmin/wiki/pages/Fetion_user.txt')
                print '%s send email' % time.localtime()
        def checkNews(self):
                self.readNews()
                self.loadNews()
                self.notifyUser()
                self.saveNews()

if __name__=='__main__':
        news=HustNews()
        news.loadUser()
        news.checkNews()
