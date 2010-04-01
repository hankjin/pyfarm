<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<title>影院 - 武汉影讯</title>
</head>
<body>
<%
String id = request.getParameter("id");
Cinema cinema = Cinema.getCinema(Integer.parseInt(id));
%>
<table>
<tr>
	<td width="10%">名称</td>
	<td><%=cinema.name %></td>
</tr>
<tr>
	<td>地址</td>
	<td><%=cinema.address %></td>
</tr>
<tr>
	<td>介绍</td>
	<td><%=cinema.introduce==null?"暂无":cinema.introduce %></td>
</tr>
<tr>
	<td>环境</td>
	<td><img src="<%=cinema.environment==null?"../image/cinema/no.jpg":cinema.environment %>"></td>
</tr>
<tr>
	<td>交通</td>
	<td><%=cinema.tranffic==null?"暂无":cinema.tranffic %></td>	
</tr>
<tr>
	<td>工作时间</td>
	<td><%=cinema.workHour==null?"暂无":cinema.workHour %></td>
</tr>
<tr>
	<td colspan="2">
	<a href="../search/search.jsp?type=cinemaid&value=<%=cinema.id %>">近期影讯</a>
	</td>
</tr>
</table>
<a href="javascript:history.go(-1);">返回</a>
</body>
</html>
