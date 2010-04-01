<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<title>电影信息 - 武汉影讯</title>
</head>
<body>
<%
String id = request.getParameter("id");
Movie movie = Movie.getMovie(Integer.parseInt(id));
%>
<table>
<tr>
	<td width="10%">名称</td>
	<td><%=movie.name %></td>
</tr>
<tr>
	<td>类型</td>
	<td><%=movie.category %></td>
</tr>
<tr>
	<td>导演</td>
	<td><%=movie.director %></td>
</tr>
<tr>
	<td>主演</td>
	<td><%=movie.actor %></td>
</tr>
<tr>
	<td>剧照</td>
	<td><img src="<%=movie.poster==null?"no.jpg":movie.poster %>"></td>
</tr>
<tr>
	<td>剧情简介</td>
	<td><%=movie.description.replace("\n","<P>") %></td>	
</tr>
<tr>
	<td>预告片</td>
	<td><a href="<%=movie.preview %>">预告片</a></td>
</tr>
<tr>
	<td colspan="2">
	<a href="../search/search.jsp?type=movieid&value=<%=movie.id %>">近期影讯</a>
	</td>
</tr>
<tr>
	<td>影评</td>
	<td><iframe width="100%" src="comment.jsp?movie=<%=movie.id %>"></iframe>
	
	</td>
</tr>
</table>
<a href="javascript:history.go(-1);">返回</a>
</body>
</html>
