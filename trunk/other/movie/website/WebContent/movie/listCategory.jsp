<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<a href="add.jsp">添加</a>
<table>
<!--表头-->
<tr>
	<td>编号</td>
	<td>分类名称</td>
	<td>重命名</td>
	<td>删除</td>
</tr>
<%
List<MovieCategory> categorys = MovieCategory.getMovieCategorys();
for(MovieCategory category:categorys){
%>
<tr>
	<td><%=category.id%></td>
	<td><%=category.name%></td>
	<td><a href="editCategory.jsp?id=<%=category.id%>">重命名</a></td>
	<td><a href="do_category.jsp?action=del&id=<%=category.id%>">删除</a></td>

</tr>
<%
}
%>
</table>
