#ifndef EXTRADEF_H
#define EXTRADEF_H

#include <QString>
#define DEFAULT_COVID19_DATA_API_1 "https://disease.sh/v3/covid-19/countries"
#define DEFAULT_COVID19_DATA_API_2 "https://api.covid19api.com/summary"
#define DEFAUL_PY_SCRIPT "E:/PY/program/testforDV/currForTesting.py"
// bool macros
#define VERROR 0
#define VTRUE 1

constexpr unsigned NUMBER_OF_COUNTRY= 231;

struct covid_data
{
    QString country_name;
    QString update;
    QString continent;
    unsigned population;
    unsigned confirmed_cases;
    unsigned deaths;
    unsigned new_cases;
    unsigned recovered;
    double latitude;
    double longitude;
};


// Print Log
#define VTRACE(LogType, ...)                                                                                                       \
    switch (LogType)                                                                                                               \
    {                                                                                                                              \
    case MsgTypes::LOG:                                                                                                            \
        printf("Date:%s Time:%s  Func:%s  Row:%d   LOG_Content: %s\n", __DATE__, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        break;                                                                                                                     \
    case MsgTypes::ERR:                                                                                                            \
        printf("Date:%s Time:%s  Func:%s  Row:%d   ERR_Content: %s\n", __DATE__, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        break;                                                                                                                     \
    default:                                                                                                                       \
        printf("Date:%s Time:%s  Func:%s  Row:%d   Content: %s\n", __DATE__, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);     \
        break;                                                                                                                     \
    }

// Enumnate value for Log Type
enum MsgTypes
{
    LOG = 1,
    ERR
};


#endif // EXTRADEF_H
