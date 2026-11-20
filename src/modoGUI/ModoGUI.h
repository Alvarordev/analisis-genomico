#ifndef MODO_GUI_H
#define MODO_GUI_H

// Modo "./genomica --gui": gui/app.py envía un comando por línea (partes
// separadas por tabulaciones) y se responde una línea JSON por comando.
//   cargar<TAB>ruta | tabla1 | buscar<TAB>id | kmers<TAB>id<TAB>k | tabla2
//   kmer<TAB>texto | top<TAB>n | grafo<TAB>beta<TAB>id o secuencia | salir
void ejecutarModoGUI();

#endif
