#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable : 4996)

#define MAXLINE 256;

//Struktur f�r eine Liste
typedef struct lin_list
{
	char *payload;				//User-Daten: Strings
	struct lin_list *next;		//Pointer auf das n�chste Element der Liste
} LinListCell, *p_LinList;

//Prototypen
p_LinList LinListAllocCell(char *payload);

void LinListFreeCell(p_LinList junk);
void LinListFree(p_LinList *junk);

p_LinList LinListInsertFirst(p_LinList *anchor, p_LinList newcell);
p_LinList LinListExtractFirst(p_LinList *anchor);

void LinListRevert(p_LinList *anchor);
void LinListPrint(FILE* out, p_LinList list);

p_LinList LinListFindLast(p_LinList *anchor);
p_LinList LinListInsertLast(p_LinList *anchor, p_LinList newcell);
p_LinList LinListExtractLast(p_LinList *anchor);

p_LinList LinListFind(p_LinList anchor, char* payload);
p_LinList LinListExtract(p_LinList *anchor, p_LinList cell);

//Hauptprogramm
int main(int argc, char *argv[])
{
	FILE *in = stdin;					//File auf Tastatureingabe setzen
	char line[MAXLINE];					//Char-Array f�r die Speicherung der Benutzereingaben
	p_LinList list = NULL;				//Liste erzeugen
	p_LinList temp;						//zweite Liste erzeugen, die nur als Zwischenspeicher genutzt wird
	char* input;						//Pointer f�r das Auslesen der Zeilen der Datei oder der Tastatureingabe

	if (argc == 2)
	{
		in = fopen(argv[1], "r");
		if (!in)
		{
			exit(EXIT_FAILURE);
		}
	}
	while ((input = fgets(line, MAXLINE, in)))
	{
		temp = LinListAllocCell(line);				//Speicher f�r Eingabe reservieren und Wert in Variable eintragen
		LinListInsertFirst(&list, temp);			//aktuelles Element als erstes der Liste einf�gen
	}
	system("Pause");
	exit(EXIT_SUCCESS);
}

//Erzeugt eine Listenzelle, deren Payload eine Kopie von payload ist und gibt einen Pointer auf diese zur�ck
//reine Speicheranforderung und Eintrag in Wert-Variable (Einsortierung in Liste erfolgt durch Insert Funktion)
p_LinList LinListAllocCell(char *payload)
{
	p_LinList p_cell = malloc(sizeof(LinListCell));			//Speicher anfordern f�r neue Zelle der Liste
	p_cell->payload = strdup(payload);						//payload in neue Zelle kopieren
	p_cell->next = NULL;									//vorsorglich n�chstes Element auf null setzen

	return p_cell;											//Pointer auf neue Zelle zur�ckgeben
}

//Gibt eine einzelne Listenzelle und den gespeicherten String an die Speicherverwaltung zur�ck
void LinListFreeCell(p_LinList junk)
{
	free(junk->payload);									//Inhalt der Zelle freigeben
	free(junk);												//Zelle freigeben
}

//Gibt die gesamte Liste mit allen Zellen an die Speicherverwaltung zur�ck und setzt den Listenanker auf NULL
void LinListFree(p_LinList *junk)
{
	p_LinList cell;						//aktuelle Zelle

	while (*junk)
	{
		cell = (*junk)->next;			//speichert die n�chste Zelle im Zellen Pointer
		LinListFreeCell(*junk);			//Aufruf der Freigabe Methode f�r die aktuelle Zelle
		*junk = cell;					//aktuelle Zelle wird an junk Pointer weitergegeben
	}
}

//Erweitert die Liste, die bei *anchor steht um das neue Element newcell (vorne neues Element)
p_LinList LinListInsertFirst(p_LinList *anchor, p_LinList newcell)
{
	newcell->next = *anchor;				//newcell->next zeigt auf den alten Wert von *anchor	
	*anchor = newcell;						//*anchor zeigt danach auf newcell

	return newcell;							//R�ckgabewert ist Pointer auf die gesamte Liste
}


