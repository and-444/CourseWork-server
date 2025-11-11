#!/usr/bin/env python3
"""
Тестовый клиент для проверки работы сервера
"""

import socket
import struct
import hashlib
import time
import sys

def sha256_hash(data):
    """Вычисление SHA256 хеша"""
    return hashlib.sha256(data.encode()).hexdigest()

def test_authentication(host='localhost', port=33333):
    """Тест аутентификации"""
    print("=== Тест аутентификации ===")
    
    # Тестовые пользователи
    test_users = [
        ('user1', 'password1'),
        ('user2', 'password2'), 
        ('test', 'test123'),
        ('admin', 'admin123'),
        ('invalid_user', 'wrongpass')  # Неправильный пользователь
    ]
    
    for username, password in test_users:
        print(f"\nТестируем пользователя: {username}")
        
        try:
            # Создаем сокет
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            
            # Подключаемся к серверу
            sock.connect((host, port))
            print("Подключение к серверу установлено")
            
            # Отправляем логин
            sock.send(username.encode())
            print(f"Отправлен логин: {username}")
            
            # Получаем соль от сервера
            salt = sock.recv(16).decode()
            if salt == "ERR":
                print("Сервер вернул ошибку: пользователь не найден")
                sock.close()
                continue
            print(f"Получена соль: {salt}")
            
            # Вычисляем и отправляем хеш
            hash_value = sha256_hash(salt + password)
            sock.send(hash_value.encode())
            print(f"Отправлен хеш: {hash_value[:16]}...")
            
            # Получаем результат аутентификации
            auth_result = sock.recv(4).decode()
            if auth_result == "OK":
                print("Аутентификация успешна!")
                
                # Тестируем вычисления после успешной аутентификации
                test_calculations(sock, username)
            else:
                print("Аутентификация не удалась")
            
            sock.close()
            
        except socket.timeout:
            print("Таймаут соединения")
        except ConnectionRefusedError:
            print("Не удалось подключиться к серверу. Убедитесь, что сервер запущен.")
            break
        except Exception as e:
            print(f"Ошибка: {e}")
        
        time.sleep(1)  # Пауза между тестами

def test_calculations(sock, username):
    """Тест вычислений с векторами"""
    print(f"\n  --- Тест вычислений для {username} ---")
    
    try:
        # Тестовые векторы
        test_vectors = [
            [1, 2, 3, 4],           # Обычный случай: 1*2*3*4 = 24
            [10, 20, 30],           # 10*20*30 = 6000
            [100, 200],             # 100*200 = 20000
            [1, 1, 1, 1, 1],        # Все единицы: 1
            [2, 3],                 # Маленький вектор: 6
            [1000000, 1000000],     # Большие числа: 1000000000000 (переполнение)
            [0, 1, 2, 3],           # С нулем: 0
            [4294967295, 2],        # Граничный случай (2^32-1 * 2)
        ]
        
        # Отправляем количество векторов
        vector_count = len(test_vectors)
        sock.send(struct.pack('!I', vector_count))
        print(f"Отправлено векторов: {vector_count}")
        
        # Отправляем каждый вектор
        for i, vector in enumerate(test_vectors):
            # Отправляем размер вектора
            size = len(vector)
            sock.send(struct.pack('!I', size))
            
            # Отправляем данные вектора
            for value in vector:
                sock.send(struct.pack('!I', value))
            
            print(f"Вектор {i+1}: {vector}")
        
        # Получаем количество результатов
        result_count_data = sock.recv(4)
        result_count = struct.unpack('!I', result_count_data)[0]
        print(f"Получено результатов: {result_count}")
        
        # Получаем результаты
        results = []
        for i in range(result_count):
            result_data = sock.recv(4)
            result = struct.unpack('!I', result_data)[0]
            results.append(result)
            
            # Вычисляем ожидаемый результат
            expected = 1
            for val in test_vectors[i]:
                expected *= val
                if expected > 0xFFFFFFFF:
                    expected = 0xFFFFFFFF
                    break
            
            status = "✓" if result == expected else "✗"
            print(f"Результат {i+1}: {result} (ожидалось: {expected}) {status}")
        
        return results
        
    except Exception as e:
        print(f"Ошибка при вычислениях: {e}")
        return []

def test_performance(host='localhost', port=33333):
    """Тест производительности"""
    print("\n=== Тест производительности ===")
    
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        sock.connect((host, port))
        
        # Аутентификация
        sock.send(b'test')
        salt = sock.recv(16).decode()
        hash_value = sha256_hash(salt + 'test123')
        sock.send(hash_value.encode())
        auth_result = sock.recv(4).decode()
        
        if auth_result != "OK":
            print("Аутентификация не удалась")
            return
        
        # Большой вектор для теста производительности
        large_vector = [2] * 1000  # 2^1000 - вызовет переполнение
        
        start_time = time.time()
        
        # Отправляем 1 вектор
        sock.send(struct.pack('!I', 1))
        
        # Отправляем размер вектора
        sock.send(struct.pack('!I', len(large_vector)))
        
        # Отправляем данные
        for value in large_vector:
            sock.send(struct.pack('!I', value))
        
        # Получаем результат
        result_count_data = sock.recv(4)
        result_count = struct.unpack('!I', result_count_data)[0]
        
        result_data = sock.recv(4)
        result = struct.unpack('!I', result_data)[0]
        
        end_time = time.time()
        
        print(f"Обработан вектор из 1000 элементов за {end_time - start_time:.3f} секунд")
        print(f"Результат: {result} (ожидалось: 4294967295 - переполнение)")
        
        sock.close()
        
    except Exception as e:
        print(f"Ошибка теста производительности: {e}")

def main():
    print("Тестовый клиент для сервера")
    print("=" * 50)
    
    if len(sys.argv) > 1:
        host = sys.argv[1]
        port = int(sys.argv[2]) if len(sys.argv) > 2 else 33333
    else:
        host = 'localhost'
        port = 33333
    
    print(f"Подключение к {host}:{port}")
    print()
    
    # Запускаем тесты
    test_authentication(host, port)
    test_performance(host, port)
    
    print("\n" + "=" * 50)
    print("Тестирование завершено!")

if __name__ == "__main__":
    main()