#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "covid19data.h"
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QProcess>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

class PenTool;
class BrushTool;
class CustomSlice;

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QPieSeries;
class QPieSlice;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);

public Q_SLOTS:
    void updateChartSettings();
    void updateSerieSettings();
    void updateSliceSettings();
    void handleSliceClicked(QPieSlice *slice);
    void showFontDialog();
    void appendSlice();
    void removeSlice();

private:
    QProcess *process_py;
    QPushButton * button_py;
private slots:
    void run_py_script();
    void handle_output();
    void handle_finished(int exit_code, QProcess::ExitStatus exit_status);

private:
    QVector<covid_data> ms_cdata;

    QCheckBox *m_sliceLabelVisible;
    QDoubleSpinBox *m_sliceLabelArmFactor;
    QCheckBox *m_sliceExploded;
    QDoubleSpinBox *m_sliceExplodedFactor;
    QPushButton *m_brush;
    BrushTool *m_brushTool;
    QPushButton *m_pen;
    PenTool *m_penTool;

    QComboBox * m_countryComboBox;
    QComboBox *m_themeComboBox;
    QCheckBox *m_aaCheckBox;
    QCheckBox *m_animationsCheckBox;
    QCheckBox *m_legendCheckBox;
    QChartView *m_chartView;
    QPieSeries *m_series;
    CustomSlice *m_slice;
    QDoubleSpinBox *m_sizeFactor;
    QDoubleSpinBox *m_holeSize;
    QLineEdit *m_sliceName;
    QLineEdit * m_sliceValue;

    QPushButton *m_font;
    QPushButton *m_labelBrush;
    QComboBox *m_labelPosition;
    BrushTool *m_labelBrushTool;
};

#endif // MAINWIDGET_H
