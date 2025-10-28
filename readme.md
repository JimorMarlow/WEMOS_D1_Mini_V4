##### Dependency: `ETL` (Embedded Template Library)
![Version](https://img.shields.io/github/package-json/v/JimorMarlow/WEMOS_D1_Mini_V4)

# WEMOS D1 Mini V4.0.0 

https://sl.aliexpress.ru/p?key=EXsWVSI

Тестоый проект для изучения возможностей платы

- Выбор типа платы: выбрал "d1_mini_lite", но должно подойти и "LOLIN(WEMOS) R2&mini". PS: "d1_mini" тоже работает
- WeMos D1 Mini Pinout <br><img src="docs\Wemos_Lolin_D1_mini_V4.0_Pinout.webp" alt="WeMos D1 Mini Pinout" width="500">
- Размеры модуля <br><img src="docs\Wemos D1 mini v.4.0.0 - sizes.webp" alt="WeMos D1 Mini sizes" width="300">
- Для работы с таймером используется GyverTimer для асинхронного неблокирующего изменения состояний. upd: Перешел на более новую версию [GTimer](https://github.com/GyverLibs/GTimer)
- lib/led - класс для простого моргания светодиодом без блокирования основного цикла
- Board info
> - Chip is ESP8266EX
> - Features: WiFi
> - Crystal is 26MHz
> - MAC: **F4:CF:A2:78:DF:F9**
- Morse - учебный класс для моргания светодиодам чисел азбукой Морзе без блокирования основного цикла на таймере AlexGyver. 

# Добавил поддержку платы ESP32 C3 Super Mini

- ESP32C3 PRO MINI development board onboard ESP32-C3FH4 chip module WiFi Bluetooth development board
- https://ali.click/2n96l7
- Внешний вид <br><img src="docs\ESP32C3 PRO MINI.png" alt="ESP32C3 PRO MINI" width="500">
- Pinout <br><img src="docs\ESP32C3 PRO MINI - pinout.png" alt="ESP32C3 PRO MINI" width="500">

Вывод в терминал заработал только после этой конфигурации:

    [env:esp32c3]
    board = esp32-c3-devkitm-1
    platform = espressif32
    framework = arduino
    monitor_speed = 115200
    build_flags =    
        -D ARDUINO_USB_MODE=1
	    -D ARDUINO_USB_CDC_ON_BOOT=1 

### Чтобы вывод в термиал заработал сразу, нужно сдетаь паузу на 1с
	Serial.begin(115200);
    #ifdef ESP32
    delay(1000);  // для ESP32 C3 supermini нуждо сделать задержку, чтобы выводилась отладочная информация
    #endif

### Если не удается загрузить прошивку:
Для включения загрузки через usb нужно зажать BOOT, потом RESET, отпускаем RESET, потом отпускаем BOOT, после чего плата начинает прошиваться 

### Определение значений во флагах для всех конфигураций
в platformio.ini добавить разделы, и в нужных расширить

    [env]
    build_flags = 
    '-DWIFI_SSID="mywifissid"'
    '-DWIFI_PASS="mypass"'

    [env:d1_mini_lite]
    ...
    build_flags = 
    ${env.build_flags}
    '-DMODE="server"'

    [env:esp32c3]
    ...
    build_flags =    
    ${env.build_flags}
    '-DMODE="client"'

В основном коде, н-р в main.cpp можно вывести:

    Serial.println("-----------WIFI----------");
    Serial.print("SSID: ");  Serial.println(WIFI_SSID);
    Serial.print("PASS: ");  Serial.println(WIFI_PASS);
    Serial.print("MODE: ");  Serial.println(MODE);
    Serial.println("-------------------------");

Output:

    -----------WIFI----------
    SSID: mywifissid
    PASS: mypass
    MODE: server
    -------------------------

---

### Общая папка библиотек
Иногда бывает удобно иметь общую папку с библиотеками для нескольких проектов, например когда библиотека разрабатывается и тестируется в нескольких проектах сразу. Для этого нужно указать путь к папке в настройке lib_extra_dirs:

    ; platformio.ini
    lib_extra_dirs = C:\Users\Alex\OneDrive\Documents\PlatformIO\libraries

---

### serial-plotter extention для показа графиков из серийного порта
badlogicgames.serial-plotter
https://marketplace.visualstudio.com/items?itemName=badlogicgames.serial-plotter

---

### При сборке под ESP32C3 появлялось предупреждение 

    warning: init-statement in selection statements only available with -std=c++17 or -std=gnu++17
        if(auto msg = get_last_received_message(); msg)
            ^~~~
Добавить в platformio.ini для сборки [env:esp32c3] параметры:

    build_flags = 
        -std=gnu++17

    build_unflags =
        -std=gnu++11
        -Wregister

## TODO
[+] Перейти на GTimer https://github.com/GyverLibs/GTimer
[+] Added version badge 