//Das erste Element  aus der Liste herausnehmen, die bei *anchor steht
p_LinList LinListExtractFirst(p_LinList *anchor)
{
	p_LinList extract = *anchor;		//Kopie von erstem Element, das entfernt werden soll

	if (extract)						//Voraussetzung: Die Liste ist nicht leer(sonst: R�ckgabewert NULL)
	{
		*anchor = extract->next;		//*anchor zeigt danach auf das vormals zweite Element der Liste
		extract->next = NULL;			//das Element hat keinen Nachfolger mehr, da es entfernt werden soll
	}
	return extract;						//R�ckgabewert ist Pointer auf das vormals erste Element (Element wird nicht gel�scht!!)
}

//Liste in umgekehrte Reihenfolge bringen
void LinListRevert(p_LinList *anchor)
{
	p_LinList newlist = NULL;				//Pointer f�r Start der neuen Liste (umgekehrte Reihenfolge)
	p_LinList temp;							//speichert aktuelles Element der Liste

	while (*anchor)
	{
		temp = LinListExtractFirst(anchor);			//entferne das erste Element der Liste und gebe den Pointer daf�r zur�ck
		LinListInsertFirst(&newlist, temp);			//einf�gen des ersten Elements der alten Liste in die neue (wird dann das letzte)
	}
	*anchor = newlist;								//neuer Startwert ist die neue Liste (letztes Element der alten Liste)
}

//gesamte Liste ausgeben
void LinListPrint(FILE* out, p_LinList list)
{
	while (list)									//solange es noch ein Element in der Liste gibt
	{
		fprintf(out, "%s", list->payload);			//Ausgabe des Wertes der Liste in Datei oder Bildschirm
		list = list->next;							//n�chstes Element der Liste
	}
}

//finde letztes Elemnt der Liste
p_LinList LinListFindLast(p_LinList *anchor)
{

}

//Hange newcell als letztes Element an die Liste bei *anchor an
//Ansonsten wie LinListInsertFirst()
//Anmerkung : Das implementiert "nebenbei" append!
p_LinList LinListInsertLast(p_LinList *anchor, p_LinList newcell)
{
	p_LinList cell = *anchor;				//aktuelle Zelle
	while (cell)							//solange bis NULL-Pointer gefunden (letztes Element der Liste)
	{
		cell->next = cell;					//Zwischenspeicher des aktuellen Elements in Zelle
	}
	
	cell->next = newcell;					//bisher letztes Element erh�lt als n�chstes Element das neue Element
	newcell->next = NULL;					//letztes Element erh�lt kein n�chstes Element (NULL-Pointer)

	return *anchor;							//R�ckgabewert ist Pointer auf die gesamte Liste (immernoch das erste Element bei anchor)
}

//Entferne das letzte Element der Liste, die bei *anchor steht
//Voraussetzung : Die Liste ist nicht leer(sonst : Ruckgabewert NULL)
//*anchor bleibt unverandert, es sei denn die Liste hat genau ein Element(dann wird *anchor auf NULL gesetzt)
//Ruckgabewert ist Pointer auf das vormals letzte Element
p_LinList LinListExtractLast(p_LinList *anchor)
{
	p_LinList extract = *anchor;		//Kopie von erstem Element, das entfernt werden soll

	if (extract)						//Voraussetzung: Die Liste ist nicht leer(sonst: R�ckgabewert NULL)
	{
		*anchor = extract->next;		//*anchor zeigt danach auf das vormals zweite Element der Liste
		extract->next = NULL;			//das Element hat keinen Nachfolger mehr, da es entfernt werden soll
	}
	return extract;						//R�ckgabewert ist Pointer auf das vormals erste Element (Element wird nicht gel�scht!!)
}

//Finde eine Listenzelle anhand der payload
//Gibt Pointer auf gefundenen Zelle oder NULL zur�ck
p_LinList LinListFind(p_LinList anchor, char* payload)
{

}

//Entferne eine beliebige Zelle aus der Liste und gib sie zur�ck
p_LinList LinListExtract(p_LinList *anchor, p_LinList cell)
{

}

//Sortiere die Liste(ASCIIbetisch, Hinweis: strcmp())
p_LinList LinListSort(p_LinList *anchor)
{

}