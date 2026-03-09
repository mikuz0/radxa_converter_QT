# Radxa ROCK 4D Converter

Удаленный перекодировщик видео для Radxa ROCK 4D с аппаратным ускорением.

## Возможности

- Подключение к Radxa по SSH
- Удаленный файловый менеджер
- Пакетная обработка файлов
- Профили настроек
- Аппаратное кодирование через RKMPP
- Извлечение аудио из видео
- Прогресс конвертации в реальном времени
- Темная тема

## Требования

- Ubuntu 22.04 или новее
- Qt5 (>= 5.12)
- libssh (>= 0.8)
- CMake (>= 3.16)
- g++ (>= 9.0)

## Установка зависимостей

```bash
sudo apt update
sudo apt install qt5-default qtbase5-dev qtbase5-dev-tools qttools5-dev-tools \
                 cmake build-essential libssh-dev pkg-config