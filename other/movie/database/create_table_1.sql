use movie;
-- 电影院
create table cinema(
	id int primary key auto_increment,
	name varchar(40) not null, 	-- 电影院名称
	zone varchar(20),		-- 地区
	address varchar(200) not null,	-- 地址
	phone varchar(30),		-- 电话
	workHour varchar(20),		-- 营业时间
	price int,			-- 一般价格
	discount varchar(10),		-- 打折
	tranffic varchar(200),		-- 交通
	introduce varchar(200),		-- 简介
	environment varchar(200)	-- 影院条件
);
	
-- 电影分类
create table movieCategory(
	id int primary key auto_increment,
	name varchar(40) not null,	-- 分类名称
	parent int default 1,		-- 父类
	foreign key(parent) references movieCategory(id)
);

-- 电影(片名,简介,主演...)
create table movie(
	id int primary key auto_increment,
	name varchar(80) not null,	-- 电影名
	director varchar(20) not null,	-- 导演
	actor varchar(80),		-- 主演
	category varchar(20),		-- 类型
	duration varchar(20),		-- 片长
	description text,		-- 剧情
	poster varchar(200),		-- 剧照
	preview varchar(200),		-- 预告片
	online varchar(200)		-- 在线观看
);

-- 影讯(时间,影院
create table movieinfo(
	id int primary key auto_increment,
	hdate varchar(20),		-- 放映日期
	htime varchar(80),		-- 放映时间
	cinema int,			-- 影院
	movie int,			-- 电影
	price int,			-- 价格
	cinemaname varchar(20),	-- 影院名称
	foreign key(cinema) references cinema(id),
	foreign key(movie) references movie(id)
);

-- 影评
create table comment(
	id int primary key auto_increment,
	movie int,			-- 电影
	user varchar(20),		-- 评论用户
	title varchar(100),		-- 评论标题
	comment text,			-- 评论内容
	hwhen datetime,			-- 时间
	foreign key(movie) references movie(id)
);
