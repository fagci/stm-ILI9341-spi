#ifndef FONT_18_PTC_H
#define FONT_18_PTC_H
#include "stm32f10x.h"  

//#define FONT      cousine_18ptcBitmaps // прописать нужный шрифт
//#define FONT_info cousine_18ptcFontInfo // описание шрифта, размер и т.д.

#define FONT      cousine_16ptcBitmaps  // прописать нужный шрифт
#define FONT_info cousine_16ptcFontInfo // описание шрифта, размер и т.д. см.в конце файла шрифта
#define indent 5   // точек (px)        // сколько точек остается поле после символа (отступ справа от символа до следующего символа)

//extern const u8 cousine_18ptcBitmaps[];
//extern const u8 cousine_18ptcFontInfo[];	

extern const u8 cousine_16ptcBitmaps[];
extern const u8 cousine_16ptcFontInfo[];	



/*  
// wi ширина символа, бит */
// he количество строк  */
// skip пропуск количества симв., между 127-м и 192-м по ANSI */


#endif /* FONT_18_PTC_H */

