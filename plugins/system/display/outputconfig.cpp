#include "outputconfig.h"
#include "resolutionslider.h"
#include "utils.h"
//#include "kcm_screen_debug.h"

#include <QStringBuilder>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

//#include <KLocalizedString>
#include <QComboBox>
#include <QGSettings>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>

#include "ComboBox/combobox.h"

#define SCALE_SCHEMAS "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY "scaling-factor"

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

//    connect(mOutput.data(), &KScreen::Output::isEnabledChanged,
//            this, [=]() {
//                mEnabled->setChecked(mOutput->isEnabled());
//            });

    connect(mOutput.data(), &KScreen::Output::rotationChanged,
            this, [=]() {
                const int index = mRotation->findData(mOutput->rotation());
                mRotation->setCurrentIndex(index);
            });

//    connect(mOutput.data(), &KScreen::Output::scaleChanged,
//            this, [=]() {
//                const int index = mScale->findData(mOutput->scale());
//                mScale->setCurrentIndex(index);
//            });


    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(2);

    //注释显示器上方标签

//    mTitle = new QLabel(this);
//    mTitle->setAlignment(Qt::AlignHCenter);
//    vbox->addWidget(mTitle);
  //  setTitle(Utils::outputName(mOutput));

    //QFormLayout *formLayout = new QFormLayout();



//    mEnabled = new QCheckBox(i18n("已启用"), this);
//    //mEnabled->setVisible(false);
//    mEnabled->setChecked(mOutput->isEnabled());
//    qDebug()<<"是否开启---->mEnable"<<mOutput<<endl;
//    connect(mEnabled, &QCheckBox::clicked,
//            this, [=](bool checked) {
//                  //qDebug()<<"显示屏为---->mEnable"<<mOutput<<endl;
//                  mOutput->setEnabled(checked);
//                  qDebug() << "上层勾选---->"<<mOutput.data() << mOutput->name() << mOutput->isEnabled();
//                  Q_EMIT changed();
//            });
//    formLayout->addRow(i18n("显示:"), mEnabled);

//    mMonitor = new QComboBox(this);
//    formLayout->addRow(i18n("主显示器"),mMonitor);

    //分辨率下拉框
    mResolution = new ResolutionSlider(mOutput, this);
//    mResolution->setFont(ft);
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
    mRotation = new QComboBox();
    mRotation->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mRotation->setMinimumSize(402,30);
    mRotation->setMaximumSize(16777215,30);

    QLabel *rotateLabel = new QLabel();
    // ~contents_path /display/orientation
    rotateLabel->setText(tr("orientation"));
//    rotateLabel->setFont(ft);
    rotateLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateLabel->setMinimumSize(118,30);
    rotateLabel->setMaximumSize(118,30);

    QHBoxLayout *rotateLayout = new QHBoxLayout();
    rotateLayout->addWidget(rotateLabel);

    rotateLayout->addWidget(mRotation);
//    rotateLayout->addStretch();

    QFrame *rotateFrame = new QFrame(this);
    rotateFrame->setFrameShape(QFrame::Shape::Box);
    rotateFrame->setLayout(rotateLayout);

    rotateFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateFrame->setMinimumSize(550,50);
    rotateFrame->setMaximumSize(960,50);

    mRotation->addItem( tr("arrow-up"), KScreen::Output::None);
    mRotation->addItem( tr("90° arrow-right"), KScreen::Output::Right);
    mRotation->addItem( tr("arrow-down"), KScreen::Output::Inverted);
    mRotation->addItem(tr("90° arrow-left"), KScreen::Output::Left);
    connect(mRotation, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotRotationChanged);
    mRotation->setCurrentIndex(mRotation->findData(mOutput->rotation()));

    //formLayout->addRow(rotateLabel, mRotation);
    vbox->addWidget(rotateFrame);

    //缩放暂时用不到
//    if (!mShowScaleOption) {
//        mScale = new QComboBox(this);
//        mScale->addItem(i18nc("Scale multiplier, show everything at 1 times normal scale", "1x"), 1);
//        mScale->addItem(i18nc("Scale multiplier, show everything at 2 times normal scale", "2x"), 2);
//        connect(mScale, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
//                this, &OutputConfig::slotScaleChanged);
//        mScale->setCurrentIndex(mScale->findData(mOutput->scale()));
//        formLayout->addRow(i18n("缩放:"), mScale);
//        formLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
//    }


    //刷新率下拉框
    mRefreshRate = new QComboBox();
    mRefreshRate->setMinimumSize(402,30);
    mRefreshRate->setMaximumSize(16777215,30);

    QLabel *freshLabel = new QLabel();
    // ~contents_path /display/refresh rate
    freshLabel->setText(tr("refresh rate"));
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


    scaleCombox = new QComboBox();
    scaleCombox->setObjectName("scaleCombox");
    scaleCombox->setMinimumSize(402,30);
    scaleCombox->setMaximumSize(16777215,30);

    int maxReslu = mResolution->getMaxResolution().width();

