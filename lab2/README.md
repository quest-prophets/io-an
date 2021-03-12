# Лабораторная работа 2

**Название:** "Разработка драйверов блочных устройств"

**Цель работы:** получить знания и навыки разработки драйверов блочных устройств для операционной системы Linux.

## Описание функциональности драйвера

Драйвер блочного устройства создает виртуальный жесткий диск в оперативной памяти с размером 50 Мбайт. Созданный виртуальный диск содержит один первичный раздел размером 30Мбайт и один расширенный раздел, содержащий два логических раздела размером 10Мбайт каждый.

## Инструкция по сборке

Собирается с помощью команды ```make```.

## Инструкция пользователя

...

## Примеры использования

```bash
sudo parted /dev/mydisk
(parted) unit mib print
```

```bash
>sudo mkfs.vfat /dev/mydisk1
mkfs.fat 4.1 (2017-01-24)
>sudo mkfs.vfat /dev/mydisk5
mkfs.fat 4.1 (2017-01-24)
>sudo mkfs.vfat /dev/mydisk6
mkfs.fat 4.1 (2017-01-24)
```

```bash
sudo dd if=/dev/mydisk1 of=/dev/mydisk5 bs=512 count=20000 oflag=direct
res:
20000+0 records in
20000+0 records out
10240000 bytes (10 MB, 9.8 MiB) copied, 0.255166 s, 40.1 MB/s
```

```bash
sudo dd if=/dev/mydisk1 of=/dev/mydisk6 bs=512 count=20000 oflag=direct
res:
20000+0 records in
20000+0 records out
10240000 bytes (10 MB, 9.8 MiB) copied, 0.252593 s, 40.5 MB/s 
```

```bash
sudo dd if=/dev/mydisk5 of=/dev/mydisk6 bs=512 count=20000 oflag=direct
res:
20000+0 records in
20000+0 records out
10240000 bytes (10 MB, 9.8 MiB) copied, 0.255658 s, 40.1 MB/s
```


