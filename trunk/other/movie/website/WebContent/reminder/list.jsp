<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*,java.text.*"%>
<a href="add.jsp">添加</a>
<table>
<!--表头-->
<tr>
	<td width="20%">时间</td>
	<td width="10%">提醒方式</td>
	<td width="*">提醒内容</td>
	<td width="5%">删除</td>
</tr>
<%
List<Reminder> reminders = Reminder.getUserReminders(((User)request.getSession().getAttribute("user")).id);
DateFormat df = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
for(Reminder reminder:reminders){
%>
<tr>
	<td><%=df.format(reminder.hwhen)%></td>
	<td><%=reminder.notetype.equals("S")?"短信":"Email"%></td>
	<td><%=reminder.content%></td>
	<td><a href="do_update.jsp?action=delete&id=<%=reminder.id%>">删除</a></td>
</tr>
<%
}
%>
</table>
