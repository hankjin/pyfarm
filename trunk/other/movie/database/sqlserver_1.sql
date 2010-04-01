use movie;
-- ��ӰԺ
create table cinema(
	id int primary key ,
	name varchar(40) not null, 	-- ��ӰԺ����
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
	id int primary key ,
	name varchar(40) not null,	-- ��������
	parent int default 1,		-- ����
	foreign key(parent) references movieCategory(id)
);

-- ��Ӱ(Ƭ��,���,����...)
create table movie(
	id int primary key ,
	name varchar(80) not null,	-- ��Ӱ��
	category int default 1,		-- ���
	description varchar(200),	-- ����
	preview varchar(200),		-- Ԥ��Ƭ
	online varchar(200),		-- ���߹ۿ�
	foreign key(category) references movieCategory(id)
);

-- ӰѶ(ʱ��,ӰԺ
create table movieinfo(
	id int primary key ,
	hwhen datetime,			-- ��ӳʱ��
	cinema int,			-- ӰԺ
	movie int,			-- ��Ӱ
	price int,			-- �۸�
	poster varchar(200),		-- ����
	foreign key(cinema) references cinema(id),
	foreign key(movie) references movie(id)
);

-- Ӱ��
create table comment(
	id int primary key ,
	movie int,			-- ��Ӱ
	huser varchar(20),		-- �����û�
	comment varchar(200),		-- ����
	hwhen datetime,			-- ʱ��
	foreign key(movie) references movie(id)
);
