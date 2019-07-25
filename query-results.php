<?php
	// Некоторые глобальные пути к файлам.
	$corpusPath = 'Corpus/corpus.xml';
	$cmpIndexPath = 'Corpus/cmpskipindex.dat';
	$posIndexPath = 'Corpus/posindex.dat';
	$TFIDFPath = 'Corpus/tfidf.dat';
	$snippetChars = 600;
	
	$query = $_GET['query'];
	execute('IR7.exe "Corpus/cmpskipindex.dat" "Corpus/posindex.dat" "Corpus/tfidf.dat"', $query);
	
	function makeSnippet($text, $terms) {
		// TODO: всё в нижний регистр, избавиться от надоедливых символов плюс
		// подсветить искомые слова.
		global $snippetChars;
		$len = strlen($text);
		$pos = array();
		$i = 0;
		foreach($terms as $term) {
			$p = strpos($text, $term);
			$pos[$i++] = ($p !== false && $p < $len) ? $p : $len;
		}
		$pos = min($pos);
		//if($pos == $len)
			//$pos = 0;
		//var_dump($pos);
		return substr($text, $pos, min($len - $pos, $snippetChars));
	}
	
	// Функция производит извлечение данных из корпуса документов в XML файле.
	// Возвращает массив пар <заголовок, сниппет>.
	function readXML($docId) {
		// Здесь нужно определить переменную как глобальную иначе будет
		// использована локальная версия пустой строки.
		global $corpusPath;
		global $query;

		// Заранее подготавливаем массив пар <заголовок, сниппет>.
		$docCount = count($docId);
		$result = array_fill(0, $docCount, [0, "", ""]);
		
		// Сперва удаляем все пробелы и служебные символы. Потом удаляем все
		// нулевые слова. Регулярное выражение совместимое с Perl (preg_*).
		$terms = preg_split('/(\s+)|([&\|\/])|([0-9])/', $query);
		$terms = array_filter($terms, 'strlen');
		//var_dump($terms);

		// Создаём XML парсер.
		$xml = new XMLReader();
		$xml->open($corpusPath);

		while($docCount > 0 && $xml->read()) {
			if($xml->nodeType != XMLReader::ELEMENT || $xml->name != 'page')
				continue;

			// Извлекаем из атрибута идентификатор документа и ищем его
			// среди списка идентификаторов поисковой выдачи.
			$id = $xml->getAttribute('id');
			$index = array_search($id, $docId);

			// Проверяем значение индекса и его тип.
			if($index !== false) {
				$title = $xml->getAttribute('title');
				$text = $xml->readString();

				$result[$index][0] = $docId[$index];
				$result[$index][1] = $title;
				$result[$index][2] = makeSnippet($text, $terms);

				--$docCount;
			}
		}
		
		//var_dump($result);
		$xml->close();
		return $result;
	}
	
	function makeLink($pageInfo) {
		echo '<div style="border:1px solid #A9A9A9;">',
			'<a href="open-page.php?pageId=', $pageInfo[0], '">', $pageInfo[1], '</a> [', $pageInfo[0], ']<br>',
			'<br>', $pageInfo[2], '</div><br>';
	}
	
	function execute($exe, $query) {
		$exe_cmd = escapeshellcmd($exe);
		$pipe_desc = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
		$process = proc_open($exe_cmd, $pipe_desc, $pipes);

		// Отправляем поисковой программе запрос и закрываем пайп.
		fputs($pipes[0], $query);
		fclose($pipes[0]);

		// Программа построчно выводит список идентификаторов документов
		// удовлетворяющих поисковому запросу разделённых пробелом, среднюю
		// длину прыжка и потраченное на выполнение запроса время.
		$docId = fgets($pipes[1]);
		$leap = fgets($pipes[1]);
		$msTime = fgets($pipes[1]);
		
		fclose($pipes[1]);
		fclose($pipes[2]);

		$ret_val = proc_close($process);

		// Программа выдаёт список идентификаторов документов в виде строки,
		// поэтому разбиваем строку на элементы массива по пробелу.
		$docId = explode(" ", $docId);
		//var_dump($docId);
		
		// Считаем количество документов и ограничиваемся первыми 50.
		$docCount = count($docId);
		$c = min($docCount, 50);

		// Отбрасываем ненужные документы.
		$docId = array_slice($docId, 0, $c);
		//var_dump($docId);

		echo "<b>Запрос:</b> $query<br>",
			"<b>Средний размер прыжка:</b> $leap<br>",
			"<b>Время поиска (мс):</b> $msTime<br>",
			"<b>Количество документов:</b> $docCount<br>",
			"<br>";

		// Функция читает XML и возвращает для каждого документа его заголовок
		// и сниппет. Далее создаём ссылки.
		$docData = readXML($docId);
		for($i = 0; $i < $c; ++$i)
			makeLink($docData[$i]);
	}
?>
