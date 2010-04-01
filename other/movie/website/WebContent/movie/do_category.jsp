<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*"%>
<%
String action = request.getParameter("action");
String succeed="", failed="", operation="";

MovieCategory category = new MovieCategory();
category.name = request.getParameter("name");

String alert = "";


try{
	if(action.equals("add")){//添加
		MovieCategory.persistent(category);
		operation = "添加分类";
		succeed = "../movie/listCategory.jsp";
		failed = "../movie/addCategory.jsp";
	}
	else if(action.equals("edit")){//修改
		category.update2DB();
		operation = "修改分类";
		succeed = "../movie/listCategory.jsp"; 
		failed = "../movie/editCategory.jsp";
	}
	else if(action.equals("del")){//删除
		operation = "删除分类";
		String id = request.getParameter("id");
		MovieCategory.delCategory(Integer.parseInt(id));
		succeed = "../movie/listCategory.jsp";
		failed = succeed;
	}
	alert = "<script language='javascript'>alert('"+operation+"成功');window.location='"+succeed+"';</script>";
}
catch(SQLException se){
	se.printStackTrace();
	alert="<script language='javascript'>alert('"+operation+"失败,请重试');window.location='"+failed+"';</script>";
}
out.println(alert);
%>