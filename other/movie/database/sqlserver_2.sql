-- �û�
create table huser(
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
	id int primary key ,
	notetype char,				-- �������� ����/�ʼ� S/E
	huser varchar(20) not null,		-- �û�
	hwhen int,				-- ����ʱ��
	content varchar(200),			-- ��������	
	foreign key(huser) references huser(id) on delete cascade
);

-- ֪ͨ �����վ,���µ�Ӱʱ֪ͨ
create table notify(
	id int primary key ,
	huser varchar(20) not null,		-- �û�
	notetype char,				-- �������� ����/�ʼ� S/E
	movie int,				-- ������
	foreign key(huser) references huser(id) on delete cascade,
	foreign key(movie) references movie(id) on delete cascade
);
