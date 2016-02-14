#include "ILI9341_core.h"
#include "ILI9341_commands.h"
#include "ILI9341_colors.h"
#include "spi_init.h"
#include "delay.h"
#include "FONT/correct_ANSI.h"

uint8_t SPI_work = 0;

// RX
void DMA1_Channel2_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC2) == SET) {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
        TFT_CS_RESET;
        SPI_work = 0;
    }
}

// TX
void DMA1_Channel3_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC3) == SET) {
        DMA_ClearITPendingBit(DMA1_IT_TC3);
    }
}

void pins_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void LCD_DMA_Tx(uint16_t size) {
    DMA_SetCurrDataCounter(DMA1_Channel2, size);
    DMA_SetCurrDataCounter(DMA1_Channel3, size);

    SPI_work = 1;
    TFT_CS_SET;

    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void LCD_DMA_Wait(void) {
    while(SPI_work);
}

void LCD_SendCommand(u8 com) {
    TFT_DC_RESET;       // прижимаем вывод DC LCD
    TFT_CS_RESET;            // тоже с выводом CS, CS это выбор кристалла, используется при нескольких устройствах на одном SPI
    SPI1->DR = com;            // отправляем байт в SPI (на отправку) ! изменить под свой МК,
    TFT_CS_SET;         // отпускаем выбор (кристалла) LCD, (PS. CS - позволяет посадить на одну шину SPI несколько устройств)
}

// *********  Отправка 1 байта ДАННЫХ по 8-bit SPI ************//
void LCD_SendData(u8 data) { // см. выше

    TFT_DC_SET;                    // для понимания см.дефайн
    TFT_CS_RESET;            // так-же см.дефайн
    SPI1->DR = data;        // (!) эти места кода изменить под свой МК, SPI1->DR - запись в регистр SPI, что означает начало отправки
    TFT_CS_SET;
}

// *********  Отправка 2 байт ДАННЫХ по 16-bit SPI ************//
void LCD_SendData_16(uint16_t data) {
    TFT_DC_SET;
    TFT_CS_RESET;
    SPI1->DR = data;        // ! изменить под свой МК,
    TFT_CS_SET;
}

void LCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_SendCommand(ILI9341_COLUMN_ADDR); // для понимания см.дефайн
    LCD_SendData(y1 >> 8);                // "делим" два байта "пополам" и отправляем по одному
    LCD_SendData(y1 & 0xFF);
    LCD_SendData(y2 >> 8);
    LCD_SendData(y2 & 0xFF);


    LCD_SendCommand(ILI9341_PAGE_ADDR); // для понимания см.дефайн
    LCD_SendData(x1 >> 8);
    LCD_SendData(x1 & 0xFF);
    LCD_SendData(x2 >> 8);
    LCD_SendData(x2 & 0xFF);

    LCD_SendCommand(ILI9341_GRAM); // отправляем в память LCD
}

// ********* 16-bit SPI ************//
void spi16(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; //Отключаем SPI1
    SPI1->CR1 |= SPI_CR1_DFF;  //16 бит данных //1
    SPI1->CR1 |= SPI_CR1_SPE;  //Включаем SPI1
}

// *********  8-bit SPI ************//
void spi8(void) {
    SPI1->CR1 &= ~SPI_CR1_SPE; //Отключаем SPI1
    SPI1->CR1 &= ~SPI_CR1_DFF; // 8 бит данных //0
    SPI1->CR1 |= SPI_CR1_SPE;  //Включаем SPI1
}

