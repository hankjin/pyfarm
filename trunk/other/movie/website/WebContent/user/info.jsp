<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<h1 align="center">个人信息</h1>
<%
//修改个人信息
User user = (User)request.getSession().getAttribute("user");
if(user == null)
	return ;
%>
<table border="0">
	<tr>
		<td>ID:</td>
		<td><%=user.id%></td>
	</tr>
	<tr>
		<td>姓名:</td>
		<td><%=user.name%></td>
	</tr>
	<tr>
		<td>Email:</td>
		<td><%=user.email%></td>
	</tr>
	<tr>
		<td>手机:</td>
		<td><%=user.phone%></td>
	</tr>
	<tr>
		<td>爱好:</td>
		<td><%=user.hobby%></td>
	</tr>
</table>
