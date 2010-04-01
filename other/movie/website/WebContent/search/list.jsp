<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<link href='../image/movie.css' rel='stylesheet' type='text/css'>
<title>排行榜 - 武汉</title>
</head>
<body>
<table width="100%">
<!--表头-->
<tr>	
	<td width="10%">电影名</td>
	<td width="5%">类别</td>
	<td>简介</td>
	<td width="10%">预告片</td>
	<td width="10%">上映信息</td>
</tr>
<%
String type = request.getParameter("type");
String sql = "select * from movie ";
if(type == null)
	return ;
else if(type.equals("latest"))
	sql += "order by id desc ";
else if(type.equals("oldest"))
	sql += "order by id asc ";
else if(type.equals("hottest"))
	sql += "order by actor asc ";
else if(type.equals("coldest"))
	sql += "order by actor desc ";
sql += "limit 10";
List<Movie> movies = Movie.getMovies(sql);
if(movies == null)
	return ;
for(Movie movie:movies){
        String preview, online;
        if(movie.preview==null || movie.preview.equals(""))
                preview = "无";
        else
                preview = "<a target=preview href='"+movie.preview+"'>预告片</a>";
        if(movie.online==null || movie.online.equals(""))
                online = "无";
        else
                online = "<a target=online href='"+movie.online+"'>在线观看</a>";

%>

<tr>
	<td><%=movie.name%></td>
	<td><%=movie.category.replace("/","<BR>") %></td>
	<td><%=movie.description.replace("\n","<BR>") %></td>
	<td><%=preview %></td>
	<td><a href="../search/search.jsp?type=movieid&value=<%=movie.id%>">
			上映信息</a></td>
</tr>
<%
}
%>
</table>
</body>
</html>
