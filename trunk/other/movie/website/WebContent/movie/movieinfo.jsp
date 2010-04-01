<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<title>上映信息 - 武汉影讯</title>
</head>
<body>

<table>

<%
//显示电影上映信息
System.out.println("Movie Info");
String movieid = request.getParameter("id");
MovieInfo info = MovieInfo.getMovieInfo(Integer.parseInt(movieid));
String desc = info.movie.description;
desc = desc.substring(0,desc.length()>100?100:desc.length());
%>
<tr>	
	<td width="10%">海报</td>
	<td><img src="<%=info.movie.poster %>"></td>
</tr>
<tr>
	<td>电影</td>
	<td><a href="../movie/movie.jsp?id=<%=info.movie.id%>"><%=info.movie.name %></a></td>
</tr>
<tr>
	<td>电影简介</td>
	<td><%=desc %><a href="../movie/movie.jsp?id=<%=info.movie.id%>">更多</a></td>
</tr>
<tr>
	<td>影院</td>
	<td><a href="../movie/cinema.jsp?id=<%=info.cinema.id %>"><%=info.cinema.name %></a></td>
</tr>
<tr>
	<td>影院简介</td>
	<td><%=info.cinema.introduce %></td>
<tr>
	<td>上映日期</td>
	<td><%=info.hdate %></td>
</tr>
<tr>
	<td>上映时间</td>
	<td><%=info.htime==null?"":info.htime.replace("|","<BR>") %></td>
</tr>

</table>
</body>
</html>
