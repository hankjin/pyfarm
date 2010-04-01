<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.util.*,java.text.*"%>
<%
String action = request.getParameter("action");

String alert = "";
User user = (User)request.getSession().getAttribute("user");
try{
	if(action.equals("delete")){//删除
		Reminder.delReminder(Integer.parseInt(request.getParameter("id")));
		alert = "<script language='javascript'>alert('删除成功');window.location='../reminder/list.jsp'</script>";
	}
	else if(user == null){//如果尚未登录
		alert = "<script language='javascript'>alert('请先登录');parent.window.location='../user/login.html'</script>";
	}
	else{
		Reminder reminder = new Reminder();
		reminder.user = user.id;
		DateFormat df = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
		reminder.hwhen = df.parse(request.getParameter("hwhen"));
		reminder.content = request.getParameter("content");
		reminder.notetype = request.getParameter("notetype");
		if(action.equals("add")){//添加
			Reminder.persistent(reminder);
			alert = "<script language='javascript'>alert('添加成功');window.location='../reminder/list.jsp'</script>";
		}
		else if(action.equals("update")){//修改
			reminder.id = Integer.parseInt(request.getParameter("id"));
			reminder.update2DB();
			alert = "<script language='javascript'>alert('修改成功');window.location='../reminder/list.jsp'</script>";
		}
	}
}
catch(Exception se){
	se.printStackTrace();
	alert="更新失败,请重试";
}
out.println(alert);
%>