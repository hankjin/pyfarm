-- �������ݿ�
drop database if exists movie;
create database movie;
-- �����û�
grant all privileges on movie.* to movie@'%' identified by 'xxxx';
flush privileges;
-- ���Ա�
use movie;
create table test(
	id int primary key auto_increment,
	name varchar(20),
	pass varchar(20)
);