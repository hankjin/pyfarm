-- 创建数据库
drop database if exists movie;
create database movie;
-- 创建用户
grant all privileges on movie.* to movie@'%' identified by 'xxxx';
flush privileges;
-- 测试表
use movie;
create table test(
	id int primary key auto_increment,
	name varchar(20),
	pass varchar(20)
);