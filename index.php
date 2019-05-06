<?php
	execute('IR4.exe');
	function execute($exe) {
		$exe_cmd = escapeshellcmd($exe); // Экранируем некоторые символы
		$pipe_desc = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
		$process = proc_open($exe_cmd, $pipe_desc, $pipes);

		fputs($pipes[0], 'фотосинтез');
		fclose($pipes[0]);

		$s = fgets($pipes[1]);
		fclose($pipes[1]);
		fclose($pipes[2]);

		echo("Return: $s<br>");

		$ret_val = proc_close($process);
		echo("Return: $ret_val<br>");
	}
?>
