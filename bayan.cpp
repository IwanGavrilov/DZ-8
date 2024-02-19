#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <stdlib.h>
#include <malloc.h>

#include <boost/container/list.hpp>
#include <boost/container/vector.hpp>
#include <boost/crc.hpp>
#include <boost/any.hpp>

#include "lib.h"
#include "print_lib.h"

/*описание алгоритма
    1. в памяти строим структуру каталогов и файлов в них с описанием необходимых данных файла 
        (имя. размер, CrC32 первого блока) для дальнейшего исключения лишних обращений к диску
    2. при сравнении файлов проверяем размер файла
        2.1 если размеры равны:
            проверяем первый блок данных из файлов. если равны то дальше проверяем весь файл.
            если блоки не равны, то переходим к следующему файлу
    3. если нашли дубли файла, то выводим результат
    4. данные о всех найденых дубликатах помечаем в струтуре как не используемые. Они нам больше не нужны
*/

static int nSizeBlock {5};          // Размер вычитываемого блока данных. статическая переменная
static int nLevel {0};              // Уровень вложенности

// список каталогов для сканирвоания
static boost::container::vector<std::string> arrScan {""};

// список каталогов для исключения из сканирвоания
static boost::container::vector<std::string> arrNotScan {""};

static uint32_t lMinSize {1};        // минимальный размер файла для поиска

/*
получение CrC32 хеш кода для блока данные
data - блок данных

возвращает значение:
    хеш-код блока
*/
uint32_t GetCrC32(std::string data) {
    boost::crc_32_type result;

    result.process_bytes(data.data(), data.length());

    return result.checksum();
}

uint32_t GetCrC32(boost::any data) {
    boost::crc_32_type result;

    result.process_bytes(&data, sizeof(data));

    return result.checksum();
}

uint32_t GetCrC32(const char * data) {
    boost::crc_32_type result;

    result.process_bytes(data, sizeof(data));

    return result.checksum();
}

/*
функция вычисляет CrC32 хеш-код первого блока данных файла
sNameFile - имя файла
Возвращает значение
    CrC32 хеш-код
*/
uint32_t GetFirstBlockCrC(std::string sNameFile) {
    char *buffer;//[nSizeBlock]; // читаемый блок
    buffer = (char*)malloc(nSizeBlock + 1);
    std::ifstream outFile(sNameFile);

    outFile.getline(buffer, nSizeBlock + 1);
    outFile.close();
    uint32_t lCrC = GetCrC32(buffer);

    memset(buffer, 0, sizeof(nSizeBlock + 1));      // буфер уже не нужен. очищаем его на всякий случай

    return lCrC;
}

/*
функция вычисляет CrC32 коды всех блоков файла 
sNameFile - имя файла
Возвращает значение
    вектор CrC32 хеш-кодов всего файла
*/
boost::container::vector<uint32_t> GetAllBlocksCrC(std::string sNameFile) {
    boost::container::vector<uint32_t> arrCrC = boost::container::vector<uint32_t>();
    char *buffer; //[nSizeBlock];                                                // читаемый блок
    buffer = (char*)malloc(nSizeBlock + 1);
    std::ifstream outFile(sNameFile);

    while(outFile.getline(buffer, nSizeBlock + 1)) {
        uint32_t lCrC = GetCrC32(buffer);
        arrCrC.push_back(lCrC);
        lCrC = 0;
        memset(buffer, 0, sizeof(nSizeBlock + 1));
    }
    outFile.close();

    return arrCrC;
}

/*
функция вставляет запись в вектор строк, если такой строки там нет.
не нашел в библиотеке boost функции поиска элемента контейнера
*/
void AddVectorRecord(boost::container::vector<std::string> *vector, std::string sInput) {
    bool bFlag = false;
    for (std::string sCurr : *vector) {
        if (sCurr.compare(sInput) == 0) {
            bFlag = true;
        }
    }
    if (!bFlag) {
        vector->push_back(sInput);
    }
}

