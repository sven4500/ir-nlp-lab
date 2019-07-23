<?php
	$query = $_GET['query'];
	execute('IR7.exe "Corpus/cmpskipindex.dat" "Corpus/posindex.dat" "Corpus/tfidf.dat"', $query);
	
	function getTitle($pageId) {
		return $pageId;
	}
	
	function getSnippet($pageId) {
		return $pageId;
	}
	
	function makeLink($pageId) {
		$title = getTitle($pageId);
		$snippet = getSnippet($pageId);
		echo '<div style="border:1px solid #A9A9A9;">';
		echo '<a href="open-page.php?pageId=', $pageId, '">', $title ,'</a><br>';
		echo '<br>', $snippet, '</div><br>';
	}
	
	function execute($exe, $query) {
		$exe_cmd = escapeshellcmd($exe);
		$pipe_desc = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
		$process = proc_open($exe_cmd, $pipe_desc, $pipes);

		fputs($pipes[0], $query);
		fclose($pipes[0]);

		$result = fgets($pipes[1]);
		$leap = fgets($pipes[1]);
		$msTime = fgets($pipes[1]);
		fclose($pipes[1]);
		fclose($pipes[2]);

		$ret_val = proc_close($process);

		// Программа выжаёт список идентификаторов документов в виде строки,
		// поэтому разбиваем строку на элементы массива по пробелу.
		$result = explode(" ", $result);
		
		// Считаем количество документов.
		$docCount = count($result);
		$c = min($docCount, 50);

		// Отбрасываем ненужные документы.
		$result = array_slice($result, 0, $c);

		echo "<b>Запрос:</b> $query<br>";
		echo "<b>Средний размер прыжка:</b> $leap<br>";
		echo "<b>Время поиска (мс):</b> $msTime<br>";
		echo "<b>Количество документов:</b> $docCount<br>";
		echo "<br>";

		for($i = 0; $i < $c; ++$i)
			makeLink($result[$i]);
	}
?>
