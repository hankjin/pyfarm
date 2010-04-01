<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<%
//修改个人信息
User user = (User)request.getSession().getAttribute("user");
if(user == null)
	return ;
%>
<form method="post" action="do_update.jsp">
<table border="0">
	<tr>
		<td>ID:</td>
		<td><%=user.id%></td>
	</tr>
	<tr>
		<td>新密码:</td>
		<td><input name="pass" type="password" value=<%=user.pass%>></td>
	</tr>
	<tr>
		<td>姓名:</td>
		<td><input name="name" type="text" value=<%=user.name%>></td>
	</tr>
	<tr>
		<td>Email:</td>
		<td><input name="email" type="text" value=<%=user.email%>></td>
	</tr>
	<tr>
		<td>手机:</td>
		<td><input name="phone" type="text" value=<%=user.phone%>></td>
	</tr>
	<tr>
		<td>爱好:</td>
		<td><input name="hobby" type="text" value=<%=user.hobby%>></td>
	</tr>
	<tr>
		<td colspan="2">
			<input type="submit" value="修改">
			<input type="button" value="取消" onclick="javascript:history.go(-1);">
		</td>
	</tr>
</table>
</form>
