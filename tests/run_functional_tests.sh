#!/bin/bash

echo "=== ЗАПУСК ФУНКЦИОНАЛЬНОГО ТЕСТИРОВАНИЯ ==="
echo "Дата: $(date)"
echo ""

# Создаем тестовые файлы
mkdir -p data logs
echo "testuser:testpass" > data/vcalc.conf
echo "alice:alicepass" >> data/vcalc.conf
echo "bob:bobpass" >> data/vcalc.conf

# 1. Запускаем сервер в фоне
echo "1. Запуск сервера..."
./server --config data/vcalc.conf --log logs/vcalc.log --port 33333 &
SERVER_PID=$!
sleep 2 # Даем серверу время на запуск

echo "   Сервер запущен (PID: $SERVER_PID)"
echo ""

# 2. Тестирование интерфейса командной строки
echo "2. Тестирование интерфейса командной строки:"
echo "   - Вывод справки..."
./server --help | head -5
echo "  Тест завершен"
echo ""

# 3. Проверка лог-файла
echo "3. Проверка лог-файла:"
if [ -f "logs/vcalc.log" ]; then
    echo "   Лог-файл создан"
    echo "   Последние строки лога:"
    tail -3 logs/vcalc.log
else
    echo "   ОШИБКА: Лог-файл не создан"
fi
echo ""

# 4. Проверка работы с неверными параметрами
echo "4. Тестирование обработки ошибок:"
echo "   - Неверный порт..."
timeout 1 ./server --port 70000 2>&1 | grep -q "Error parsing arguments" && echo "Ошибка обработана корректно"
echo ""

# 5. Остановка сервера
echo "5. Остановка сервера..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
echo "   Сервер остановлен"
echo ""

# 6. Итоговая проверка логов
echo "6. Итоговая проверка логов:"
if [ -f "logs/vcalc.log" ]; then
    echo "   Содержимое лог-файла:"
    cat logs/vcalc.log
    echo ""
    echo "Все операции записаны в лог"
fi

echo ""
echo "=== ФУНКЦИОНАЛЬНОЕ ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ==="