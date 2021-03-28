<?php
	$query = $_GET['query'];
	print("<b>Запрос:</b> $query<br>");
	execute('IR5.exe "Corpus/index.dat" "Corpus/posindex.dat"', $query);
	function execute($exe, $query) {
		$exe_cmd = escapeshellcmd($exe);
		$pipe_desc = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
		$process = proc_open($exe_cmd, $pipe_desc, $pipes);

		fputs($pipes[0], $query);
		fclose($pipes[0]);

		$result = fgets($pipes[1]);
		$msTime = fgets($pipes[1]);
		fclose($pipes[1]);
		fclose($pipes[2]);

		print("<b>Документы:</b> $result<br>");
		print("<b>Время поиска (мс):</b> $msTime<br>");

		$ret_val = proc_close($process);
		print("<b>Ошибка:</b> $ret_val<br>");
	}
?>
