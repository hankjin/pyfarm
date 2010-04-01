<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
User user = new User();
user.id = request.getParameter("id");
user.pass = request.getParameter("pass");
String alert = "";
try{
	User.persistent(user);
	alert = "<script language='javascript'>alert('注册成功');window.location='login.html';</script>";
}
catch(SQLException se){
	alert = "<script language='javascript'>alert('注册失败,该ID已经被占用,请使用其它ID');window.location='register.html';</script>";
}
out.println(alert);
	
%>