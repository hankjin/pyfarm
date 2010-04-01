-- 要抓取的网址列表
create table website(
	id int primary key auto_increment,
	url varchar(200),		-- 网址
	parser varchar(20),	-- Python解析器
	note varchar(200)		-- 备注
);