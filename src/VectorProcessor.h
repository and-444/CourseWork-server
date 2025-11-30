/**
 * @file VectorProcessor.h
 * @brief Обработчик математических операций над векторами
 * @author Савин А.В.
 * @version 1.0
 */

#ifndef VECTORPROCESSOR_H
#define VECTORPROCESSOR_H

#include <vector>
#include <cstdint>


/**
 * @class VectorProcessor
 * @brief Вычисление произведения элементов вектора
 * 
 * Класс предоставляет статический метод для вычисления произведения
 * всех элементов вектора с обработкой переполнения.
 */
class VectorProcessor {
public:
    /**
     * @brief Вычисление произведения элементов вектора
     * @param vector Входной вектор целых чисел
     * @return Произведение всех элементов вектора
     * 
     * @note При переполнении возвращает MAX_UINT32
     * @note Для пустого вектора возвращает 0
     */
    static uint32_t computeProduct(const std::vector<uint32_t>& vector);
};

#endif