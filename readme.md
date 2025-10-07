# WEMOS D1 Mini V4.0.0

https://sl.aliexpress.ru/p?key=EXsWVSI

Тестоый проект для изучения возможностей платы

- Выбор типа платы: выбрал "d1_mini_lite", но должно подойти и "LOLIN(WEMOS) R2&mini". PS: "d1_mini" тоже работает
- WeMos D1 Mini Pinout <br><img src="docs\Wemos_Lolin_D1_mini_V4.0_Pinout.webp" alt="WeMos D1 Mini Pinout" width="500">
- Размеры модуля <br><img src="docs\Wemos D1 mini v.4.0.0 - sizes.webp" alt="WeMos D1 Mini sizes" width="300">
- Для работы с таймером используется GyverTimer для асинхронного неблокирующего изменения состояний
- lib/led - класс для простого моргания светодиодом без блокирования основного цикла
- Board info
> - Chip is ESP8266EX
> - Features: WiFi
> - Crystal is 26MHz
> - MAC: **F4:CF:A2:78:DF:F9**
- Morse - учебный класс для моргания светодиодам чисел азбукой Морзе без блокирования основного цикла на таймере AlexGyver. 