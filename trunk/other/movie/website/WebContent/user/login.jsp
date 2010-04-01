<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
String id = request.getParameter("id");
String pass = request.getParameter("pass");
User user = User.getUser(id);
if(user != null && user.pass.equals(pass)){
	request.getSession().setAttribute("user", user);
	String alert="<script language='javascript'>window.location='../common/index.html';</script>";
	out.println(alert);
}
else{
	String alert="<script language='javascript'>alert('登录失败');window.location='../user/login.html';</script>";
	out.println(alert);
}
%>