void LCD_WriteString(unsigned char x0, unsigned int y0, unsigned char *s,
                 unsigned int color)//, unsigned int back_color) заменить, если надо выводить конкретный цвет фона
{
    // пример отправки в эту функцию:
    // WriteString( 1, 5, "Атм. давление" , GREEN);
    // где: 1 - строка,   5 - начало с 5-й точки строки, "собсно текст",  GREEN -  ранее задефайненный цвет, (16-бит) (#define GREEN 0x07E0)
    // если требуется кроме текста передать некие значения, то используем спринтф и передаем через массив, (!) массив (array) надо объявить ранее!
    // sprintf( array,"Атм.давление %u.%ummHg", Press_bmp/10, Press_bmp%10 );  -сперва передаем все в array (ранее объявленный)
    // WriteString( 1, 0, array , GREEN); и теперь из массива выводим строку

    // Даллее, в итоге всех манипуляций ниже в этой функции будем иметь:

    // d  - ширина символа бАйт, (не меняется до окончания строки)
    // he - высота символа, строк
    // wi - вычисленная ширина символа, бит (не меняется до окончания строки)
    // i  - номер начального (первого) байта символа в массиве, ессно меняется вместе с символом (PS.масив всех символов непрерывный )
    // nb - номер конечного (последнего) байта символа в массиве
    // по сути байты от i до nb это байты выводимого в данный момент символа
    // color - цвет символа
    // (здесь не использовано)back_color - цвет фона (можно передать в функцию при желании, немного переделав код)
    // размер стандартного пропуска между 127-м и 192-м по ANSI корректируется в функции "correct_ANSI",
    // т.к. не вижу смысла хранить в памяти МК кучу кракозябр
    // но! всегда присутствует коррекция на первые 32 непечатных символа по ANSI
    // s - поход в массив шрифта (косвенная адр.) ...(*s )
    // z - номер символа в кодировке ANSI, передаваемый в функцию из строки
    // st1 (start) - самый первый действительный (=1) бит слева по ширине символа, т.е. все =0 биты не рисуются,
    // en (end) - тоже справа от символа, используются для поиска фактической ширины конкретного символа
    // k - номер п.порядку символа в строке при поиске
    // q - для цикла, для строки,
    // так-же дальше, для фактической (вычисленной) длины строки (используется повторно)
    // j - очередной байт в строке
    // так-же дальше для отделения первого байта строки от последующих
    // x0 - переданныйИ в эту функцию номер строки, в которую будем писать символы
    // y0 - переданный в эту функц. номер позиции (точки), (!)(т.е. конкретной точки на длине строки)
    // spi16(); или spi8(); используется при возможности переключения в 16-битный SPI, для бОльшей скорости вывода
    // если нет 16-ти бит, то закомментить эти функции и перекомментить шесть строк кода, т.е. сделать выдачу двух байт в два приема

    unsigned char z, y, nb, he, wi, d, j, q, st1, en1, y2;
    unsigned int  x, i, k;
    y2 = y0;            // копируем начальнуюу позицию для случая переноса длинной строки
    d  = FONT_info[0];  // ширина символа, бАйт, берется из файла шрифта
    wi = d * 8;             // вычисляемая ширина символа , бИт
    he = FONT_info[1]; // высота символа, строк, берется из файла шрифта
    nb = (he * d);        //вычисляем кол-во байт на один символ

    for (z = 0; s[z] != '\0'; z++) // перебираем все символы в строке, пока не дойдем до пустышки
    {
        if (s[z] < 128)                                        // если символ латинница, то..
        { i = (s[z] - 32) * nb; }                //корректируем только на первые 32 символа
        else if (s[z] > 191)                            // если символ русский, то..
        {
            i = (s[z] - 32 - (FONT_info[2])) * nb;
        } //пропуск кол-ва симв., между 127-м и 192-м по ANSI		(см.файл шрифта в конце)
        else                                                                // если между рус. и англ, т.е. между 127 и 192, то
        {                                                                    // идем корректировать в файл correct_ANSI.c в соответствии с нашими требованиями
            i = (corr_ANSI(s, z)) *
                nb;                // , т.е. смотря сколько мы выкинули из шрифта всяких символов - кракозябр
        }
        // теперь получаем реальную ширину текущего символа, т.е. вычисляем ширину пустоты слева и справа от символа
        // чтобы отрезать слишком широкие поля, скажем, от маленькой точки (PS. высота символа не меняется)
        x      = i;                         // копируем номер первого байта символа в массиве (указываем байт, с которого начинается символ в массиве)
        st1    = 0;                                               // просто очистка, от результатов предыдущего символа
        en1    = 0;                                               // -*-*-
        for (q = 0; q < he; q++)       // перебираем (проходим) "строки" символа
        {
            for (j = 0, k = 0; j < d; j++)// перебираем все байты строки, зайдя сюда выводим все байты текущей строки,
            {                                                     // PS. "к" - счетчик байт с этой строке, ессно сбрасуется перед новой строкой
                y = 8;                             // счетчик бИт одного байта строки
                while (y--)                         // перебираем и проверяем бит за битом
                {
                    k++;                           // прибавляем счетчик бита в строке
                    if (((FONT[x] & (1 << y)) && (st1 == 0)) ||
                        ((FONT[x] & (1 << y)) && (k < st1))) { st1 = k; } // ищем среди всех строк самый левый бит =1
                    if ((FONT[x] & (1 << y)) &&
                        (en1 < k)) { en1 = k; }                            // ищем среди всех строк самый ПРАВЫЙ бит =1
                }
                x++;                                    // следующий байт этого символа
            }
        }

        if (st1 > 0) { st1--; }                // немного уменьшаем, чтобы символы не "слипались"
        if (en1 < 2) {
            en1 = wi / 3;
        }                // если символ пустой, то это пробел, задаем ему ширину 1/3 от ширины символа
        else if (en1 < wi) {
            en1 = (en1 - st1 + indent);
        } // высчитываем реальную ширину и прибавляем отступ (см. font.h), дабы обеспечить расстояние между симв.

        j = 0;      // обнуляем "отделитель-указатель" что это первый байт в строке
        k = nb + i; // получаем номер последнего (в массиве) байта  этого символа
        q = en1;  // копируем ширину символа, БИТ , en1 остается "хранительницей" ширины этого символа, пока выводятся "строки"


        if ((y0 + en1) > LCD_HEIGHT) {
            y0 = y2;
            x0++;
        }  // если последний символ не вмещается, то переносим на следующую строку (LCD_HEIGHT - см.дефайн)
        // если не нужен перенос, то оставшиеся символы следует "убить" иначе в конце сроки будет клякса
        LCD_SetCursorPosition(x0 * he, y0, (x0 * he) + (he - 1),
                              (y0) + (en1 - 1)); // выделяем поле в памяти LCD, х - вертикаль, у - горизонталь
        y0 = y0 +
             en1;                                                                                                            // указываем у0, где в строке будет начало следующего символа

        //********		закомментировать строку ниже при использовании только 8-битного SPI
        spi16();  // переключаем на 16-битный режим SPI

        for (x = i; x < k; x++) // проходим (в массиве) по очереди все байты текущего символа
        {
            if (j++ == 0) {
                y = 10 - st1;
            } // если это первый байт строки (j=0), то отнимаем пустые биты сначал строки, но прибавляем промежуток 2 pt
            else { y = 8; }               // значит это не первый байт строки
            while (y--)                    // выводим байт строки, т.е. проверяем все биты (?=0 или ?=1) этого байта
            {
                if ((FONT[x] & (1 << y)) != 0)        // бит =1 ? или =0 ?
                { LCD_SendData_16(color); }     // если ДА (=1) пишем цвет  символа (16-bit SPI)

                    //********		закомментировать строку выше и закомм. две ниже при использовании только 8-битного SPI, тоже самое с бэк_колор ниже
                    // LCD_SendData(color >> 8)    // заменяем, если 8-битный SPI
                    // LCD_SendData(color & 0xFF); // и эту строку тоже

                else {
                    LCD_SendData_16(BLACK);
                }     // если НЕТ (=0) пишем цвет	фона  (16-bit SPI) //BLACK <- back_color);} заменить, если надо выводить конкретный цвет фона
                // LCD_SendData(BLACK >> 8)    // заменяем, если 8-битный SPI
                // LCD_SendData(BLACK & 0xFF); // и эту строку тоже

                if (!--q)                      // смотрим сколько действительных бит строки вывели, если уже все (значит конец строки)
                {
                    if (j != d) {
                        x = x + (d - j);
                    } // то.. проверим, вдруг фактическая ширина меньше начальной больше чем на байт, если так, то пропустим ненужные
                    // (пустые байты в массиве) (допустим: фактически точка = 3бит, а на 1 символ отведено 3 байта = 24бит, 2 байт "пустые")
                    y = 0;
                    j = 0;
                    q = en1;    // но в любом случае обнуляем счетчик бит, байт строки и заносим ширину строки
                }
            }
        }
        //********		закомментировать строку ниже при использовании только 8-битного SPI
        spi8(); // переключаем на 8-битный режим SPI

    }
}


