#!/bin/bash

# Тестовый клиент для сервера
# Автоматически тестирует аутентификацию и вычисления

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Тестовый клиент для сервера ==="
echo "Убедитесь, что сервер запущен на порту 33333"
echo

# Проверяем, установлен ли Python3
if ! command -v python3 &> /dev/null; then
    echo "Установите Python3 для работы тестового клиента:"
    echo "sudo apt install python3"
    exit 1
fi

# Запускаем Python тестовый клиент
python3 tests/test_server.py