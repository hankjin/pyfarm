<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<table align="center" width=100%>
<!--表头-->
<tr>	
	<th width="12%">电影名</th>
	<th width="8%">导演</th>
	<th width="10%">主演</th>
	<th width="10%">类别</th>
	<th width="*">简介</th>
	<th width="10%">海报</th>
	<th width="10%">预告片</th>
	<th width="12%">上映信息</th>
</tr>
<%
String category = request.getParameter("category");
List<Movie> movies;
if(category == null){
	movies = Movie.getMovies("select * from movie");
}
else{
	MovieCategory mc = MovieCategory.getMovieCategory(Integer.parseInt(category));
	movies = Movie.getMovies("select * from movie where category  like '%"
	+mc.name+"%'");
}
for(Movie movie:movies){
	String poster, preview;
	if(movie.poster == null || movie.poster.equals(""))
		poster = "无";
	else
		poster = "<img src='"+movie.poster+"'>";
	if(movie.preview== null || movie.preview.equals(""))
		preview = "无";
	else
		preview = "<a href='"+movie.id+"'>预告片</a>";
		
%>
<tr>
	<td><a href="movie.jsp?id=<%=movie.id %>"><%=movie.name%></a></td>
	<td><%=movie.director %></td>
	<td><%=movie.actor.replace(" ","<BR>") %></td>
	<td><%=movie.category.replace("/","<BR>") %></td>
	<td><%=movie.description.substring(0, 
			movie.description.length()>100?100:movie.description.length()) %></td>
	<td><%=poster %></td>
	<td><%=preview %></td>
	<td><a href='../search/search.jsp?type=movieid&value=<%=movie.id %>'>上映信息</a></td>
</tr>
<%
}
%>
</table>
