<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
User user = (User)request.getSession().getAttribute("user");
user.pass = request.getParameter("pass");
user.name = request.getParameter("name");
user.email = request.getParameter("email");
user.phone = request.getParameter("phone");
user.hobby = request.getParameter("hobby");
String alert = "";
try{
	user.update2DB(); //更新到数据库
	request.getSession().setAttribute("user", user);//更新session
	alert = "<script language='javascript'>alert('更新个人信息成功');parent.window.location='../common/index.html';</script>";
}
catch(SQLException se){
	alert="更新失败,请重试";
}
out.println(alert);
%>