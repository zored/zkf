# YMD09

| | |
|---|---|
| Board | [YMDK YMD09](https://github.com/qmk/qmk_firmware/tree/master/keyboards/ymdk/ymd09), 3×3 macropad, ATmega32U4 |
| Framework | QMK via ZKF, target `ymdk/ymd09` |
| Firmware | `firmwares/ymd09.hex`: `./project.sh build ymd09` or `./project.sh download ymd09` |
| Flash tool | QMK Toolbox (Atmel DFU); legacy: `./project.sh flash ymd09` (`dfu-programmer`) |
| Layout | [layout viewer](https://zored.github.io/zkf/) → `ymd09`; source: `keyboards.ymd09` in [config.yaml](../../config.yaml) |

## Enter bootloader

Hold the top-right key (`c`, hold → `layer3A`), then press the bottom-left key (`doBootloader`).

## Flash

1. Open `firmwares/ymd09.hex` in QMK Toolbox.
2. Enter bootloader; Toolbox reports an Atmel DFU device.
3. Flash.

## How it works

Every key is a tap/hold dance: tap types a letter (`a`…`i`), hold opens a layer (two holds = second layer or a modifier). `doNextMapping` cycles the letter layouts below and switches the OS language.

## Layout cheat sheet

Four blocks: Latin, Latin shifted, Russian, Russian shifted.

```
a b c
d e f  

  
  g h  o   p  w x  
i j k  q r s  y z ,
l m n  t u v  ; [ ]

' . `  1 2 3  -   =
  \ /  4   6  0 5
       7 8 9  


  G H  O   P  W X  
I J K  Q R S  Y Z <
L M N  T U V  : { }

" > ~  ! @ #  _   +
  ? |  $   ^  ) %
       & * (  




а б в
г д е


  ё ж  н   о  х ц  
з и й  п р с  ч ш щ
к л м  т у ф  ъ ы ь 

э ю я  1 2 3  -   =
  \ .  4   6  0 5
       7 8 9 

А Б В
Г Д Е


  Ё Ж  Н   О  Х Ц  
З И Й  П Р С  Ч Ш Щ
К Л М  Т У Ф  Ъ Ы Ь 

Э Ю Я  ! " №  _   +
  / ,  ;   :  ) %
       ? * ( 
```