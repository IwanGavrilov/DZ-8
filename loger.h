#include <fstream>
#include <filesystem>
#include <string>
#include <queue>
#include <vector>
#include <list>

#ifndef LOGER_H
#define LOGER_H

using namespace std;

template<typename T>
struct value_type { using type = T; };

class Loger {
    private:
        string sFileName = "";      // имя файла лога
        int nStatus{0};             // статус работы с файлом: 0 - все хорошо, 1 - не хорошо

    public:
        Loger(string sFileName);

        // возвращаем статус работы с файлом
        int GetStatus();

        // добавляем в лог новую запись
        template <typename T>
        void AddRecord(T data)
        {
            fstream file_add(sFileName, ios::app);
            if (file_add.is_open()) {
                std::string sOut = ToString(data);
                file_add << sOut << std::endl;
                file_add.close();
            }
        }

    private:
        // создаем файл лога. возвращаемое значение 0 - файл успешно создан. 1 - не создан
        int CreateFile();

        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<std::is_integral<T>::value>::type* = 0)
        {
            return std::to_string(_Value);
        }

        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
        {
            return std::to_string(_Value);
        }
        
        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<is_same<T, std::string>::value>::type* = 0)
        {
            return _Value;
        }

        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<std::is_same<T, std::queue<typename T::value_type>>::value>::type* = 0)
        {
            std::string sOut = "";

            for( ; !_Value.empty(); _Value.pop()) {
                sOut += _Value.front();
                sOut += " ";
            }
            return sOut;
        }

        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<std::is_same<T, std::vector<typename T::value_type>>::value>::type* = 0)
        {
            std::string sOut = "";
            for (auto iter : _Value) {
                sOut += std::to_string(iter);
                sOut += " ";
            }
            return sOut;
        }

        template <typename T>
        std::string ToString(T _Value, typename std::enable_if<std::is_same<T, std::list<typename T::value_type>>::value>::type* = 0)
        {
            std::string sOut = "";
            for (auto iter : _Value) {
                sOut += std::to_string(iter);
                sOut += " ";
            }
            return sOut;
        }

};

#endif