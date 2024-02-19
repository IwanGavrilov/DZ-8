#include "loger.h"

Loger::Loger(string sFileName)
{
    this->sFileName = sFileName;
    CreateFile();
}

int Loger::CreateFile ()
{
    fstream fLog;
    
    if(!std::ifstream(sFileName)) {
        fLog.open(sFileName, ios::out);
        if (fLog.is_open()) {
            // т.к. только создавали файл, тоего можно сразу закрыть
            fLog.close();
        }
        else {
            // если файл не создан и не открыт, то возвращаем ошибку
            nStatus = 1;
        }
    }

    return nStatus;
}

int Loger::GetStatus()
{
    return nStatus;
}