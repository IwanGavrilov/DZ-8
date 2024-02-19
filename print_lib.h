#include <iostream>
#include <vector>
#include <list>
#include <queue>

#ifndef PRINT_LIB_H
#define PRINT_LIB_H

/* печатаем контейнеры
    формат вывода value[0] value[1] ... value[n]
*/
template <typename T>
typename std::enable_if<std::is_same<T, std::vector<typename T::value_type>>::value ||
        std::is_same<T, std::list<typename T::value_type>>::value>::type
print_value(T container)
{
    std::cout << "Values:{ ";
    for(auto value : container)
        std::cout << value << " ";
    std::cout << "}\n";
}

// печатаем очередь
template <typename T>
typename std::enable_if<std::is_same<T, std::queue<typename T::value_type>>::value>::type
print_value(T conteiner)
{
    std::cout << "Values:{ ";
    for (; !conteiner.empty(); conteiner.pop()) {
        std::cout << conteiner.front() << ' ';
    }
    
    std::cout << "}\n";
}

// печатаем целые
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
print_value(T && nValue)
{
    std::cout << nValue << std::endl;
}

// печатаем дробные
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
print_value(T && dValue)
{
    std::cout << std::fixed << dValue << std::endl;
}

// печатаем строку
template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value>::type
print_value(T && sValue)
{
    std::cout << sValue << std::endl;
}

#endif