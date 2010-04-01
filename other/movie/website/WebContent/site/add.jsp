<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/movie.css" rel="stylesheet" type="text/css" />
<!--添加站点-->
<form method="post" action="do_update.jsp">
<input type="hidden" name="action" value="add">
<input type="hidden" name="id" value="1">
<table>
<tr>
	<td>网站</td>
	<td><input type="text" name="url" ></td>
</tr>
<tr>
	<td>说明</td>
	<td><input type="text" name="note" ></td>
</tr>
<tr>
	<td>Python解析器</td>
	<td><input type="text" name="parser" ></td>
</tr>
<tr>
	<td><input type="submit" value="添加"></td>
	<td><input type="reset" value="取消"></td>
</tr>
</table>
