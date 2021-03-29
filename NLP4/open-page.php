<?php
	$pageId = $_GET['pageId'];
	$pageId = strval($pageId);

	$xml = new XMLReader();
	$xml->open('Corpus/corpus.xml') or die('No such XML file found');
	
	while($xml->read())
	{
		if($xml->nodeType == XMLReader::ELEMENT /*&& $xml->name == 'page'*/ && $xml->getAttribute('id') == $pageId) {
			
			$xml->moveToElement();
			$elem = new SimpleXMLElement($xml->readOuterXml());
			echo strval($elem);
			break;
		}
	}
	
	$xml->close();
?>
