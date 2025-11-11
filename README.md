# Network Server

Серверная программа для клиент-серверной системы обработки данных с аутентификацией.

## Установка зависимостей

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
```

## Запуск сервера

```bash
# Через скрипт запуска из корневой папки проекта
chmod +x start_server.sh
./start_server.sh

## Прямой запуск из корневой папки
./build/server --config ./data/vcalc.conf --log ./logs/vcalc.log --port 33333

## Прямой запуск из папки build
cd build
./server --config ../data/vcalc.conf --log ../logs/vcalc.log --port 33333
```

## Параметры командной строки

-c, --config FILE - файл базы пользователей (по умолчанию: /etc/vcalc.conf)
-l, --log FILE - файл журнала (по умолчанию: /var/log/vcalc.log)
-p, --port PORT - порт сервера (по умолчанию: 33333)
-h, --help - справка

## Скрипт тестового клиента

```bash
# Тестовый клиент для сервера
# Автоматически тестирует аутентификацию и вычисления

# Запускаем Python тестовый клиент из папки tests
chmod +x tests/test_server.py
python3 tests/test_server.py

# Запускаем Python тестовый клиент скрипт из корневой папки проекта
chmod +x test_client.sh
./test_client.sh
```