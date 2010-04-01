use movie;
-- ��ӰԺ
create table cinema(
	id int primary key auto_increment,
	name varchar(40) not null, 	-- ��ӰԺ����
	zone varchar(20),		-- ����
	address varchar(200) not null,	-- ��ַ
	phone varchar(30),		-- �绰
	workHour varchar(20),		-- Ӫҵʱ��
	price int,			-- һ��۸�
	discount varchar(10),		-- ����
	tranffic varchar(200),		-- ��ͨ
	introduce varchar(200),		-- ���
	environment varchar(200)	-- ӰԺ����
);
	
-- ��Ӱ����
create table movieCategory(
	id int primary key auto_increment,
	name varchar(40) not null,	-- ��������
	parent int default 1,		-- ����
	foreign key(parent) references movieCategory(id)
);

-- ��Ӱ(Ƭ��,���,����...)
create table movie(
	id int primary key auto_increment,
	name varchar(80) not null,	-- ��Ӱ��
	director varchar(20) not null,	-- ����
	actor varchar(80),		-- ����
	category varchar(20),		-- ����
	duration varchar(20),		-- Ƭ��
	description text,		-- ����
	poster varchar(200),		-- ����
	preview varchar(200),		-- Ԥ��Ƭ
	online varchar(200)		-- ���߹ۿ�
);

-- ӰѶ(ʱ��,ӰԺ
create table movieinfo(
	id int primary key auto_increment,
	hdate varchar(20),		-- ��ӳ����
	htime varchar(80),		-- ��ӳʱ��
	cinema int,			-- ӰԺ
	movie int,			-- ��Ӱ
	price int,			-- �۸�
	cinemaname varchar(20),	-- ӰԺ����
	foreign key(cinema) references cinema(id),
	foreign key(movie) references movie(id)
);

-- Ӱ��
create table comment(
	id int primary key auto_increment,
	movie int,			-- ��Ӱ
	user varchar(20),		-- �����û�
	title varchar(100),		-- ���۱���
	comment text,			-- ��������
	hwhen datetime,			-- ʱ��
	foreign key(movie) references movie(id)
);
