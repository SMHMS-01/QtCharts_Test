#include "mainwidget.h"

#include "covid19data.h"

#include "customslice.h"
#include "pentool.h"
#include "brushtool.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFontDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QFont>
QT_CHARTS_USE_NAMESPACE

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
      m_slice(0)
{
    VTRACE(MsgTypes::LOG,"IN");
    //import coid19 data
    Covid19Data * tmp_data = Covid19Data::getSingletonInstance();
    ms_cdata = tmp_data->getData();

    // create chart
    QChart *chart = new QChart;
    chart->setTitle("全球新冠疫情感染案例  "+ms_cdata[0].update);
    chart->setAnimationOptions(QChart::AllAnimations);
    // create series
    m_series = new QPieSeries();


    QVector<QString> default_country_setting({"China","Germany","USA","Russia","India","UK","France"});
    foreach (const covid_data& tmp, ms_cdata) {
        for(auto & countryStr:default_country_setting)
            if(tmp.country_name == countryStr)
                *m_series<< new CustomSlice(countryStr,tmp.confirmed_cases);
    }

    m_series->setLabelsVisible();
    chart->addSeries(m_series);

    connect(m_series, &QPieSeries::clicked, this, &MainWidget::handleSliceClicked);

    // chart settings
    m_themeComboBox = new QComboBox();
    m_themeComboBox->addItem("Light", QChart::ChartThemeLight);
    m_themeComboBox->addItem("BlueCerulean", QChart::ChartThemeBlueCerulean);
    m_themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    m_themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    m_themeComboBox->addItem("Qt", QChart::ChartThemeQt);

    m_aaCheckBox = new QCheckBox();
    m_animationsCheckBox = new QCheckBox();
    m_animationsCheckBox->setCheckState(Qt::Checked);

    m_legendCheckBox = new QCheckBox();

    QScrollArea *settingsScrollBar = new QScrollArea();
    QWidget *settingsContentWidget = new QWidget();

    QFormLayout *chartSettingsLayout = new QFormLayout(settingsContentWidget);
    chartSettingsLayout->addRow("样式主题", m_themeComboBox);
    chartSettingsLayout->addRow("抗锯齿", m_aaCheckBox);
    chartSettingsLayout->addRow("动画效果", m_animationsCheckBox);
    chartSettingsLayout->addRow("图例", m_legendCheckBox);
    QGroupBox *chartSettings = new QGroupBox("图表");
    chartSettings->setLayout(chartSettingsLayout);

    connect(m_themeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWidget::updateChartSettings);
    connect(m_aaCheckBox, &QCheckBox::toggled, this, &MainWidget::updateChartSettings);
    connect(m_animationsCheckBox, &QCheckBox::toggled, this, &MainWidget::updateChartSettings);
    connect(m_legendCheckBox, &QCheckBox::toggled, this, &MainWidget::updateChartSettings);

    m_sizeFactor = new QDoubleSpinBox();
    m_sizeFactor->setMinimum(0.0);
    m_sizeFactor->setMaximum(1.0);
    m_sizeFactor->setSingleStep(0.1);
    m_sizeFactor->setValue(m_series->pieSize());

    m_holeSize = new QDoubleSpinBox();
    m_holeSize->setMinimum(0.0);
    m_holeSize->setMaximum(1.0);
    m_holeSize->setSingleStep(0.1);
    m_holeSize->setValue(m_series->holeSize());

    // initialize pushbutton for exectuing py program
    button_py = new QPushButton("Run Python Script");
    process_py = new QProcess(this);
    connect(process_py, SIGNAL(readyReadStandardOutput()), this, SLOT(handle_output()));
    connect(process_py, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handle_finished(int, QProcess::ExitStatus)));
    connect(button_py, SIGNAL(clicked()), this, SLOT(run_py_script()));

    QPushButton *removeSlice = new QPushButton("移除选定的国家");
    // Add countries as comboboxes
    m_countryComboBox = new QComboBox();
    foreach (const covid_data& tmp, ms_cdata) {
        m_countryComboBox->addItem(tmp.country_name);
    }

    QFormLayout *seriesSettingsLayout = new QFormLayout(settingsContentWidget);
    seriesSettingsLayout->addRow("其他数据可视化形式",button_py);
    seriesSettingsLayout->addRow("切片尺寸", m_sizeFactor);
    seriesSettingsLayout->addRow("孔洞尺寸", m_holeSize);
    seriesSettingsLayout->addRow("添加国家",m_countryComboBox);
    seriesSettingsLayout->addRow(removeSlice);


    QGroupBox *seriesSettings = new QGroupBox("图形");
    seriesSettings->setLayout(seriesSettingsLayout);

    connect(m_sizeFactor,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWidget::updateSerieSettings);
    connect(m_holeSize,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWidget::updateSerieSettings);
    connect(m_countryComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWidget::appendSlice);
    connect(removeSlice, &QPushButton::clicked, this, &MainWidget::removeSlice);

    // slice settings
    m_sliceName = new QLineEdit("<点击一个国家>");
    m_sliceName->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_sliceValue = new QLineEdit("<数值显示咋在这里>");
    m_sliceValue->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_sliceLabelVisible = new QCheckBox();
    m_sliceLabelArmFactor = new QDoubleSpinBox();
    m_sliceLabelArmFactor->setSingleStep(0.01);
    m_sliceExploded = new QCheckBox();
    m_sliceExplodedFactor = new QDoubleSpinBox();
    m_sliceExplodedFactor->setSingleStep(0.01);
    m_pen = new QPushButton("切片边界色");
    m_penTool = new PenTool("切片边界色", this);
    m_brush = new QPushButton("切片填充色");
    m_brushTool = new BrushTool("切片填充色", this);
    m_font = new QPushButton("标签字体");
    m_labelBrush = new QPushButton("标签颜色");
    m_labelBrushTool = new BrushTool("标签颜色", this);
    m_labelPosition = new QComboBox(this);
    m_labelPosition->addItem("外部", QPieSlice::LabelOutside);
    m_labelPosition->addItem("内部-水平", QPieSlice::LabelInsideHorizontal);
    m_labelPosition->addItem("内部-垂直", QPieSlice::LabelInsideTangential);
    m_labelPosition->addItem("内部", QPieSlice::LabelInsideNormal);

    QFormLayout *sliceSettingsLayout = new QFormLayout(settingsContentWidget);
    sliceSettingsLayout->addRow("标签", m_sliceName);
    sliceSettingsLayout->addRow("数值", m_sliceValue);
    sliceSettingsLayout->addRow("切片边界色",m_pen);
    sliceSettingsLayout->addRow("切片填充色",m_brush);
    sliceSettingsLayout->addRow("标签可视", m_sliceLabelVisible);
    sliceSettingsLayout->addRow("标签字体",m_font);
    sliceSettingsLayout->addRow("标签颜色",m_labelBrush);
    sliceSettingsLayout->addRow("标签位置", m_labelPosition);
    sliceSettingsLayout->addRow("标签臂长", m_sliceLabelArmFactor);
    sliceSettingsLayout->addRow("散开", m_sliceExploded);
    sliceSettingsLayout->addRow("散开距离", m_sliceExplodedFactor);
    QGroupBox *sliceSettings = new QGroupBox("已选定的国家");
    sliceSettings->setLayout(sliceSettingsLayout);

    connect(m_sliceName, &QLineEdit::textChanged, this, &MainWidget::updateSliceSettings);
    connect(m_sliceValue,&QLineEdit::textChanged, this, &MainWidget::updateSliceSettings);

    connect(m_pen, &QPushButton::clicked, m_penTool, &PenTool::show);
    connect(m_penTool, &PenTool::changed, this, &MainWidget::updateSliceSettings);
    connect(m_brush, &QPushButton::clicked, m_brushTool, &BrushTool::show);
    connect(m_brushTool, &BrushTool::changed, this, &MainWidget::updateSliceSettings);
    connect(m_font, &QPushButton::clicked, this, &MainWidget::showFontDialog);
    connect(m_labelBrush, &QPushButton::clicked, m_labelBrushTool, &BrushTool::show);
    connect(m_labelBrushTool, &BrushTool::changed, this, &MainWidget::updateSliceSettings);
    connect(m_sliceLabelVisible, &QCheckBox::toggled, this, &MainWidget::updateSliceSettings);
    connect(m_sliceLabelArmFactor,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWidget::updateSliceSettings);
    connect(m_sliceExploded, &QCheckBox::toggled, this, &MainWidget::updateSliceSettings);
    connect(m_sliceExplodedFactor,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWidget::updateSliceSettings);
    connect(m_labelPosition,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWidget::updateSliceSettings);

    // create chart view
    m_chartView = new QChartView(chart);

    // create main layout
    QVBoxLayout *settingsLayout = new QVBoxLayout();
    settingsLayout->addWidget(chartSettings);
    settingsLayout->addWidget(seriesSettings);
    settingsLayout->addWidget(sliceSettings);

    settingsContentWidget->setLayout(settingsLayout);
    settingsScrollBar->setWidget(settingsContentWidget);
    settingsScrollBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QGridLayout *baseLayout = new QGridLayout();
    baseLayout->addWidget(settingsScrollBar, 0, 0);
    baseLayout->addWidget(m_chartView, 0, 1);
    setLayout(baseLayout);

    updateSerieSettings();
    updateChartSettings();
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::run_py_script()
{
    VTRACE(MsgTypes::LOG,"IN");
    QString python_script(DEFAUL_PY_SCRIPT);
    QStringList arguments;
    arguments << python_script;
    process_py->start("python", arguments);
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::handle_output()
{
    VTRACE(MsgTypes::LOG,"IN")
            QString output = QString::fromLocal8Bit(process_py->readAllStandardOutput());
    qDebug() << "Output:" << output;
    VTRACE(MsgTypes::LOG,"OUT")
}

void MainWidget::handle_finished(int exit_code, QProcess::ExitStatus exit_status)
{
    VTRACE(MsgTypes::LOG,"IN")
            qDebug() << "Process finished with exit code" << exit_code << "and exit status" << exit_status;
    VTRACE(MsgTypes::LOG,"OUT")
}
void MainWidget::updateChartSettings()
{
    VTRACE(MsgTypes::LOG,"IN");
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(m_themeComboBox->itemData(
                                                                   m_themeComboBox->currentIndex()).toInt());
    m_chartView->chart()->setTheme(theme);
    m_chartView->setRenderHint(QPainter::Antialiasing, m_aaCheckBox->isChecked());

    if (m_animationsCheckBox->checkState() == Qt::Checked)
        m_chartView->chart()->setAnimationOptions(QChart::AllAnimations);
    else
        m_chartView->chart()->setAnimationOptions(QChart::NoAnimation);

    if (m_legendCheckBox->checkState() == Qt::Checked)
        m_chartView->chart()->legend()->show();
    else
        m_chartView->chart()->legend()->hide();
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::updateSerieSettings()
{
    VTRACE(MsgTypes::LOG,"IN");
    m_series->setPieSize(m_sizeFactor->value());
    m_holeSize->setMaximum(m_sizeFactor->value());
    m_series->setHoleSize(m_holeSize->value());
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::updateSliceSettings()
{
    VTRACE(MsgTypes::LOG,"IN");
    if (!m_slice)
        return;

    m_slice->setLabel(m_sliceName->text());

    m_slice->setValue(m_sliceValue->text().toDouble());

    m_slice->setPen(m_penTool->pen());
    m_slice->setBrush(m_brushTool->brush());

    m_slice->setLabelBrush(m_labelBrushTool->brush());
    m_slice->setLabelVisible(m_sliceLabelVisible->isChecked());
    m_slice->setLabelArmLengthFactor(m_sliceLabelArmFactor->value());
    // can't work on asyn
    m_slice->setLabelPosition((QPieSlice::LabelPosition)m_labelPosition->currentIndex());

    m_slice->setExploded(m_sliceExploded->isChecked());
    m_slice->setExplodeDistanceFactor(m_sliceExplodedFactor->value());
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::handleSliceClicked(QPieSlice *slice)
{
    VTRACE(MsgTypes::LOG,"IN");
    m_slice = static_cast<CustomSlice *>(slice);

    // name
    m_sliceName->blockSignals(true);
    m_sliceName->setText(slice->label());
    m_sliceName->blockSignals(false);

    // value
    m_sliceValue->blockSignals(true);
    m_sliceValue->setText(QString::number(slice->value(),'.',0));
    m_sliceValue->blockSignals(false);

    // pen
    m_pen->setText(PenTool::name(m_slice->pen()));
    m_penTool->setPen(m_slice->pen());

    // brush
    m_brush->setText(m_slice->originalBrush().color().name());
    m_brushTool->setBrush(m_slice->originalBrush());

    // label
    m_labelBrush->setText(BrushTool::name(m_slice->labelBrush()));
    m_labelBrushTool->setBrush(m_slice->labelBrush());
    m_font->setText(slice->labelFont().toString());

    m_sliceLabelVisible->blockSignals(true);
    m_sliceLabelVisible->setChecked(slice->isLabelVisible());
    m_sliceLabelVisible->blockSignals(false);

    m_sliceLabelArmFactor->blockSignals(true);
    m_sliceLabelArmFactor->setValue(slice->labelArmLengthFactor());
    m_sliceLabelArmFactor->blockSignals(false);
    m_labelPosition->blockSignals(true);
    // can't work on asyn
    m_labelPosition->setCurrentIndex(slice->labelPosition());
    m_labelPosition->blockSignals(false);

    // exploded
    m_sliceExploded->blockSignals(true);
    m_sliceExploded->setChecked(slice->isExploded());
    m_sliceExploded->blockSignals(false);

    m_sliceExplodedFactor->blockSignals(true);
    m_sliceExplodedFactor->setValue(slice->explodeDistanceFactor());
    m_sliceExplodedFactor->blockSignals(false);
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::showFontDialog()
{
    VTRACE(MsgTypes::LOG,"IN");
    if (!m_slice)
        return;

    QFontDialog dialog(m_slice->labelFont());
    dialog.show();
    dialog.exec();

    m_slice->setLabelFont(dialog.currentFont());
    m_font->setText(dialog.currentFont().toString());
    VTRACE(MsgTypes::LOG,"OUT");
}

void MainWidget::appendSlice()
{
    VTRACE(MsgTypes::LOG,"IN");
    *m_series << new CustomSlice(ms_cdata[m_countryComboBox->currentIndex()].country_name,ms_cdata[m_countryComboBox->currentIndex()].confirmed_cases);
    VTRACE(MsgTypes::LOG,"OUT");
}


void MainWidget::removeSlice()
{
    VTRACE(MsgTypes::LOG,"IN");
    if (!m_slice)
        return;

    m_sliceName->setText("<点击一个国家>");
    m_sliceValue->setText("<数值显示在这里>");
    m_series->remove(m_slice);
    m_slice = 0;
    VTRACE(MsgTypes::LOG,"OUT");
}

#include "moc_mainwidget.cpp"
