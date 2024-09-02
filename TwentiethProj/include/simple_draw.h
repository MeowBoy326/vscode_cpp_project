/*
	simple_draw.h - Headerdatei f�r die Zeichenfunktionen zu "Simple DDE Draw"
	Autor: Martin Gr�fe, martin.e.graefe@iem.thm.de
*/

#ifdef __cplusplus
extern "C" {
#endif

	/******************************************************
		L�schen der Grafik
		keine Parameter, kein R�ckgabewert
	*******************************************************/
	void ClearGraphic(void);


	/******************************************************
		Grafik-Cursor positionieren
		Parameter: x, y
	*******************************************************/
	void MoveTo(int x, int y);


	/******************************************************
		Linie von Grafik-Cursor aus zu (x,y) ziehen
		Parameter: x, y
	*******************************************************/
	void DrawTo(int x, int y);


	/******************************************************
		Linie zeichnen
		Parameter: x1, y1, x2, y2
	*******************************************************/
	void DrawLine(int x1, int y1, int x2, int y2);


	/******************************************************
		Farbe und Strichst�rke einstellen
		Parameter: Rot, Gr�n, Blau, Breite
	*******************************************************/
	void SetPen(int r, int g, int b, int w);


	/******************************************************
		Einzelnen Pixel setzen
		Parameter: x, y, Rot, Gr�n, Blau
	*******************************************************/
	void DrawPixel(int x, int y, int r, int g, int b);


	/******************************************************
		Text zeichnen
		Parameter: x, y, Text (UTF-8, max. 40 Bytes)
	*******************************************************/
	void PlaceText(int x, int y, char *text);


	/******************************************************
		Fenstergr��e �ndern / Fenster verschieben
		Parameter: Position x, Position y, Breite, H�he
	*******************************************************/
	void ResizeGraphic(int x, int y, int width, int height);


	/******************************************************
		Fenster automatisch im Vordergrund ein-/ausschalten
		Parameter: on/off
	*******************************************************/
	void GraphicToFront(int on_off);


	/******************************************************
		X-Koordinate des Cursors (Maus) auslesen
		keine Parameter
		R�ckgabewert(int): x-Koordinate des Cursors im Fenster
	*******************************************************/
	int GetMouseX(void);


	/******************************************************
		Y-Koordinate des Cursors auslesen
		keine Parameter
		R�ckgabewert(int): y-Koordinate des Cursors im Fenster
	*******************************************************/
	int GetMouseY(void);


	/******************************************************
		Mausklick abfragen
		keine Parameter
		R�ckgabewert: Bit 0: Maustaste ist momentan gedr�ckt
					  Bit 1: Maustaste wurde seit letzter
							 Abfrage gedr�ckt
	******************************************************/
	int GetMouseButton(void);

	/******************************************************
		Tastatur abfragen
		keine Parameter
		R�ckgabewert(int): liefert 1, wenn eine Taste
		gedr�ckt wurde, sonst 0
	*******************************************************/
	int KeyPressed(void);

	/******************************************************
		Tastatur abfragen
		keine Parameter
		R�ckgabewert(char): liefert den ASCII-Code der
		gedr�ckten Taste bzw. 0, wenn keine Taste gedr�ckt ist
	*******************************************************/
	int GetKey(void);

	/******************************************************
		Verz�gerung
		Parameter: Zeit in Millisekunden
	*******************************************************/
	void Delay(int milliseconds);

#ifdef __cplusplus
}
#endif
