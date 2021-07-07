#include "outputconfig.h"
#include "resolutionslider.h"
#include "utils.h"
#include "scalesize.h"

#include <QStringBuilder>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

#include <QComboBox>
#include <QGSettings>
#include <QDBusInterface>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>

#include "ComboBox/combobox.h"

double mScaleres = 0;

OutputConfig::OutputConfig(QWidget *parent) :
    QWidget(parent),
    mOutput(nullptr)
{
    initDpiConnection();
}

OutputConfig::OutputConfig(const KScreen::OutputPtr &output, QWidget *parent) :
    QWidget(parent)
{
    initDpiConnection();
    setOutput(output);
}

OutputConfig::~OutputConfig()
{
}

void OutputConfig::setTitle(const QString &title)
{
    mTitle->setText(title);
}

void OutputConfig::initUi()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(2);

    // 分辨率下拉框
    mResolution = new ResolutionSlider(mOutput, this);
    mResolution->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QLabel *resLabel = new QLabel(this);
    //~ contents_path /display/resolution
    resLabel->setText(tr("resolution"));
    resLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    resLabel->setFixedSize(118, 30);

    QHBoxLayout *resLayout = new QHBoxLayout();
    resLayout->addWidget(resLabel);
    resLayout->addWidget(mResolution);

    QFrame *resFrame = new QFrame(this);
    resFrame->setFrameShape(QFrame::Shape::Box);
    resFrame->setLayout(resLayout);

    resFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    resFrame->setMinimumSize(552, 50);
    resFrame->setMaximumSize(960, 50);

    vbox->addWidget(resFrame);

    connect(mResolution, &ResolutionSlider::resolutionChanged,
            this, [=](QSize size, bool emitFlag){
                slotResolutionChanged(size, emitFlag);
            });

    connect(mResolution, &ResolutionSlider::resolutionChanged,
            this, &OutputConfig::slotScaleIndex);

    connect(mResolution, &ResolutionSlider::resolutionsave,
            this, &OutputConfig::slotScaleIndex);

    // 方向下拉框
    mRotation = new QComboBox(this);
    mRotation->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QLabel *rotateLabel = new QLabel(this);
    // ~contents_path /display/orientation
    rotateLabel->setText(tr("orientation"));
    rotateLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rotateLabel->setFixedSize(118, 30);

    QHBoxLayout *rotateLayout = new QHBoxLayout();
    rotateLayout->addWidget(rotateLabel);

    rotateLayout->addWidget(mRotation);

    QFrame *rotateFrame = new QFrame(this);
    rotateFrame->setFrameShape(QFrame::Shape::Box);
    rotateFrame->setLayout(rotateLayout);

    rotateFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rotateFrame->setMinimumSize(550, 50);
    rotateFrame->setMaximumSize(960, 50);

    mRotation->addItem(tr("arrow-up"), KScreen::Output::None);
    mRotation->addItem(tr("90° arrow-right"), KScreen::Output::Right);
    mRotation->addItem(tr("90° arrow-left"), KScreen::Output::Left);
    mRotation->addItem(tr("arrow-down"), KScreen::Output::Inverted);
    connect(mRotation, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotRotationChanged);
    mRotation->setCurrentIndex(mRotation->findData(mOutput->rotation()));

    vbox->addWidget(rotateFrame);

    // 刷新率下拉框
    mRefreshRate = new QComboBox(this);

    QLabel *freshLabel = new QLabel(this);
    // ~contents_path /display/frequency
    freshLabel->setText(tr("frequency"));
    freshLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    freshLabel->setFixedSize(118, 30);

    QHBoxLayout *freshLayout = new QHBoxLayout();
    freshLayout->addWidget(freshLabel);
    freshLayout->addWidget(mRefreshRate);

    QFrame *freshFrame = new QFrame(this);
    freshFrame->setFrameShape(QFrame::Shape::Box);
    freshFrame->setLayout(freshLayout);

    freshFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    freshFrame->setMinimumSize(550, 50);
    freshFrame->setMaximumSize(960, 50);

    mRefreshRate->addItem(tr("auto"), -1);
    vbox->addWidget(freshFrame);

    slotResolutionChanged(mResolution->currentResolution(), true);
    connect(mRefreshRate, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotRefreshRateChanged);

    // 缩放率下拉框
    QFrame *scaleFrame = new QFrame(this);
    scaleFrame->setFrameShape(QFrame::Shape::Box);

    scaleFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    scaleFrame->setMinimumSize(550, 50);
    scaleFrame->setMaximumSize(960, 50);

    QHBoxLayout *scaleLayout = new QHBoxLayout(scaleFrame);

    mScaleCombox = new QComboBox(this);
    mScaleCombox->setObjectName("scaleCombox");

    double scale = getScreenScale();

    slotScaleIndex(mResolution->currentResolution());

    mScaleCombox->setCurrentText(scaleToString(scale));

    if (mScaleCombox->findData(scale) == -1) {
        mScaleCombox->addItem(scaleToString(scale), scale);
        mScaleCombox->setCurrentText(scaleToString(scale));
    }

    connect(mScaleCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &OutputConfig::slotScaleChanged);

    QLabel *scaleLabel = new QLabel(this);
    //~ contents_path /display/screen zoom
    scaleLabel->setText(tr("screen zoom"));
    scaleLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    scaleLabel->setFixedSize(118, 30);

    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(mScaleCombox);

    vbox->addWidget(scaleFrame);

    initConnection();
}