/*
функция поблочного сравнения файлов
sFileName1 - полный путь к первому файлу
sFileName2 - полный путь ко второму файлу
nSizeBlock - размер блока сравнения

возвращает значение:
    0 - файлы не равны
    1 - файлы равны
*/
int CompareFile(std::string sFileName1, std::string sFileName2) {
    int nOut {0};
    boost::container::vector<uint32_t> arrCrC1;  // все хеш коды из первого файла
    boost::container::vector<uint32_t> arrCrC2;  // все хеш коды из второго файла

    // вычитываем все хеши из первого файла поблочно
    arrCrC1 = GetAllBlocksCrC(sFileName1);

    // вычитываем все хеши из второго файла поблочно
    arrCrC2 = GetAllBlocksCrC(sFileName2);

    if (arrCrC1 == arrCrC2) {
        nOut = 1;
    }
    return nOut;
}

/*
функция формирует список исключенных каталогов
*/
void InitLists(std::string sScan, std::string sNotScan) {
    std::stringstream data1(sScan);
    std::string sSubStr {""};
    
    std::stringstream data2(sNotScan);
    
    while (std::getline(data1, sSubStr, ',')){
        if (sSubStr != "") {
            arrScan.push_back(sSubStr);
        }
    }

    while (std::getline(data2, sSubStr, ',')){
        if (sSubStr != "") {
            arrNotScan.push_back(sSubStr);
        }
    }
}
/*
функция проверяет: каталог можно сканировать или он исключен из сканирвоания
*/
bool IsScaning(std::string sNameCatalog) {
    // проверка выполняется по наличию имени каталога в строке каталогов исключенных из сканировани
    bool bFlag = false;
    for (std::string sCat : arrNotScan) {
        if (sCat.compare(sNameCatalog) == 0) {
            bFlag = true;
        }
    }
    return bFlag;
}

struct file{
    std::string sFileName{""};          // полный путь файла
    uint32_t lCrC32{0};                 // хеш-первого блока из файла
    uint32_t lSize{0};                  // размер
    bool bNotUsed{false};               // false - использовать файл при поиске дубликатов true - не использовать

    file(std::string sName, uint32_t CrC, uint32_t Size) {
         sFileName = sName;
         lCrC32 = CrC;
         lSize = Size;
         bNotUsed = false;
    }

    bool operator ==(const file &file1) {
        if (lSize != file1.lSize) {
            return false;
        }
        if (lCrC32 != file1.lCrC32) {
            return false;
        }

        return true;
    }
};

/*
struct catalog{
    std::string sCatalogName {""};
    boost::container::list<file> lstFiles;
    boost::container::list<catalog> lstSubCatalog;

    catalog (std::string sName) {
        sCatalogName = sName;
        lstFiles = boost::container::list<file>();
        lstSubCatalog = boost::container::list<catalog>();
    }

    void AddFile(file newFile) {
        lstFiles.push_back(newFile);
    }

    catalog * AddSubCatalog(std::string sCatalogName) {
        catalog *ctlNew = new catalog(sCatalogName);
        lstSubCatalog.push_back(*ctlNew);

        return ctlNew;
    }
*/

/*
ищем дубликаты исходного файла в файлах каталога
fileOrigin - исходный файл
lstFileNameDublicat - список полных имен уже найденных дубликатов
*/
void FindDublicat(file * fileOrigin, boost::container::list<file> *lstFiles, boost::container::vector<std::string> *lstFileNameDublicat) {
    for(boost::container::list<file>::iterator fileCurr = lstFiles->begin(); fileCurr != lstFiles->end(); fileCurr++) {
    //for (file fileCurr : *lstFiles) {
        if (!fileCurr->bNotUsed) {
            if (fileOrigin->sFileName != fileCurr->sFileName) {
                // исключаем сравнение с самим собой
                if (*fileOrigin == *fileCurr) {
                    // если равны 2 описания файла, то сравниваем файлы поблочно с диска
                    if (CompareFile(fileOrigin->sFileName, fileCurr->sFileName) == 1) {
                        // добавляем запись о найденом дубликате в список
                        AddVectorRecord(lstFileNameDublicat, fileOrigin->sFileName);
                        AddVectorRecord(lstFileNameDublicat, fileCurr->sFileName);
                        // т.к. дубликат найден, значит при сравнении со следеющим файлом этот файл нам не интересен
                        fileCurr->bNotUsed = true;
                        fileOrigin->bNotUsed = true;
                    }
                }
            }
        }
    }
}