void LCD_Fill(uint16_t color) { // просто заливка одним цветом

    uint32_t n = LCD_PIXEL_COUNT; // вносим в счетчик количество точек дисплея (см.дефайн LCD_PIXEL_COUNT)

    LCD_SetCursorPosition(0, 0, LCD_WIDTH - 1, LCD_HEIGHT -
                                               1); // указываем поле под заливку от нуля до краев (см.дефайн LCD_WIDTH и  LCD_HEIGHT)

    LCD_SendCommand(ILI9341_GRAM); // вносим в память (ram)  LCD

    spi16();  // для 16-бит SPI
    while (n) {

        if (SPI1->SR | SPI_SR_TXE) {
            n--;
            LCD_SendData_16(color); // для 16-бит SPI
            //  LCD_SendData( color >> 8); // раскомментить обе строки для 8-бит SPI
            //	LCD_SendData( color & 0xFF);
        }

    }
    spi8();  // для 16-бит SPI, возврат на 8-бит
}


// (страница 127 даташит)
// для выбора ставим где надо 1 и не забываем поставить 0 вместо 1, где не надо ;-)
#define Dspl_Rotation_0_degr   0 // поворот дисплея, сколько градусов 01001000 портрет
#define Dspl_Rotation_90_degr  1 // поворот дисплея, сколько градусов 00101000 альбом
#define Dspl_Rotation_180_degr 0 // поворот дисплея, сколько градусов 10001000 портрет, но снизу вверх
#define Dspl_Rotation_270_degr 0 // поворот дисплея, сколько градусов 11101000

