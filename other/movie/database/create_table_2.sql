-- �û�
create table user(
	id varchar(20) primary key,		-- ID, idΪadmin���ǹ���Ա�˻�
	pass varchar(20) not null,		-- ����
	name varchar(20),				-- ����
	gender char,					-- �Ա�
	birthday datetime,				-- ����
	hobby varchar(200),			-- ����
	phone char(11),				-- �ֻ���
	email varchar(40)				-- Email
);

-- ���� ����û�,��������һ��,�̶���ʱ������
create table reminder(
	id int primary key auto_increment,
	notetype char,				-- �������� ����/�ʼ� S/E
	user varchar(20) not null,		-- �û�
	hwhen long,				-- ����ʱ��
	content varchar(200),			-- ��������	
	foreign key(user) references user(id) on delete cascade
);

-- ֪ͨ �����վ,���µ�Ӱʱ֪ͨ
create table notify(
	id int primary key auto_increment,
	user varchar(20) not null,		-- �û�
	notetype char,				-- �������� ����/�ʼ� S/E
	movie int,				-- ������
	foreign key(user) references user(id) on delete cascade,
	foreign key(movie) references movie(id) on delete cascade
);
