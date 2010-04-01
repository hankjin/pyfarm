<%@page contentType="text/html;charset=UTF-8" import="java.io.RandomAccessFile"%>
<%
//把网站的访问记录记录到日志中
String fileName = "";
if(System.getProperty("os.name").startsWith("Windows"))
	fileName = "D:\\log.txt";
else
	fileName = "/home/cbib/log/movie.log";
RandomAccessFile file = new RandomAccessFile(fileName,"rw");
file.seek(file.length());
file.writeUTF(new java.util.Date() +"\t"+ request.getRemoteAddr()+"\r\n");
file.close();
%>