#ifndef CUSTOMSLICE_H
#define CUSTOMSLICE_H

#include <QtCharts/QPieSlice>

QT_CHARTS_USE_NAMESPACE

class CustomSlice : public QPieSlice
{
    Q_OBJECT

public:
    CustomSlice(QString label, qreal value);

public:
    QBrush originalBrush();

public Q_SLOTS:
    void showHighlight(bool show);

private:
    QBrush m_originalBrush;
};

#endif // CUSTOMSLICE_H
