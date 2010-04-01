<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>

<marquee><a href="../movieinfo.jsp?id=3">武汉影讯 12月21号 武当影院上映<<武当张三丰>>,票价三折</a></marquee>

<BR><BR><BR>
<div align="right">
	欢迎您:
<%
cbib.db.User user = (cbib.db.User)request.getSession().getAttribute("user");
if(user != null)
	out.println(user.name);
else
	out.println("客人");
%>
	<a href="../user/logout.jsp">退出</a>
</div>