<?php
	$query = $_GET['query'];
	execute('NLP3.exe "Corpus/normindex.dat" "Corpus/posindex.dat" "Corpus/tfidf.dat"', $query);
	
	function makeLink($pageId) {
		echo '<a href="open-page.php?pageId=', $pageId, '">', $pageId ,'</a><br>';
	}
	
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

		$ret_val = proc_close($process);
		
		echo "<b>Запрос:</b> $query<br>";
		echo "<b>Время поиска (мс):</b> $msTime<br>";
		
		$result = explode(" ", $result);
		$c = min(count($result), 50);
		for($i = 0; $i < $c; ++$i)
			makeLink($result[$i]);
	}
?>
