<%@ page contentType="text/html;charset=UTF-8" import="cbib.db.*,java.sql.*,java.util.*"%>
<link href="../image/menu.css" rel="stylesheet" type="text/css" />
<!-- SCRIPT部分 -->
<script language="javascript">
function showHideUL(ulId){
	// 先全部隐藏
	var uls = document.getElementsByTagName("ul");
	for (i=0;i<uls.length;i++)
	{
		uls[i].style.display="none";
	}
	// 再显示点击的
	var ul=document.getElementById(ulId);
	// 原隐藏即显示，原显示即隐藏
	if(ul.style.display=="block" || ul.style.display=="" ){
		ul.style.display="none";
	}
	else{
		ul.style.display="block";
	}
}
</script>

<!-- 链接导航部分 -->
<div class="ulTitle"  onclick="showHideUL('poems')"/><a href="#">用户信息</a></div>
<ul id='poems' style='display:none'>
<li><a target="main" href="../user/info.jsp">个人信息</a></li>
<li><a target="main" href="../user/update.jsp">修改密码</a></li>
<li><a target="main" href="../user/update.jsp">修改资料</a></li>
<li><a target="main" href="../user/logout.jsp">退出登录</a></li>
</ul>

<div class="ulTitle"  onclick="showHideUL('zens')"/><a href="#">电影分类</a></div>
<ul id='zens' style='display:none'>
<!-- 添加分类功能在后面的管理部分 -->
<%
List<MovieCategory> categorys = MovieCategory.getMovieCategorys();
for(MovieCategory category : categorys){
%>
<li><a target="main" href="../movie/listMovie.jsp?category=<%=category.id %>"><%=category.name %></a></li>
<%	
}
%>
</ul>
<div class="ulTitle"  onclick="showHideUL('cinema')"/><a href="#">影院列表</a></div>
<ul id='cinema' style='display:none'>
<%
List<Cinema> cinemas = Cinema.getCinemas();
for(Cinema cinema:cinemas){
%>
<li><a target="main" href="../movie/cinema.jsp?id=<%=cinema.id %>"><%=cinema.name %></a></li>
<%	
}
%>
</ul>

<div class="ulTitle"  onclick="showHideUL('logions')"/><a href="#">影讯</a></div>
<ul id='logions' style='display:none'>
<li><a target='main' href="../search/list.jsp?type=latest">最新</a></li>
<li><a target='main' href="../search/list.jsp?type=hottest">最热</a></li>
<li><a target='main' href="../search/list.jsp?type=oldest">最老</a></li>
<li><a target='main' href="../search/list.jsp?type=coldest">最冷</a></li>
</ul>




<div class="ulTitle"  onclick="showHideUL('couplets')"/><a href="#">提醒</a></div>
<ul id='couplets' style='display:none'>
<li><a target="main" href="../reminder/list.jsp?type=current">当前提醒</a></li>
<li><a target="main" href="../reminder/add.jsp">设置提醒</a></li>
</ul>
<%
User user = (User)request.getSession().getAttribute("user");
if(user != null && user.id.equals("admin")){
%>
<div class="ulTitle"  onclick="showHideUL('website')"/><a href="#">站点管理</a></div>
<ul id='website' style='display:none'>
<li><a target="main" href="../site/list.jsp">所有站点</a></li>
<li><a target="main" href="../site/add.jsp">添加站点</a></li>
<li><a target="main" href="../site/list.jsp">修改站点</a></li>
</ul>
<div class="ulTitle"  onclick="showHideUL('movieM')"/><a href="#">电影管理</a></div>
<ul id='movieM' style='display:none'>
<li><a target="main" href="../movie/listCategory.jsp">电影分类</a></li>
<li><a target="main" href="../movie/addCategory.jsp">添加分类</a></li>
<li><a target="main" href="../movie/listMovie.jsp">所有电影</a></li>
<li><a target="main" href="../movie/add.jsp">添加电影</a></li>

</ul>

<%	
}
%>


