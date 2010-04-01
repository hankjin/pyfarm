<?php
/////////////////////////////////////////////
// Get fetion_home via env: FETIONROBOT_HOME
/////////////////////////////////////////////
function get_fetion_home()
{
	return getenv("FETIONROBOT_HOME");	
}

/////////////////////////////////////////////
// Get fetion service port via config file
/////////////////////////////////////////////
function get_fetion_svc_port($fetion_home, $mobile_no)
{
	$conf_file = $fetion_home.DIRECTORY_SEPARATOR;
	$conf_file.= 'conf'.DIRECTORY_SEPARATOR.'svc_port_'.$mobile_no.'.conf';
	return @file_get_contents($conf_file);
}

////////////////////////////////////////////
// get fetion_home and service port
////////////////////////////////////////////
$mobile_no = '';
if($mobile_no == '' )
{
	echo "Please set mobile_no to fetion mobile account before running this demo.";
	exit(0);
}

$fetion_home = get_fetion_home();
if(!$fetion_home)
{
	echo "Please set envionment: FETIONROBOT_HOME to fetion robot path\n";
	exit(0);
}
$svc_port = get_fetion_svc_port($fetion_home, $mobile_no);
if(!$svc_port)
{
	echo "Can't open service config file\n";
	exit(0);	
}
////////////////////////////////////////////

$command = "sms 0 hello,world";
$fetion_svc_socket = socket_create(AF_INET,SOCK_DGRAM,0);
@socket_sendto($fetion_svc_socket,$command,strlen($command),0,'127.0.0.1', $svc_port);
socket_close($fetion_svc_socket);
?>