//#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
//    if (maxReslu >= 2000) {
//        scaleCombox->addItem(tr("200%"));
//    } else {
//        scaleCombox->addItem(tr("100%"));
//    }
//#else
    scaleCombox->addItem(tr("100%"));
    if (maxReslu >= 2000 && maxReslu <= 3800) {
        scaleCombox->addItem(tr("200%"));
    } else if (maxReslu >= 3800 || maxReslu >= 4000) {
        scaleCombox->addItem(tr("200%"));
        scaleCombox->addItem(tr("300%"));
    }
//#endif

    QLabel *scaleLabel = new QLabel();

    //~ contents_path /display/screen zoom
    scaleLabel->setText(tr("screen zoom"));
    scaleLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    scaleLabel->setMinimumSize(118,30);
    scaleLabel->setMaximumSize(118,30);

    QHBoxLayout *scaleLayout = new QHBoxLayout();
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(scaleCombox);
//    freshLayout->addStretch();

    QFrame *scaleFrame = new QFrame(this);
    scaleFrame->setFrameShape(QFrame::Shape::Box);
    scaleFrame->setLayout(scaleLayout);

    scaleFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    scaleFrame->setMinimumSize(550,50);
    scaleFrame->setMaximumSize(960,50);
    vbox->addWidget(scaleFrame);

    int scale = getScreenScale();
//#if QT_VERSION < QT_VERSION_CHECK(5,7,0)

//#else
    scaleCombox->setCurrentIndex(0);
    if (scale <= scaleCombox->count() && scale > 0) {
//        qDebug()<<"scale is----->"<<scale<<endl;
        scaleCombox->setCurrentIndex(scale - 1);
    }
    slotScaleChanged(scale - 1);
//#endif

    connect(scaleCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &OutputConfig::slotScaleChanged);
}

int OutputConfig::getScreenScale() {
    QGSettings * dpiSettings;
    QByteArray id(SCALE_SCHEMAS);
    int scale = 1;
    if (QGSettings::isSchemaInstalled(SCALE_SCHEMAS)) {
        dpiSettings = new QGSettings(id);
        if (dpiSettings->keys().contains("scalingFactor")) {
            scale = dpiSettings->get(SCALE_KEY).toInt();
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
    // Ignore disconnected outputs
    if (!size.isValid()) {
        return;
    }

    KScreen::ModePtr selectedMode;
    QList<KScreen::ModePtr> modes;
    Q_FOREACH (const KScreen::ModePtr &mode, mOutput->modes()) {
        if (mode->size() == size) {
            modes << mode;
            if (!selectedMode || selectedMode->refreshRate() < mode->refreshRate()) {
                selectedMode = mode;
            }
        }
    }

    Q_ASSERT(selectedMode);
    mOutput->setCurrentModeId(selectedMode->id());

    // Don't remove the first "Auto" item - prevents ugly flicker of the combobox
    // when changing resolution
    for (int i = mRefreshRate->count(); i >=2; --i) {
        mRefreshRate->removeItem(i - 1);
    }

    for (int i = 0, total = modes.count(); i < total; ++i) {
        const KScreen::ModePtr mode = modes.at(i);
        mRefreshRate->addItem(tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())), mode->id());
        // If selected refresh rate is other then what we consider the "Auto" value
        // - that is it's not the highest resolution - then select it, otherwise
        // we stick with "Auto"
        if (mode == selectedMode && mRefreshRate->count() > 1) {
            // i + 1 since 0 is auto
            mRefreshRate->setCurrentIndex(i + 1);
        }
    }

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
//    auto scale = mScale->itemData(index).toInt();
//    mOutput->setScale(scale);
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

QStringList OutputConfig::readFile(const QString& filepath) {
    QFile file(filepath);
    if(file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while(!textStream.atEnd()) {
            QString line= textStream.readLine();
            line.remove('\n');
            this->proRes<<line;
        }
        file.close();
        return this->proRes;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return QStringList();
    }
}

