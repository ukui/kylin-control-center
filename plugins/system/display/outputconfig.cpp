#include "outputconfig.h"
#include "resolutionslider.h"
#include "utils.h"

#include <QStringBuilder>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

#include <QComboBox>
#include <QGSettings>

#include <QDBusInterface>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>

#include "ComboBox/combobox.h"

#define SCALE_SCHEMAS "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY     "scaling-factor"

const QSize KRsolution(1920, 1080);

OutputConfig::OutputConfig(QWidget *parent)
    : QWidget(parent)
    , mOutput(nullptr)
{

}

OutputConfig::OutputConfig(const KScreen::OutputPtr &output, QWidget *parent)
    : QWidget(parent)
{
    setOutput(output);
}

OutputConfig::~OutputConfig()
{
}

void OutputConfig::setTitle(const QString& title)
{
    mTitle->setText(title);
}


void OutputConfig::initUi()
{
    connect(mOutput.data(), &KScreen::Output::isConnectedChanged,
            this, [=]() {
                if (!mOutput->isConnected()) {
                    setVisible(false);
                }
            });

    connect(mOutput.data(), &KScreen::Output::rotationChanged,
            this, [=]() {
                const int index = mRotation->findData(mOutput->rotation());
                mRotation->setCurrentIndex(index);
            });

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(2);

    //分辨率下拉框
    mResolution = new ResolutionSlider(mOutput, this);
    mResolution->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mResolution->setMinimumSize(402,30);
    mResolution->setMaximumSize(16777215,30);

    QLabel *resLabel = new QLabel(this);
    //~ contents_path /display/resolution
    resLabel->setText(tr("resolution"));
    resLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    resLabel->setMinimumSize(118,30);
    resLabel->setMaximumSize(118,30);

    QHBoxLayout *resLayout = new QHBoxLayout();
    resLayout->addWidget(resLabel);
    resLayout->addWidget(mResolution);

    QFrame *resFrame = new QFrame(this);
    resFrame->setFrameShape(QFrame::Shape::Box);
    resFrame->setLayout(resLayout);

    resFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    resFrame->setMinimumSize(552,50);
    resFrame->setMaximumSize(960,50);

    vbox->addWidget(resFrame);

    connect(mResolution, &ResolutionSlider::resolutionChanged,
            this, &OutputConfig::slotResolutionChanged);

    //方向下拉框
    mRotation = new QComboBox(this);
    mRotation->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mRotation->setMinimumSize(402,30);
    mRotation->setMaximumSize(16777215,30);

    QLabel *rotateLabel = new QLabel(this);
    // ~contents_path /display/orientation
    rotateLabel->setText(tr("orientation"));
    rotateLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateLabel->setMinimumSize(118,30);
    rotateLabel->setMaximumSize(118,30);

    QHBoxLayout *rotateLayout = new QHBoxLayout();
    rotateLayout->addWidget(rotateLabel);

    rotateLayout->addWidget(mRotation);

    QFrame *rotateFrame = new QFrame(this);
    rotateFrame->setFrameShape(QFrame::Shape::Box);
    rotateFrame->setLayout(rotateLayout);

    rotateFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateFrame->setMinimumSize(550,50);
    rotateFrame->setMaximumSize(960,50);

    mRotation->addItem(tr("arrow-up"), KScreen::Output::None);
    mRotation->addItem(tr("90° arrow-right"), KScreen::Output::Right);
    mRotation->addItem(tr("90° arrow-left"), KScreen::Output::Left);
    mRotation->addItem(tr("arrow-down"), KScreen::Output::Inverted);
    connect(mRotation, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotRotationChanged);
    mRotation->setCurrentIndex(mRotation->findData(mOutput->rotation()));

    vbox->addWidget(rotateFrame);

    //刷新率下拉框
    mRefreshRate = new QComboBox(this);
    mRefreshRate->setMinimumSize(402,30);
    mRefreshRate->setMaximumSize(16777215,30);

    QLabel *freshLabel = new QLabel(this);
    // ~contents_path /display/frequency
    freshLabel->setText(tr("frequency"));
    freshLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    freshLabel->setMinimumSize(118,30);
    freshLabel->setMaximumSize(118,30);

    QHBoxLayout *freshLayout = new QHBoxLayout();
    freshLayout->addWidget(freshLabel);
    freshLayout->addWidget(mRefreshRate);

    QFrame *freshFrame = new QFrame(this);
    freshFrame->setFrameShape(QFrame::Shape::Box);
    freshFrame->setLayout(freshLayout);

    freshFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    freshFrame->setMinimumSize(550,50);
    freshFrame->setMaximumSize(960,50);

    mRefreshRate->addItem(tr("auto"), -1);
    vbox->addWidget(freshFrame);

    slotResolutionChanged(mResolution->currentResolution());
    connect(mRefreshRate, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotRefreshRateChanged);

    scaleCombox = new QComboBox(this);
    scaleCombox->setObjectName("scaleCombox");
    scaleCombox->setMinimumSize(402,30);
    scaleCombox->setMaximumSize(16777215,30);

    int maxReslu = mResolution->getMaxResolution().width();

    scaleCombox->addItem(tr("100%"));
    if (maxReslu >= 2000) {
        scaleCombox->addItem(tr("200%"));
    }

    QLabel *scaleLabel = new QLabel(this);
    //~ contents_path /display/screen zoom
    scaleLabel->setText(tr("screen zoom"));
    scaleLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    scaleLabel->setMinimumSize(118,30);
    scaleLabel->setMaximumSize(118,30);

    QHBoxLayout *scaleLayout = new QHBoxLayout();
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(scaleCombox);

    QFrame *scaleFrame = new QFrame(this);
    scaleFrame->setFrameShape(QFrame::Shape::Box);
    scaleFrame->setLayout(scaleLayout);

    scaleFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    scaleFrame->setMinimumSize(550,50);
    scaleFrame->setMaximumSize(960,50);
    vbox->addWidget(scaleFrame);

    int scale = getScreenScale();

    scaleCombox->setCurrentIndex(0);
    if (scale <= scaleCombox->count() && scale > 0) {
        scaleCombox->setCurrentIndex(scale - 1);
    }
    slotScaleChanged(scale - 1);

    connect(scaleCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &OutputConfig::slotScaleChanged);
}

