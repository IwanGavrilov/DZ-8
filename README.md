# DZ-8
ДЗ - 8
Утилита для обнаружения фалов-дубликатов

Входные параметры командной строки:
- директории для сканирования. если несколько директорий, то перечислить директории через запятую, без пробелов
- директории для исключения из сканирования. если несколько директорий, то перечислить директории через запятую, без пробелов
- уровень сканирования (один на все директории, 0 - только указанная
директория без вложенных)
- минимальный размер файла, по умолчанию проверяются все файлы
больше 1 байта.
- маски имен файлов разрешенных для сравнения. если масок несколько, то перечислить через запятую
- размер блока, которым производится чтения файлов.
по умолчанию алгоритм шифрования выбран CrC32

Пример командной строки:
bayan /media/Test,/media/1/Test /media/Test/15 3 2 *.txt,*.bin 10