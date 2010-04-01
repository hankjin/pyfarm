<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
String id = request.getParameter("id");
if(WebSite.delWebSite(Integer.parseInt(id))){
	String alert = "<script language='javascript'>alert('Delete Succeed'); window.location='list.jsp';</script>";
	out.println(alert);//删除成功
}
else{
	out.println("Del failed");//删除失败
}
%>