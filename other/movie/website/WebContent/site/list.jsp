<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<a href="add.jsp">添加</a>
<table>
<!--表头-->
<tr>
	<td>网站</td>
	<td>说明</td>
	<td>Python解析器</td>
	<td>修改</td>
	<td>删除</td>
</tr>
<%
List<WebSite> sites = WebSite.getWebSites();
for(WebSite site:sites){
%>
<tr>
	<td><%=site.url%></td>
	<td><%=site.note%></td>
	<td><%=site.parser%></td>
	<td><a href="edit.jsp?id=<%=site.id%>">修改</a></td>
	<td><a href="del.jsp?id=<%=site.id%>">删除</a></td>

</tr>
<%
}
%>
</table>
