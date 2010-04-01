<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href='../image/movie.css' rel='stylesheet' type='text/css'>
<script type="text/javascript" language="javascript" src="../image/calendar.js"></script>
<title>高级搜索 - 武汉</title>
</head>
<body>
<h1 align="center">高级搜索</h1>
<center><a href="../index.html">返回首页</a></center>

<form method="post" action="search.jsp">
<input type="hidden" name="type" value="advance">
<input type="hidden" name="value" value="advance">
<table align="center">
	<tr>
		<td>电影分类</td>
		<td><select name="category">
			<option value="全部" selected>全部</option>
<%
List<MovieCategory> cates = MovieCategory.getMovieCategorys();
for(MovieCategory cate : cates){
	out.println("<option value='"+cate.name+"'>"+cate.name+"</option>");
}
%>			
		</td>
	</tr>
	<tr>
		<td>电影名称</td>
		<td><input type="text" name="movie" value="全部"></td>
	</tr>
	<tr>
		<td>导演</td>
		<td><input type="text" name="director" value="全部"></td>
	</tr>
	<tr>
		<td>主演</td>
		<td><input type="text" name="actor" value="全部"></td>
	</tr>
	<tr>
		<td>上映时间</td>
		<td><select name="hdate">
			<option value="全部">全部</option>
<%
List<String> hdates = DBUtil.getMovieDate();
for(String hdate:hdates){
	out.println("<option value='"+hdate+"'>"+hdate+"</option>");
}
%>	
			</select>
		</td>
	</tr>
	<tr>
		<td>地区</td>
		<td><select name="zone">
			<option value="全部">全部</option>
			<option value="武昌">武昌</option>
			<option value="汉口">汉口</option>
			<option value="江汉">江汉</option>
			</select>
		</td>
	<tr>
		<td>电影院</td>
		<td><select name="cinema">
			<option value="全部" selected>全部</option>
<%
List<Cinema> cinemas = Cinema.getCinemas();
for(Cinema cinema:cinemas){
	out.println("<option value='"+cinema.id+"'>"+cinema.name+"</option>");
}
%>		
		</td>
	</tr>
	<tr>
		<td colspan="2">
		<input type="submit" value="搜索">
		<input type="button" onclick="javascript:history.go(-1)" value="取消">
		</td>
	</tr>
</table>
</form>
</body>
</html>