/*
функция первого чтения файлов и построения структуры
так же при инициализации сравниваем со всеми файлами, первый файл
*/
void Initialize(std::string sCatalogPath, boost::container::list<file> * lst, 
                    boost::container::vector<std::string> * lstFileNameDublicat,
                    int nLev) {
    boost::container::vector<std::string> arrCatalog = boost::container::vector<std::string>();

    for (const auto & entry : std::filesystem::directory_iterator(sCatalogPath)) {
        //std::cout << entry.path() << std::endl;
        std::string sName = entry.path().c_str();
        if (entry.is_regular_file()) {
            // файл, определяем его размер и первый блок
            uint32_t lSize = std::filesystem::file_size(entry.path());
            if (lSize >= lMinSize) {
                // если размер файла больше или равно минимальному размеру для сканирования
                uint32_t lCrC = GetFirstBlockCrC(sName);
                file newFile(sName, lCrC, lSize);
                lst->push_back(newFile);
            }
        }
        if (entry.is_directory()) {
            // директория, добавляем в список поддиррикторий
            if (!IsScaning(sName)) {
                arrCatalog.push_back(sName);
            }
        }
    }
    // после того как обработали файлы в текущей директирии, обрабатываем файлы во поддиректориях
    // если уровень вложенности позволяет
    nLev++;                                 // т.к. переходим на следующий уровень, то увеличиваем счетчик
    if (nLevel >= nLev) {
        for (std::string sCat : arrCatalog) {
            Initialize(sCat, lst, lstFileNameDublicat, nLev);
        }
    }
}

void PrintResult(boost::container::vector<std::string> *vector) {
    if (vector->size() == 0) {
        return;
    }
    for (std::string sCurr : *vector) {
        print_value((std::string) sCurr);
    }
    print_value((std::string) "------------------------------------------");
}

int main(int argc, char *argv[]) {
    // разбираем командную строку
    //std::string sFile {"/media/waine-86/WORK/My_Project_C_Delphy/OTUS/DZ-8/Test"};
    std::string sFile{""};
    std::string sNotScan{""};

    if (argc >= 7) {
        sFile = argv[1];
        sNotScan = argv[2];
        nLevel = std::stoi(argv[3]);
        if (std::stol(argv[4]) > 1) {
            // если минимальный размер файла задан больше 5
            lMinSize = std::stol(argv[4]);
        }
        if (std::stoi(argv[6]) >= 0) {
            // минимальный размер блока не должен быть меньше либо равен 0
            nSizeBlock = std::stoi(argv[6]);
        }
    }
    else {
        print_value((std::string) "ERROR. Arguments <> 7");
        return 0;
    }
    
    int nLevelCat = 0;
  
    boost::container::list<file> listFiles = boost::container::list<file>();
    boost::container::vector<std::string> arrDublicatName = boost::container::vector<std::string>();
  
    // формирую список исключенных из проверки каталогов
    InitLists(sFile, sNotScan);

    // выполняем инициализацию данных по файлам для каждого пути
    for (std::string sPath : arrScan) {
        if (sPath != "") {
            Initialize(sPath, &listFiles, &arrDublicatName, nLevelCat);
        }
    }

    // ищем дубликаты уже во всех файлах
    for (file fl : listFiles) {
        if (!fl.bNotUsed) {
            // проверяем только те. у которых не стоит флаг не использовать
            // если флаг стоит,значит дубликат уже был ранее найден
            FindDublicat(&fl, &listFiles, &arrDublicatName);
            PrintResult(&arrDublicatName);
            arrDublicatName.clear();
        }
    }

    print_value((std::string) "The End");

    return 0;
}