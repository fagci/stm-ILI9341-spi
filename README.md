# STM32 ILI9341 spi

[![STM32 ILI9341 spi](http://img.youtube.com/vi/-2-tuqgc90k/0.jpg)](http://www.youtube.com/watch?v=-2-tuqgc90k "STM32 ILI9341 spi")

Библиотека для работы с дисплеем ILI9341 по SPI с использованием прямого доступа к памяти.
Рисование примитивов + текста портировано с ардуиновской библиотеки от Adafruit с применением некоторй оптимизации.

Пины настраиваются в файле config.h в папке с библиотекой

Проект для CLion IDE.
Используется стандартная библиотека для работы с перифирией.
Для успешной компиляции править пути к библиотеке в CMakeLists.txt

---

This project for IntelliJ IDEA CLion includes fast DMA graphics library.
Testing on STM32f103C8T6.

Graphics drawing ported from Adafruit library for Arduino.
Some optimizations applied.

Pins configuration in config.h
