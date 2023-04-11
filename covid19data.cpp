#include "covid19data.h"

Covid19Data *Covid19Data::instance=0;


//Covid19Data* mg_covid19data = Covid19Data::getSingletonInstance();

Covid19Data::Covid19Data(const QUrl& qurl):m_url(qurl)
{
    //Set uo the request
    m_request.setUrl(m_url);
    m_reply = m_manager.get(m_request);
    // Send the request and wait for the response

    while(!m_reply->isFinished())
        qApp->processEvents();

#if 0
    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
#endif

    parseCovid19Data();
//    printCovid19Data();
}

Covid19Data* Covid19Data::getSingletonInstance()
{
    if(!instance)
        instance = new Covid19Data(QUrl(DEFAULT_COVID19_DATA_API_1));
    return instance;
}
Covid19Data::~Covid19Data()
{
    delete m_reply;
}
void Covid19Data::parseCovid19Data()
{
    VTRACE(MsgTypes::LOG,"IN");
    // Parse the JSON response
    m_response_data = m_reply->readAll();
    m_json_doc = QJsonDocument::fromJson(m_response_data);
    m_json_array = m_json_doc.array();

    // Clean up
    m_reply->deleteLater();

    // Iterate over the data for each country/region
    foreach (const QJsonValue& country, m_json_array)
    {
        QJsonObject country_data = country.toObject();
        QDateTime dataTime;
        covid_data tmp_data;

        tmp_data.country_name= country_data.value("country").toString();
        tmp_data.continent= country_data.value("continent").toString();
        tmp_data.confirmed_cases= country_data.value("cases").toInt();
        tmp_data.deaths= country_data.value("deaths").toInt();
        dataTime.setTime_t(country_data.value("updated").toDouble()/1000);
        tmp_data.update= dataTime.toString("yyyy-MM-dd");
        tmp_data.new_cases= country_data.value("todayCases").toInt();
        tmp_data.population= country_data.value("population").toInt();
        tmp_data.recovered= country_data.value("recovered").toInt();
        tmp_data.latitude= country_data.value("lat").toDouble();
        tmp_data.longitude= country_data.value("long").toDouble();

        m_data.push_back(tmp_data);
    }
    VTRACE(MsgTypes::LOG,"OUT");
}

void Covid19Data::printCovid19Data()
{
    for(unsigned i=0; i< unsigned(m_data.size()); ++i)
    {
        qDebug() << "Country or Region: " << m_data.at(i).country_name;
        qDebug() << "Confirmed cases: " << m_data.at(i).confirmed_cases;
        qDebug() << "Deaths: " << m_data.at(i).deaths;
        qDebug() << "Update: " << m_data.at(i).update;
        qDebug() << "New cases: " << m_data.at(i).new_cases;
    }
}

DataForm Covid19Data::getData()
{
    return m_data;
}
