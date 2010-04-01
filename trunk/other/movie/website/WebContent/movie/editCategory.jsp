<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<!--添加站点分类-->
<form method="post" action="do_category.jsp">
<input type="hidden" name="action" value="edit">
<input type="hidden" name="id" value="1">
<table>
<tr>
	<td>网站分类</td>
	<td><input type="text" name="name" ></td>
</tr>
<tr>
	<td><input type="submit" value="修改"></td>
	<td><input type="reset" value="取消"></td>
</tr>
</table>
