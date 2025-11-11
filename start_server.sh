#!/bin/bash

# Определяем корневую папку проекта
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Project directory: $SCRIPT_DIR"
echo "Starting server..."

# Запускаем сервер
./build/server --config "./data/vcalc.conf" --log "./logs/vcalc.log" --port 33333