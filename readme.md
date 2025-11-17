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
![ESP32-C3 SuperMini плата разработки (памятка)](https://blog.mons.ws/?p=4548) - советы по работе с ESP32C3Mini

    Если требуется внешний источник питания, просто подключите + уровень внешнего источника питания к точке 5В. А GND подключается к отрицательному полюсу. (Поддерживаются источники питания 3,3 — 6 В). Помните что при подключении внешнего источника питания доступ к USB невозможен. USB и внешний источник питания можно выбрать только один.

    Будьте максимально осторожны, во время пайки, не допускайте короткого замыкания положительного и отрицательных электродов, иначе это приведет к возгоранию батареи и оборудования.

#### Часто задаваемые вопросы (FAQ)

- COM-порт не распознается в IDE

    Способ 1: Войдите в режим загрузки. Для этого нажмите и удерживайте кнопку BOOT, затем подайте питания на плату (подключите кабель).

    Способ 2: Нажмите и удерживайте кнопку BOOT на ESP32C3, нажмите кнопку RESET, отпустите кнопку RESET, а затем отпустите кнопку BOOT. После этого ESP32C3 перейдет в режим загрузки. (При каждом подключении необходимо повторно войти в режим загрузки, иногда понадобится нажать один раз, как только услышали звук нового устройства, значит все получилось.

- Программа не запускается после загрузки

    Иногда после успешной загрузки скетча, вам необходимо нажать кнопку Reset, чтобы выполнить перезагрузку.

- Последовательный порт ESP32 C3 SuperMini не печатает вывод

    Установите для параметра FLASH MODE значение "QIO", а USB CDC On Boot "Enabled" и наоборот. (менять необходимо оба параметра)

- Нет подключения к сети WiFi при подключении датчиков

    Это вероятнее всего связано с конфликтом использования пинов (PINS), что может мешать работе WiFI. Для решения проблемы воспользуйтесь другим свободным пином, например PIN 0

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

### Локальная отладка ETL в этом проекте

- выбрать гит для работы с последней опубликованной версией "https://github.com/JimorMarlow/etl@^0.9.1"
- или локальный путь к папке ETL "../etl"

[env]
framework = arduino
monitor_speed = 115200
lib_deps = 
	gyverlibs/GTimer@^1.1.1
;	https://github.com/JimorMarlow/etl@^0.9.1       
	../etl

    Предполагается что этот проект и ETL лежат в одной родительской папке
    Wemos_D1_Mini_V4/
    ├── lib/
    │   └── MyDebugLibrary/     
    ├── src/
    │   └── main.cpp
    └── platformio.ini
    ETL/    <-- локальная библиотека
    ├── lib/
    ├── src/
    │   └── ETL/
    │   │   └── ...
    │   └── main.cpp
    └── platformio.ini

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

---

### ESP32-WROOM-32U
https://myrobot.ru/wiki/index.php?n=Experiences.Esp32Pinout

Добавил конфигурацию, соборал ETL для этой платы, вернулся сюда. Добавил картинки с pinout
Встроенный светодиод: GPIO2

Для загрузки прошивки нужно устанвоить драйвер: CP2102 драйвер для windows 7-11 / Linux / MacOS
Скачал тут: https://voltiq.ru/cp2102-driver-windows-7-10/

---

## TODO
[+] Перейти на GTimer https://github.com/GyverLibs/GTimer
[+] Added version badge 