double OutputConfig::getScreenScale()
{
    double scale = 1.0;
    if (QGSettings::isSchemaInstalled(SCALE_SCHEMAS)) {
        if (mDpiSettings->keys().contains("scalingFactor")) {
            scale = mDpiSettings->get(SCALE_KEY).toDouble();
        }
    }
    return scale;
}

void OutputConfig::initConnection()
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
        mRotation->blockSignals(true);
        mRotation->setCurrentIndex(index);
        mRotation->blockSignals(false);
    });

    connect(mOutput.data(), &KScreen::Output::currentModeIdChanged,
            this, [=]() {
        mRefreshRate->blockSignals(true);
        if (mOutput->currentMode())
            slotResolutionChanged(mOutput->currentMode()->size(), false);
        mRefreshRate->blockSignals(false);
    });
}

void OutputConfig::initDpiConnection()
{
    QByteArray id(SCALE_SCHEMAS);
    if (QGSettings::isSchemaInstalled(SCALE_SCHEMAS)) {
        mDpiSettings = new QGSettings(id, QByteArray(), this);
        connect(mDpiSettings, &QGSettings::changed, this, [=](QString key) {
            slotDPIChanged(key);
        });
    }
}

QString OutputConfig::scaleToString(double scale)
{
    return QString::number(scale * 100) + "%";
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

void OutputConfig::slotResolutionChanged(const QSize &size, bool emitFlag)
{
    // Ignore disconnected outputs
    if (!size.isValid()) {
        return;
    }

    QString modeID;
    KScreen::ModePtr currentMode = mOutput->currentMode();
    QList<KScreen::ModePtr> modes;
    Q_FOREACH (const KScreen::ModePtr &mode, mOutput->modes()) {
        if (mode->size() == size) {
            modes << mode;
        }
    }

//    Q_ASSERT(currentMode);
    if (!currentMode)
        return;
    modeID = currentMode->id();

    // Don't remove the first "Auto" item - prevents ugly flicker of the combobox
    // when changing resolution
    for (int i = mRefreshRate->count(); i >= 2; --i) {
        mRefreshRate->removeItem(i - 1);
    }

    for (int i = 0, total = modes.count(); i < total; ++i) {
        const KScreen::ModePtr mode = modes.at(i);

        bool alreadyExisted = false;
        for (int j = 0; j < mRefreshRate->count(); ++j) {
            if (tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())) == mRefreshRate->itemText(j)) {
                alreadyExisted = true;
                break;
            }
        }
        if (alreadyExisted == false) {   //不添加已经存在的项
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

    if (emitFlag)
        Q_EMIT changed();
}

void OutputConfig::slotRotationChanged(int index)
{
    KScreen::Output::Rotation rotation
        = static_cast<KScreen::Output::Rotation>(mRotation->itemData(index).toInt());
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
    qDebug() << "modeId is:" << modeId << endl;
    mOutput->setCurrentModeId(modeId);

    Q_EMIT changed();
}

void OutputConfig::slotScaleChanged(int index)
{
    Q_EMIT scaleChanged(mScaleCombox->itemData(index).toDouble());
}

void OutputConfig::slotDPIChanged(QString key)
{
    if (!key.compare("scalingFactor", Qt::CaseSensitive)) {
        double scale = mDpiSettings->get(key).toDouble();
        if (mScaleCombox->findData(scale) == -1) {
            mScaleCombox->addItem(scaleToString(scale), scale);
        }
        mScaleCombox->blockSignals(true);
        mScaleCombox->setCurrentText(scaleToString(scale));
        mScaleCombox->blockSignals(false);
    }
}

void OutputConfig::slotScaleIndex(const QSize &size)
{
    QSize msize;
    if (mScaleSize != QSize()) {
        msize = size.width() > mScaleSize.width()?mScaleSize:size;
    } else {
        msize = size;
    }
    if (!msize.isValid()) {
        return;
    }

    mScaleCombox->blockSignals(true);
    mScaleCombox->clear();
    mScaleCombox->addItem("100%", 1.0);

    if (k150Scale.contains(msize)) {
        mScaleCombox->addItem("125%", 1.25);
        mScaleCombox->addItem("150%", 1.5);
    }
    if (k175Scale.contains(msize)) {
        mScaleCombox->addItem("175%", 1.75);
    }
    if (k200Scale.contains(msize)) {
        mScaleCombox->addItem("200%", 2.0);
    }
    if (k250Scale.contains(msize)) {
        mScaleCombox->addItem("225%", 2.25);
        mScaleCombox->addItem("250%", 2.5);
    }
    if (k275Scale.contains(msize)) {
        mScaleCombox->addItem("275%", 2.75);
    }

    double scale = getScreenScale();

    if (mScaleCombox->findData(scale) == -1) {
        //该变量保存改变前的缩放率，当用户点击恢复时，恢复对应的缩放率
        mScaleres = scale;

        scale = 1.0;
        if (QGSettings::isSchemaInstalled(SCALE_SCHEMAS)) {
            if (mDpiSettings->keys().contains("scalingFactor")) {
                mDpiSettings->set(SCALE_KEY,scale);
            }
        }
        QMessageBox::information(this, tr("Information"),
                                 tr("Some applications need to be logouted to take effect"));
    }
    mScaleCombox->setCurrentText(scaleToString(scale));
    mScaleCombox->blockSignals(false);
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

// 拿取配置
void OutputConfig::initConfig(const KScreen::ConfigPtr &config)
{
    mConfig = config;
}
