#include "pentool.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QColorDialog>

PenTool::PenTool(QString title, QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(title);
    setWindowFlags(Qt::Tool);

    m_colorButton = new QPushButton();

    m_widthSpinBox = new QDoubleSpinBox();

    m_styleCombo = new QComboBox();
    m_styleCombo->addItem("NoPen");
    m_styleCombo->addItem("SolidLine");
    m_styleCombo->addItem("DashLine");
    m_styleCombo->addItem("DotLine");
    m_styleCombo->addItem("DashDotLine");
    m_styleCombo->addItem("DashDotDotLine");

    m_capStyleCombo = new QComboBox();
    m_capStyleCombo->addItem("FlatCap", Qt::FlatCap);
    m_capStyleCombo->addItem("SquareCap", Qt::SquareCap);
    m_capStyleCombo->addItem("RoundCap", Qt::RoundCap);

    m_joinStyleCombo = new QComboBox();
    m_joinStyleCombo->addItem("MiterJoin", Qt::MiterJoin);
    m_joinStyleCombo->addItem("BevelJoin", Qt::BevelJoin);
    m_joinStyleCombo->addItem("RoundJoin", Qt::RoundJoin);
    m_joinStyleCombo->addItem("SvgMiterJoin", Qt::SvgMiterJoin);

    QFormLayout *layout = new QFormLayout();
    layout->addRow("颜色", m_colorButton);
    layout->addRow("宽度", m_widthSpinBox);
    layout->addRow("风格", m_styleCombo);
    layout->addRow("Cap style", m_capStyleCombo);
    layout->addRow("添加风格", m_joinStyleCombo);
    setLayout(layout);

    // Use old style connect on some signals because the signal is overloaded
    connect(m_colorButton, &QPushButton::clicked, this, &PenTool::showColorDialog);
    connect(m_widthSpinBox,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &PenTool::updateWidth);
    connect(m_styleCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PenTool::updateStyle);
    connect(m_capStyleCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PenTool::updateCapStyle);
    connect(m_joinStyleCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PenTool::updateJoinStyle);
}

void PenTool::setPen(const QPen &pen)
{
    m_pen = pen;
    m_colorButton->setText(m_pen.color().name());
    m_widthSpinBox->setValue(m_pen.widthF());
    m_styleCombo->setCurrentIndex(m_pen.style()); // index matches the enum
    m_capStyleCombo->setCurrentIndex(m_capStyleCombo->findData(m_pen.capStyle()));
    m_joinStyleCombo->setCurrentIndex(m_joinStyleCombo->findData(m_pen.joinStyle()));
}

QPen PenTool::pen() const
{
    return m_pen;
}

QString PenTool::name()
{
    return name(m_pen);
}

QString PenTool::name(const QPen &pen)
{
    return pen.color().name() + ":" + QString::number(pen.widthF());
}

void PenTool::showColorDialog()
{
    QColorDialog dialog(m_pen.color());
    dialog.show();
    dialog.exec();
    m_pen.setColor(dialog.selectedColor());
    m_colorButton->setText(m_pen.color().name());
    emit changed();
}

void PenTool::updateWidth(double width)
{
    if (!qFuzzyCompare((qreal) width, m_pen.widthF())) {
        m_pen.setWidthF(width);
        emit changed();
    }
}

void PenTool::updateStyle(int style)
{
    if (m_pen.style() != style) {
        m_pen.setStyle((Qt::PenStyle) style);
        emit changed();
    }
}

void PenTool::updateCapStyle(int index)
{
    Qt::PenCapStyle capStyle = (Qt::PenCapStyle) m_capStyleCombo->itemData(index).toInt();
    if (m_pen.capStyle() != capStyle) {
        m_pen.setCapStyle(capStyle);
        emit changed();
    }
}

void PenTool::updateJoinStyle(int index)
{
    Qt::PenJoinStyle joinStyle = (Qt::PenJoinStyle) m_joinStyleCombo->itemData(index).toInt();
    if (m_pen.joinStyle() != joinStyle) {
        m_pen.setJoinStyle(joinStyle);
        emit changed();
    }
}

#include "moc_pentool.cpp"
