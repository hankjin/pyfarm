<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
WebSite site = new WebSite();
site.id = Integer.parseInt(request.getParameter("id"));
site.url = request.getParameter("url");
site.parser = request.getParameter("parser");
site.note = request.getParameter("note");

String action = request.getParameter("action");
if(action.equals("add")){//添加站点
	try{
		WebSite.persistent(site);
		String alert = "<script language='javascript'>alert('Add Succeed'); window.location='list.jsp';</script>";
		out.println(alert);//添加成功
	}
	catch(SQLException se){
		out.println("Failed");
	}
}
else{
	if(site.update2DB()){//修改站点成功
		String alert = "<script language='javascript'>alert('Edit Succeed'); window.location='list.jsp';</script>";
		out.println(alert);
	}
	else{
		out.println("Edit Failed");
	}
}
%>
