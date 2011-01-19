<?php
$folder="/var/www/files/";
if(isset($_POST['action']) && $_POST["action"]=="upload"){
	if(!is_dir($folder))
		mkdir($folder);
	$filename=$_FILES['file']['name'];
	$tmpname= $_FILES['file']['tmp_name'];
	move_uploaded_file($tmpname, $folder.$filename);
}
else if(!isset($_GET['action'])){
	$name=$folder."diary.db";
	header('Content-Type: application/octet-stream');
	header('Content-disposition: attachment;filename:'.$name);
	if(file_exists($name)){
		$file=fopen($name, "r");
		echo fread($file, filesize($name));
		fclose($file);
	}
}
else{
?>
<form method="post" enctype="multipart/form-data">
<input type="hidden" name="action" value="upload">
<input type="text" name="user">
<input type="file" name="file">
<input type="submit">
</form>
<?php
}
?>
