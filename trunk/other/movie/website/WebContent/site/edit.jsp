<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<%
String id = request.getParameter("id");
WebSite site = WebSite.getWebSite(Integer.parseInt(id));
if(site == null){
	out.println("Error");
}
%>
<form method="post" action="do_update.jsp">
<input type="hidden" name="action" value="edit">
<input type="hidden" name="id" value="<%=id%>">
<table>
<tr>
	<td>网站</td>
	<td><input type="text" name="url" value="<%=site.url%>"></td>
</tr>
<tr>
	<td>说明</td>
	<td><input type="text" name="note" value="<%=site.note%>"></td>
</tr>
<tr>
	<td>Python解析器</td>
	<td><input type="text" name="parser" value="<%=site.parser%>"></td>
</tr>
<tr>
	<td><input type="submit" value="修改"></td>
	<td><input type="reset" value="取消"></td>
</tr>
</table>
