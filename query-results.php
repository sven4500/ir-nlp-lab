<?php
	$query = $_GET['query'];
	print("<b>Запрос:</b> $query<br>");
	execute('IR4.exe "Corpus/index.dat" "' . $query . '"');
	function execute($exe) {
		$exe_cmd = escapeshellcmd($exe);
		$pipe_desc = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
		$process = proc_open($exe_cmd, $pipe_desc, $pipes);

		//fputs($pipes[0], 'фотосинтез');
		fclose($pipes[0]);

		$result = fgets($pipes[1]);
		fclose($pipes[1]);
		fclose($pipes[2]);

		print("<b>Документы:</b> $result<br>");

		$ret_val = proc_close($process);
		//print("Return: $ret_val<br>");
	}
?>
