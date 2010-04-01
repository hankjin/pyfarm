<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<ul>
<%
String movie = request.getParameter("movie");
out.println(movie);
if(movie == null || movie.trim().equals(""))
	return ;
List<Comment> comments = Comment.getCommentsOfMovie(Integer.parseInt(movie));
for(Comment comment:comments){
%>
<li>
	<B><%=comment.title %> &nbsp;&nbsp;&nbsp;<%=comment.user %> &nbsp;&nbsp;&nbsp;&nbsp;<%=comment.hwhen %>
	</B>
	<p><%=comment.comment.replace("\n","<BR>") %>
	</p>
</li>
<%	
}
%>
</ul>
