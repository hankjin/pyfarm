<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<html>
<head>
	<script type="text/javascript" src="../image/calendar.js"></script>
</head>
<body>
<h1 align="center">添加提醒</h1>
<form method="post" action="do_update.jsp">
<input type="hidden" name="action" value="add">

<table align="center" border="0">
	<tr>
		<td>时间：</td>
		<td><input name="hwhen" onfocus="setday(this)"></td>
	</tr>
	<tr>
		<td>提醒类型：</td>
		<td><select name="notetype">
				<option value="S">短信</option>
				<option value="E">Email</option>
			</select>
		</td>
	</tr>
	<tr>
		<td>提醒内容：</td>
		<td><textarea name="content" rows="10" cols="40">
			</textarea>
		</td>
	</tr>
	
	<tr>
		<td colspan="2">
			<input type="submit" value="设置提醒">
			<input type="button" value="取消" onclick="javascript:history.go(-1);">
		</td>
	</tr>
</table>
</form>
</body>
</html>