#!/bin/bash

echo "=== Проверка зависимостей Radxa ROCK 4D Converter ==="
echo "Версия для Ubuntu 25.10"
echo ""

# Список необходимых библиотек с актуальными именами
LIBS=(
    "libQt5Core.so.5"
    "libQt5Gui.so.5"
    "libQt5Widgets.so.5"
    "libQt5Network.so.5"
    "libQt5Concurrent.so.5"
    "libssh.so.4"
    "libGL.so.1"
    "libcrypto.so.3"
    "libstdc++.so.6"
    "libc.so.6"
    "libpthread.so.0"
    "libxcb.so.1"
    "libX11.so.6"
    "libpng16.so.16"
    "libharfbuzz.so.0"
    "libfreetype.so.6"
    "libglib-2.0.so.0"
    "libicui18n.so.76"
    "libicuuc.so.76"
    "libz.so.1"
    "libgcc_s.so.1"
    "libm.so.6"
    "libdouble-conversion.so.3"
    "libpcre2-16.so.0"
)

MISSING=0
FOUND=0

for lib in "${LIBS[@]}"; do
    if ldconfig -p 2>/dev/null | grep -q "$lib"; then
        echo "✓ $lib"
        ((FOUND++))
    else
        echo "✗ $lib - НЕ НАЙДЕНА"
        ((MISSING++))
    fi
done

echo ""
echo "=== Результат ==="
echo "Найдено: $FOUND"
echo "Отсутствует: $MISSING"

if [ $MISSING -eq 0 ]; then
    echo "✓ Все зависимости установлены!"
    exit 0
else
    echo "⚠ Отсутствуют $MISSING библиотек"
    echo ""
    echo "Для установки выполните:"
    echo "  sudo apt update"
    echo "  sudo apt install libqt5core5t64 libqt5gui5t64 libqt5widgets5t64 \\"
    echo "    libqt5network5t64 libqt5concurrent5t64 libssh-4 libgl1 \\"
    echo "    libssl3t64 libx11-6 libxcb1 qt5-gtk-platformtheme"
    exit 1
fi