int OutputConfig::getScreenScale() {
    QByteArray id(SCALE_SCHEMAS);
    int scale = 1;
    if (QGSettings::isSchemaInstalled(SCALE_SCHEMAS)) {
        QGSettings dpiSettings(id);
        if (dpiSettings.keys().contains("scalingFactor")) {
            scale = dpiSettings.get(SCALE_KEY).toInt();
        }
    }
    return scale;
}

void OutputConfig::setOutput(const KScreen::OutputPtr &output)
{
    mOutput = output;
    initUi();
}

KScreen::OutputPtr OutputConfig::output() const
{
    return mOutput;
}


void OutputConfig::slotResolutionChanged(const QSize &size)
{
    qDebug() << Q_FUNC_INFO << size;
    // Ignore disconnected outputs
    if (!size.isValid()) {
        return;
    }

    QString modeID;
    KScreen::ModePtr selectedMode;
    KScreen::ModePtr currentMode  = mOutput->currentMode();
    QList<KScreen::ModePtr> modes;
    Q_FOREACH (const KScreen::ModePtr &mode, mOutput->modes()) {
        if (mode->size() == size) {
            selectedMode = mode;
            modes << mode;
        }
    }

    Q_ASSERT(selectedMode);
    modeID = selectedMode->id();

    // Don't remove the first "Auto" item - prevents ugly flicker of the combobox
    // when changing resolution
    for (int i = mRefreshRate->count(); i >= 2; --i) {
        mRefreshRate->removeItem(i - 1);
    }

    for (int i = 0, total = modes.count(); i < total; ++i) {
        const KScreen::ModePtr mode = modes.at(i);
        if (!(mode->size() == KRsolution && mode->refreshRate() < 50.0)) {
            mRefreshRate->addItem(tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())), mode->id());
        }

        // If selected refresh rate is other then what we consider the "Auto" value
        // - that is it's not the highest resolution - then select it, otherwise
        // we stick with "Auto"
        if (mode == currentMode && mRefreshRate->count() > 1) {
            // i + 1 since 0 is auto
            mRefreshRate->setCurrentIndex(i + 1);
        }
    }

    if (-1 == mRefreshRate->currentIndex() || 0 == mRefreshRate->currentIndex()) {
        modeID = mRefreshRate->itemData(1).toString();
    }

    mOutput->setCurrentModeId(modeID);

    Q_EMIT changed();
}

void OutputConfig::slotRotationChanged(int index)
{
    KScreen::Output::Rotation rotation =
        static_cast<KScreen::Output::Rotation>(mRotation->itemData(index).toInt());
    mOutput->setRotation(rotation);

    Q_EMIT changed();
}

void OutputConfig::slotRefreshRateChanged(int index)
{
    QString modeId;
    if (index == 0) {
        // Item 0 is "Auto" - "Auto" is equal to highest refresh rate (at least
        // that's how I understand it, and since the combobox is sorted in descending
        // order, we just pick the second item from top
        modeId = mRefreshRate->itemData(1).toString();
    } else {
        modeId = mRefreshRate->itemData(index).toString();
    }
    qDebug()  << "modeId is:" << modeId << endl;
    mOutput->setCurrentModeId(modeId);

    Q_EMIT changed();
}

void OutputConfig::slotScaleChanged(int index)
{
    Q_EMIT scaleChanged(index);
}

void OutputConfig::setShowScaleOption(bool showScaleOption)
{
    mShowScaleOption = showScaleOption;
    if (mOutput) {
        initUi();
    }
}

bool OutputConfig::showScaleOption() const
{
    return mShowScaleOption;
}

//拿取配置
void OutputConfig::initConfig(const KScreen::ConfigPtr &config){
    mConfig = config;
}
