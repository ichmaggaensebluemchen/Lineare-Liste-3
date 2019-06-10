#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable : 4996)

#define MAXLINE 256

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

p_LinList LinListInsertLast(p_LinList *anchor, p_LinList newcell);
p_LinList LinListExtractLast(p_LinList *anchor);

p_LinList LinListFind(p_LinList *anchor, char* payload);
p_LinList LinListExtract(p_LinList *anchor, p_LinList cell);

p_LinList LinListSort(p_LinList *anchor);

//Hauptprogramm
//Verwenden Sie diese, um ein Programm zu schreiben, dass
//beliebig viele Strings(bis 255 Zeichen Lange) einliest
//alle Duplikate verwirft oder loscht(d.h.nur die erste Kopie behalt)
//die Liste danach in Originalreihenfolge wieder ausgibt
int main(int argc, char *argv[])
{
	FILE *in = stdin;					//File auf Tastatureingabe setzen
	char line[MAXLINE];					//Char-Array f�r die Speicherung der Benutzereingaben
	p_LinList list = NULL;				//Liste erzeugen
	p_LinList temp;						//zweite Liste erzeugen, die nur als Zwischenspeicher genutzt wird
	p_LinList sortedList;
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
		if (!LinListFind(&list, line) || (list == NULL))
		{
			temp = LinListAllocCell(line);					//Speicher f�r Eingabe reservieren und Wert in Variable eintragen
			LinListInsertLast(&list, temp);					//aktuelles Element als letztes der Liste einf�gen
		}
	}
	sortedList = LinListSort(&list);
	printf("-------------------------------------------\n");
	printf("Sorted List\n");
	printf("-------------------------------------------\n");
	LinListPrint(stdout, sortedList);
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
	newcell->next = anchor;				//newcell->next zeigt auf den alten Wert von *anchor	
	anchor = newcell;						//*anchor zeigt danach auf newcell

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

//H�nge newcell als letztes Element an die Liste bei *anchor an
//Ansonsten wie LinListInsertFirst()
//Anmerkung : Das implementiert "nebenbei" append!
p_LinList LinListInsertLast(p_LinList *anchor, p_LinList newcell)
{
	//Spezialfall in Liste ist leer
	if (*anchor == NULL)
	{
		*anchor = newcell;
		newcell->next = NULL;
		return *anchor;
	}

	//zuerst Suche des letzten Elements
	p_LinList cell = *anchor;				//aktuelle Zelle zum Beginn der Suche

	while (cell->next != NULL)				//solange cell Pointer ungleich NULL
	{
		cell = cell->next;					//springe zur n�chsten Zelle
	}

	//neues Element an Ende der Liste anf�gen
	cell->next = newcell;					//bisher letztes Element erh�lt als n�chstes Element das neue Element
	newcell->next = NULL;					//letztes Element erh�lt kein n�chstes Element (NULL-Pointer)

	return *anchor;							//R�ckgabewert ist Pointer auf die gesamte Liste (immernoch das erste Element bei anchor)
}

//Entferne das letzte Element der Liste, die bei *anchor steht
//*anchor bleibt unver�ndert, es sei denn die Liste hat genau ein Element(dann wird *anchor auf NULL gesetzt)
p_LinList LinListExtractLast(p_LinList *anchor)
{
	p_LinList cell = *anchor;			//aktuelle Zelle wird Beginn der Liste
	p_LinList extract = *anchor;		//Nachfolger der aktuellen Zelle und potenzielles letztes Elemnt, das entfernt werden soll

	while (cell)						//solange Liste nicht leer
	{
		if (cell->next != NULL)			//�berpr�fung, ob n�chstes Element der Liste das letzte ist
		{
			extract = cell->next;

			if (extract->next == NULL)
			{									//dann ist cell die vorletzte Zelle und nextcell die letzte
				cell->next = NULL;				//Vorletzte Zelle wird die letzte Zelle der Liste
				return extract;					//R�ckgabewert ist Pointer auf das vormals letzte Element (= zu l�schende Element) es ist noch nicht gel�scht!
			}
		}
		else                                    //anchor ist erstes und letztes Element
		{
			return anchor;
		}
		cell = cell->next;						//zur n�chsten Zelle springen
	}
}

//Finde eine Listenzelle anhand der payload
//Gibt Pointer auf gefundenen Zelle oder NULL zur�ck
p_LinList LinListFind(p_LinList *anchor, char* payload)
{
	p_LinList list = *anchor;						//Start der Liste
	while (list)									//solange es noch ein Element in der Liste gibt
	{
		if (strcmp(list->payload, payload) == NULL)	//falls das payload des aktuellen Listenelements gleich dem payload ist
		{
			return list;							//R�ckgabe: aktuelles Element der Liste mit entsprechend gleichem payload
		}
		else
		{
			list = list->next;						//ansonsten springe zum n�chsten Element der Liste
		}
	}
	//wird nur erreicht, wenn beim letzten Element angekommen und payload nicht gefunden oder Lliste leer ist
	return NULL;									//R�ckgabe NULL-Pointer
}

//Entferne eine beliebige Zelle aus der Liste und gib sie zur�ck
p_LinList LinListExtract(p_LinList *anchor, p_LinList cell)
{
	//suche des Vorg�ngers von cell
	p_LinList temp = *anchor;

	if (temp == cell)				//�berpr�fung, ob cell das erste Element der Liste ist
	{
		return LinListExtractFirst(anchor);
	}

	if (cell->next == NULL)			//�berpr�fung, ob cell das letzte Element der Liste ist
	{
		return LinListExtractLast(anchor);
	}

	while (temp)
	{
		if (temp->next == cell)			//�bepr�fen, ob temp der Vorg�nger von cell ist
		{
			temp->next = cell->next;	//Vorg�nger erh�lt als Nachfolger den Nachfolger von cell
			cell->next = NULL;
			return cell;
		}
		temp = temp->next;
	}
}

//Sortiere die Liste(ASCIIbetisch, Hinweis: strcmp())
p_LinList LinListSort(p_LinList *anchor)
{
	p_LinList list = *anchor;
	p_LinList temp = *anchor;
	p_LinList newList = NULL;
	p_LinList newAnchor = NULL;
	while (temp)													//solange noch Elemente unsortiert sind
	{
		while (list)											
		{
			if (strcmp(list->payload, temp->payload) < 0)			//list payload kleiner als temp payload
			{
				temp = list;										//kleinstes Element, sobald alle Listenelemente durchlaufen
				list = list->next;									//Sprung zum n�chsten Element
			}
			else
			{
				list = list->next;									//Sprung zum n�chsten Element
			}
		}
		temp = LinListExtract(anchor, temp);						//entfernen, des kleinesten Elements der Liste
		newAnchor = LinListInsertLast(&newAnchor, temp);			//kleinstes Element in neue Liste am Anfang einf�gen
		list = *anchor;												//Liste auf Start setzen
		temp = *anchor;												//temp auf Start setzen
	}
	return newAnchor;												//R�ckgabe auf Start der neuen, sortierten Liste
}