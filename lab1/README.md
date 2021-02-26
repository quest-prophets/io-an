# Лабораторная работа 1

**Название:** "Разработка драйверов символьных устройств"

**Цель работы:** "Получить знания и навыки разработки драйверов символьных устройств для операционной системы Linux."

## Описание функциональности драйвера

При записи строки в файл символьного устройства драйвер производит подсчет суммы всех чисел в строке, разделенных другими символами (буквы, пробелы и т.п.). Последовательность полученных результатов с момента загрузки модуля ядра должна выводиться при чтении созданного файла /proc/var3 в консоль пользователя. При чтении из файла символьного устройства в кольцевой буфер ядра должен осуществляться вывод тех же данных, которые выводятся при чтении файла /proc/var3.

## Инструкция по сборке

Собирается с помощью команды ```make```.

## Инструкция пользователя

Для запуска драйвера необходимо ввести команду ```insmod ch_driver.ko``` && ```sudo chmod 0666 /dev/var3```, которая загружает модуль ядра в систему и устанавливает необходимые права доступа.
Для выгрузки модуля драйвера нужно ввести команду ```sudo rmmod ch_driver```.

Для записи текста можно использовать команду ```echo "text" > dev/var3```, где вместо ```text``` подставляется пользовательский текст. После записи текста драйвер сохраняет количество пробелов в введенном тексте.

Для просмотра списка полученных результатов можно ввести команду ```cat /proc/var3```.
Можно так же использовать команду ```cat /dev/var3``` для вывода результатов в кольцевой буфер, содержимое которого можно посмотреть с помощью команды ```dmesg```.

## Примеры использования

### Создание символьного устройства и пример работы

```bash
root@vagrant:/home/vagrant/Documents/io-an/lab1# make
root@vagrant:/home/vagrant/Documents/io-an/lab1# insmod ch_driver.ko
root@vagrant:/home/vagrant/Documents/io-an/lab1# echo '' > /dev/var3 
root@vagrant:/home/vagrant/Documents/io-an/lab1# cat /proc/var3 
0
root@vagrant:/home/vagrant/Documents/io-an/lab1# echo '3d3ddijgrf7' > /dev/var3 
root@vagrant:/home/vagrant/Documents/io-an/lab1# cat /proc/var3 
0
13
root@vagrant:/home/vagrant/Documents/io-an/lab1# echo 'dfjj' > /dev/var3 
root@vagrant:/home/vagrant/Documents/io-an/lab1# cat /proc/var3 
0
13
0
```

### Проверка вывода данных в кольцевой буфер

```bash
root@vagrant:/home/vagrant/Documents/io-an/lab1# echo 'dfjj' > /dev/var3 
root@vagrant:/home/vagrant/Documents/io-an/lab1# cat /dev/var3 
root@vagrant:/home/vagrant/Documents/io-an/lab1# dmesg
[ 5330.007349] Driver: open()
[ 5330.007420] Driver: close()
[ 5332.477246] Driver: open()
[ 5332.477275] 0
[ 5332.477277] 13
[ 5332.477279] 0
[ 5332.477281] 0
[ 5332.477305] Driver: close()
```
