#!/bin/bash

echo "=== Установка зависимостей для Radxa ROCK 4D Converter ==="
echo "Версия для Ubuntu 25.10 (Plucky Puffin)"
echo ""

# Проверка прав
if [ "$EUID" -ne 0 ]; then 
    echo "Пожалуйста, запустите с sudo: sudo $0"
    exit 1
fi

# Обновление репозиториев
echo "Обновление списка пакетов..."
apt update

echo ""
echo "=== Установка системных библиотек ==="
apt install -y \
    libc6 \
    libstdc++6 \
    libgcc-s1 \
    zlib1g \
    libatomic1 \
    libbz2-1.0

echo ""
echo "=== Установка Qt5 библиотек ==="
# В Ubuntu 25.10 используются пакеты с суффиксом t64
apt install -y \
    libqt5core5t64 \
    libqt5gui5t64 \
    libqt5widgets5t64 \
    libqt5network5t64 \
    libqt5concurrent5t64 \
    libdouble-conversion3 \
    libpcre2-16-0 \
    libglib2.0-0t64 \
    libharfbuzz0b \
    libpng16-16t64 \
    libmd4c0 \
    libfreetype6 \
    libgraphite2-3 \
    libbrotli1

echo ""
echo "=== Установка OpenGL библиотек ==="
# В Ubuntu 25.10 libgl1-mesa-glx заменен на libgl1
apt install -y \
    libgl1 \
    libglx-mesa0 \
    libglvnd0

echo ""
echo "=== Установка ICU библиотек ==="
apt install -y \
    libicu76 \
    libicu-data

echo ""
echo "=== Установка библиотек безопасности ==="
apt install -y \
    libssl3t64 \
    libkrb5-3 \
    libk5crypto3 \
    libcom-err2 \
    libkrb5support0 \
    libkeyutils1

echo ""
echo "=== Установка SSH библиотеки ==="
apt install -y \
    libssh-4

echo ""
echo "=== Установка X11 библиотек ==="
apt install -y \
    libx11-6 \
    libxcb1 \
    libxau6 \
    libxdmcp6 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxcb-sync1 \
    libxcb-xfixes0 \
    libxcb-xinerama0 \
    libxcb-xkb1 \
    libxkbcommon-x11-0 \
    libxkbcommon0

echo ""
echo "=== Установка дополнительных библиотек ==="
apt install -y \
    libzstd1 \
    libbz2-1.0

echo ""
echo "=== Установка платформенных плагинов Qt5 ==="
# В Ubuntu 25.10 пакет libqt5xcbqpa5 заменен на qt5-gtk-platformtheme
apt install -y \
    qt5-gtk-platformtheme \
    qt5-image-formats-plugins

# Дополнительные XCB плагины для Qt
apt install -y \
    libxcb-xinerama0 \
    libxcb-xinput0 \
    libxcb-util1

echo ""
echo "=== Установка Wayland поддержки (опционально) ==="
apt install -y \
    qtwayland5 || echo "Wayland пакеты не установлены (некритично)"

echo ""
echo "=== Проверка установленных библиотек ==="
echo ""

# Проверка основных библиотек
echo "Проверка libQt5Core:"
ldconfig -p | grep libQt5Core || echo "⚠ Не найдена"
echo ""
echo "Проверка libssh:"
ldconfig -p | grep libssh || echo "⚠ Не найдена"
echo ""
echo "Проверка OpenGL:"
ldconfig -p | grep libGL || echo "⚠ Не найдена"
echo ""

echo "=== Установка завершена ==="
echo ""
echo "Теперь можно запускать программу: ./radxa_converter"