<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
ResultSet rs = new DBConn().getRs("select * from test");
out.println("数据库连接测试<BR>");
while(rs.next()){
	String name = rs.getString("name");
	out.println(name);
}
%>