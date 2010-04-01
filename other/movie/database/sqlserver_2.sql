-- 用户
create table huser(
	id varchar(20) primary key,		-- ID, id为admin的是管理员账户
	pass varchar(20) not null,		-- 密码
	name varchar(20),				-- 姓名
	gender char,					-- 性别
	birthday datetime,				-- 生日
	hobby varchar(200),			-- 爱好
	phone char(11),				-- 手机号
	email varchar(40)				-- Email
);

-- 提醒 针对用户,提醒内容一定,固定的时间运行
create table reminder(
	id int primary key ,
	notetype char,				-- 提醒类型 短信/邮件 S/E
	huser varchar(20) not null,		-- 用户
	hwhen int,				-- 提醒时间
	content varchar(200),			-- 提醒内容	
	foreign key(huser) references huser(id) on delete cascade
);

-- 通知 针对网站,有新电影时通知
create table notify(
	id int primary key ,
	huser varchar(20) not null,		-- 用户
	notetype char,				-- 提醒类型 短信/邮件 S/E
	movie int,				-- 新内容
	foreign key(huser) references huser(id) on delete cascade,
	foreign key(movie) references movie(id) on delete cascade
);
