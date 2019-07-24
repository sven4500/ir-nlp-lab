<?php
	// Некоторые глобальные пути к файлам.
	$corpusPath = 'Corpus/corpus.xml';
	$cmpIndexPath = 'Corpus/cmpskipindex.dat';
	$posIndexPath = 'Corpus/posindex.dat';
	$TFIDFPath = 'Corpus/tfidf.dat';
	
	$query = $_GET['query'];
	execute('IR7.exe "Corpus/cmpskipindex.dat" "Corpus/posindex.dat" "Corpus/tfidf.dat"', $query);
	
	// Функция производит извлечение данных из корпуса документов в XML файле.
	// Возвращает массив пар <заголовок, сниппет>.
	function readXML($docId) {
		// Здесь нужно определить переменную как глобальную иначе будет
		// использована локальная версия пустой строки.
		global $corpusPath;
		
		// Заранее подготавливаем массив пар <заголовок, сниппет>.
		$docCount = count($docId);
		$result = array_fill(0, $docCount, [0, "", ""]);
		
		//var_dump($docId, '<br><br>');
		//var_dump($result, '<br><br>');
		
		// Создаём XML парсер.
		$xml = new XMLReader();
		$xml->open($corpusPath);
		
		// Находим первый элемент page.
		//while($xml->read() && $xml->name != 'page');

		while($docCount > 0 && $xml->read()) {
			if(/*$xml->nodeType != XMLReader::ELEMENT ||*/ $xml->name != 'page')
				continue;
			
			//$xml->moveToElement();
			//$node = new SimpleXMLElement($xml->readOuterXML());
			//$id = $node->attributes()->id;
			
			// Извлекаем из атрибута идентификатор документа и ищем его
			// среди списка идентификаторов поисковой выдачи.
			$id = $xml->getAttribute('id');
			$index = array_search($id, $docId);

			if($index !== false) {
				$title = $xml->getAttribute('title');
				//$text = $xml->readString();

				$result[$index][0] = $docId[$index];
				$result[$index][1] = $title;
				$result[$index][2] = 'Очень скоро здесь будет сниппет...';
				
				// Удаляем найденный идентификатор документа из массива и
				// уменьшаем количество документов которые осталось найти.
				//unset($docId[$index]);
				--$docCount;
			}

			$xml->next();
		}
		
		//var_dump($result);
		$xml->close();
		return $result;
	}
	
	function makeLink($pageInfo) {
		echo '<div style="border:1px solid #A9A9A9;">';
		echo '<a href="open-page.php?pageId=', $pageInfo[0], '">', $pageInfo[1], '</a> [', $pageInfo[0], ']<br>';
		echo '<br>', $pageInfo[2], '</div><br>';
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
		$result = fgets($pipes[1]);
		$leap = fgets($pipes[1]);
		$msTime = fgets($pipes[1]);
		
		fclose($pipes[1]);
		fclose($pipes[2]);

		$ret_val = proc_close($process);

		// Программа выдаёт список идентификаторов документов в виде строки,
		// поэтому разбиваем строку на элементы массива по пробелу.
		$result = explode(" ", $result);
		//var_dump($result, '<br><br>');
		
		// Считаем количество документов и ограничиваемся первыми 50.
		$docCount = count($result);
		$c = min($docCount, 50);

		// Отбрасываем ненужные документы.
		$result = array_slice($result, 0, $c);
		//var_dump($result, '<br><br>');

		echo "<b>Запрос:</b> $query<br>";
		echo "<b>Средний размер прыжка:</b> $leap<br>";
		echo "<b>Время поиска (мс):</b> $msTime<br>";
		echo "<b>Количество документов:</b> $docCount<br>";
		echo "<br>";

		$docData = readXML($result);
		for($i = 0; $i < $c; ++$i)
			makeLink($docData[$i]);
	}
?>
