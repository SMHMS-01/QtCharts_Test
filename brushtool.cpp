#include "brushtool.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QColorDialog>

BrushTool::BrushTool(QString title, QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(title);
    setWindowFlags(Qt::Tool);

    m_colorButton = new QPushButton();
    m_styleCombo = new QComboBox();
    m_styleCombo->addItem("Nobrush", (int) Qt::NoBrush);
    m_styleCombo->addItem("Solidpattern", (int) Qt::SolidPattern);
    m_styleCombo->addItem("Dense1pattern", (int) Qt::Dense1Pattern);
    m_styleCombo->addItem("Dense2attern", (int) Qt::Dense2Pattern);
    m_styleCombo->addItem("Dense3Pattern", (int) Qt::Dense3Pattern);
    m_styleCombo->addItem("HorPattern", (int) Qt::HorPattern);
    m_styleCombo->addItem("VerPattern", (int) Qt::VerPattern);
    m_styleCombo->addItem("CrossPattern", (int) Qt::CrossPattern);
    m_styleCombo->addItem("BDiagPattern", (int) Qt::BDiagPattern);
    m_styleCombo->addItem("FDiagPattern", (int) Qt::FDiagPattern);
    m_styleCombo->addItem("DiagCrossPattern", (int) Qt::DiagCrossPattern);

    QFormLayout *layout = new QFormLayout();
    layout->addRow("颜色", m_colorButton);
    layout->addRow("风格", m_styleCombo);
    setLayout(layout);

    connect(m_colorButton, &QPushButton::clicked, this, &BrushTool::showColorDialog);
    connect(m_styleCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &BrushTool::updateStyle);
}

void BrushTool::setBrush(QBrush brush)
{
    m_brush = brush;
    m_colorButton->setText(m_brush.color().name());
    m_styleCombo->setCurrentIndex(m_brush.style());
}

QBrush BrushTool::brush() const
{
    return m_brush;
}

QString BrushTool::name()
{
    return name(m_brush);
}

QString BrushTool::name(const QBrush &brush)
{
    return brush.color().name();
}

void BrushTool::showColorDialog()
{
    QColorDialog dialog(m_brush.color());
    dialog.show();
    dialog.exec();
    m_brush.setColor(dialog.selectedColor());
    m_colorButton->setText(m_brush.color().name());
    emit changed();
}

void BrushTool::updateStyle()
{
    Qt::BrushStyle style = (Qt::BrushStyle) m_styleCombo->itemData(m_styleCombo->currentIndex()).toInt();
    if (m_brush.style() != style) {
        m_brush.setStyle(style);
        emit changed();
    }
}

#include "moc_brushtool.cpp"
