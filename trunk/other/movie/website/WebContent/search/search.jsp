<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,cbib.db.filter.*,java.sql.*,java.util.*"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">

<%@page import="java.io.RandomAccessFile"%><html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<title>搜索结果 - 武汉影讯</title>
</head>
<body>
<form method="post" action="search.jsp">
	<center>
		<input type="radio" name="type" value="movie" checked>电影名
		<input type="radio" name="type" value="cinema">影院名
		<a href="../doc/help.html">帮助</a>&nbsp;&nbsp;
		<a href="../search/advance.jsp">高级搜索</a>
	</center>
	<hr>
	<center>
		<input type=text name="value" size=42 maxlength=100>
		<input type=submit value=搜索>
	</center>
</form>
<table>	
<tr>
	<th width="20%">电影海报</th>
	<th width="10%">电影</th>
	<th width="*">简介</th>
	<th width="20%">影院</th>
	<th width="10%">上映日期</th>
	<th width="10%">上映时间</th>
	<th width="10%">详细信息</th>
</tr>
<%
MFilter filter = new MFilter();
String type = request.getParameter("type");
String value = request.getParameter("value");
System.out.println("Hi"+type+"Search");
if(type == null||value==null||value.trim().equals("")){	
}
else if(type.equals("cinema")){
	filter.addCondition("cinema.name", value);
}
else if(type.equals("movie")){
	filter.addCondition("movie.name", value);
}
else if(type.equals("movieid")){
	filter.addRawCondition("movie.id="+value);
}
else if(type.equals("cinemaid")){
	filter.addRawCondition("cinema.id="+value);
}
else if(type.equals("advance")){
	String zone = request.getParameter("zone");
	String category = request.getParameter("category");
	String movie = request.getParameter("movie");
	String director = request.getParameter("director");
	String actor = request.getParameter("actor");
	String hdate = request.getParameter("hdate");
	String cinema = request.getParameter("cinema");
	if(zone != null && !zone.equals("全部"))
		filter.addCondition("cinema.zone",zone);
	if(category != null && !category.equals("全部"))
		filter.addCondition("movie.category",category);
	if(movie != null && !movie.equals("全部"))
		filter.addCondition("movie.name",movie);
	if(director != null && !director.equals("全部"))
		filter.addCondition("movie.director", director);
	if(actor != null && !actor.equals("全部"))
		filter.addCondition("movie.actor", actor);
	if(hdate != null && !hdate.equals("全部"))
		filter.addCondition("movieinfo.hdate", hdate);
	if(cinema != null && !cinema.equals("全部"))
		filter.addCondition("cinema.name", cinema);
}
List<MovieInfo> infos = MovieInfo.getMovieInfos(filter);
for(MovieInfo info:infos){
	String desc = info.movie.description;
	desc = desc.substring(0,desc.length()>100?100:desc.length());
%>
<tr>	
	<td><img src="<%=info.movie.poster %>"></td>
	<td><a href="../movie/movie.jsp?id=<%=info.movie.id%>"><%=info.movie.name %></a></td>
	<td><%=desc %><a href="../movie/movie.jsp?id=<%=info.movie.id%>">更多</a></td>
	
	<td><a href="../movie/cinema.jsp?id=<%=info.cinema.id %>"><%=info.cinema.name %></a></td>
	<td><%=info.hdate %></td>
	<td><%=info.htime==null?"":info.htime.replace("|","<BR>") %></td>
	<td><a href="../movie/movieinfo.jsp?id=<%=info.id%>">详情</a></td>
</tr>
<%
}
%>	
</table>
<a href="../index.html">返回</a>
</body>
</html>
