#ifndef COVID19DATA_H
#define COVID19DATA_H

#include "extradef.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUrl>
#include <QByteArray>
#include <QDateTime>
#include <QVector>
#include <QVectorIterator>
#include <QRandomGenerator>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>


typedef QVector<covid_data> DataForm;
class Covid19Data
{
public:
    ~Covid19Data();
    void parseCovid19Data();
    void printCovid19Data();
    DataForm getData();
    static Covid19Data *getSingletonInstance();
private:
    static Covid19Data * instance;
    Covid19Data(const QUrl& qurl);
private:
    QUrl m_url;
    QNetworkAccessManager m_manager;
    QNetworkRequest m_request;
    // Parse the JSON response
    QNetworkReply *m_reply;
    QByteArray m_response_data;
    QJsonDocument m_json_doc;
    QJsonArray m_json_array;
    DataForm m_data;
};

#endif // COVID19DATA_H