//стр.155 даташит
#define Fram_Rate_61Hz  0 // частота кадров (70Hz по умолч)
#define Fram_Rate_70Hz  0 // частота кадров
#define Fram_Rate_90Hz  0 // частота кадров
#define Fram_Rate_100Hz 0 // частота кадров
#define Fram_Rate_119Hz 1 // частота кадров

void LCD_Init(void) {
    pins_init();
    SPI_init_master();
    delay_ms(1);
    // сброс дисплея
    TFT_RST_SET;
    LCD_SendCommand(ILI9341_RESET);
    delay_ms(100);

    /// комманды и данные
    LCD_SendCommand(ILI9341_POWERA);
    LCD_SendData(0x39);
    LCD_SendData(0x2C);
    LCD_SendData(0x00);
    LCD_SendData(0x34);
    LCD_SendData(0x02);
    LCD_SendCommand(ILI9341_POWERB);
    LCD_SendData(0x00);
    LCD_SendData(0xC1);
    LCD_SendData(0x30);
    LCD_SendCommand(ILI9341_DTCA);
    LCD_SendData(0x85);
    LCD_SendData(0x00);
    LCD_SendData(0x78);
    LCD_SendCommand(ILI9341_DTCB);
    LCD_SendData(0x00);
    LCD_SendData(0x00);
    LCD_SendCommand(ILI9341_POWER_SEQ);
    LCD_SendData(0x64);
    LCD_SendData(0x03);
    LCD_SendData(0x12);
    LCD_SendData(0x81);
    LCD_SendCommand(ILI9341_PRC);
    LCD_SendData(0x20);
    LCD_SendCommand(ILI9341_POWER1);
    LCD_SendData(0x23);
    LCD_SendCommand(ILI9341_POWER2);
    LCD_SendData(0x10);
    LCD_SendCommand(ILI9341_VCOM1);
    LCD_SendData(0x3E);
    LCD_SendData(0x28);
    LCD_SendCommand(ILI9341_VCOM2);
    LCD_SendData(0x86);
    LCD_SendCommand(ILI9341_MAC);


#if Dspl_Rotation_0_degr
    LCD_SendData(0x48); //0градусов
#elif Dspl_Rotation_90_degr
    LCD_SendData(0x28); //90градусов
#elif Dspl_Rotation_180_degr
    LCD_SendData(0x88); //180градусов
#elif Dspl_Rotation_270_degr
    LCD_SendData(0xE8); //270градусов
#endif

    LCD_SendCommand(ILI9341_FRC);
    LCD_SendData(0);

#if Fram_Rate_70Hz
    LCD_SendData(0x1F);
#elif Fram_Rate_70Hz
    LCD_SendData(0x1B);
#elif Fram_Rate_90Hz
    LCD_SendData(0x15);
#elif Fram_Rate_100Hz
    LCD_SendData(0x13);
#elif Fram_Rate_119Hz
    LCD_SendData(0x10);
#endif


    LCD_SendCommand(ILI9341_PIXEL_FORMAT);
    LCD_SendData(0x55);
    LCD_SendCommand(ILI9341_FRC);
    LCD_SendData(0x00);
    LCD_SendData(0x18);
    LCD_SendCommand(ILI9341_DFC);
    LCD_SendData(0x08);
    LCD_SendData(0x82);
    LCD_SendData(0x27);
    LCD_SendCommand(ILI9341_3GAMMA_EN);
    LCD_SendData(0x00);
    LCD_SendCommand(ILI9341_COLUMN_ADDR);
    LCD_SendData(0x00);
    LCD_SendData(0x00);
    LCD_SendData(0x00);
    LCD_SendData(0xEF);
    LCD_SendCommand(ILI9341_PAGE_ADDR);
    LCD_SendData(0x00);
    LCD_SendData(0x00);
    LCD_SendData(0x01);
    LCD_SendData(0x3F);
    LCD_SendCommand(ILI9341_GAMMA);
    LCD_SendData(0x01);
    LCD_SendCommand(ILI9341_PGAMMA);
    LCD_SendData(0x0F);
    LCD_SendData(0x31);
    LCD_SendData(0x2B);
    LCD_SendData(0x0C);
    LCD_SendData(0x0E);
    LCD_SendData(0x08);
    LCD_SendData(0x4E);
    LCD_SendData(0xF1);
    LCD_SendData(0x37);
    LCD_SendData(0x07);
    LCD_SendData(0x10);
    LCD_SendData(0x03);
    LCD_SendData(0x0E);
    LCD_SendData(0x09);
    LCD_SendData(0x00);
    LCD_SendCommand(ILI9341_NGAMMA);
    LCD_SendData(0x00);
    LCD_SendData(0x0E);
    LCD_SendData(0x14);
    LCD_SendData(0x03);
    LCD_SendData(0x11);
    LCD_SendData(0x07);
    LCD_SendData(0x31);
    LCD_SendData(0xC1);
    LCD_SendData(0x48);
    LCD_SendData(0x08);
    LCD_SendData(0x0F);
    LCD_SendData(0x0C);
    LCD_SendData(0x31);
    LCD_SendData(0x36);
    LCD_SendData(0x0F);
    LCD_SendCommand(ILI9341_SLEEP_OUT);

    delay_ms(100);
    LCD_SendCommand(ILI9341_DISPLAY_ON);
    LCD_SendCommand(ILI9341_GRAM);
}

