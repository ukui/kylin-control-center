#include "unifiedoutputconfig.h"
#include "resolutionslider.h"
#include "utils.h"

#include <QComboBox>
#include <QIcon>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QStyledItemDelegate>
#include <QFile>
#include <QVector>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/config.h>
#include <KF5/KScreen/kscreen/getconfigoperation.h>

const QVector<QSize> k150Scale{QSize(1280, 1024), QSize(1440, 900), QSize(1600, 900),
                               QSize(1680, 1050), QSize(1920, 1080), QSize(1920, 1200),
                               QSize(2048, 1080), QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440),QSize(3840, 2160)};

const QVector<QSize> k175Scale{QSize(1680, 1050), QSize(1920, 1080), QSize(1920, 1200),
                               QSize(2048, 1080), QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440), QSize(3840, 2160)};

const QVector<QSize> k200Scale{QSize(1920, 1200), QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440), QSize(3840, 2160)};

bool operator<(const QSize &s1, const QSize &s2)
{
    return s1.width() * s1.height() < s2.width() * s2.height();
}

template<>
bool qMapLessThanKey(const QSize &s1, const QSize &s2)
{
    return s1 < s2;
}

UnifiedOutputConfig::UnifiedOutputConfig(const KScreen::ConfigPtr &config, QWidget *parent) :
    OutputConfig(parent),
    mConfig(config)
{
}

UnifiedOutputConfig::~UnifiedOutputConfig()
{
}

void UnifiedOutputConfig::setOutput(const KScreen::OutputPtr &output)
{
    mOutput = output;

    mClones.clear();
    mClones.reserve(mOutput->clones().count());
    Q_FOREACH (int id, mOutput->clones()) {
        mClones << mConfig->output(id);
    }
    mClones << mOutput;

    OutputConfig::setOutput(output);
}

void UnifiedOutputConfig::initUi()
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    KScreen::OutputPtr fakeOutput = createFakeOutput();
    mResolution = new ResolutionSlider(fakeOutput, this);

    mResolution->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mResolution->setMinimumSize(402, 30);

    connect(mOutput.data(), &KScreen::Output::currentModeIdChanged,
            this, &UnifiedOutputConfig::slotRestoreResoltion);

    connect(mOutput.data(), &KScreen::Output::rotationChanged,
            this, &UnifiedOutputConfig::slotRestoreRatation);

    QLabel *resLabel = new QLabel(this);
    resLabel->setText(tr("resolution"));
    resLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    resLabel->setMinimumSize(118, 30);
    resLabel->setMaximumSize(118, 30);

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
            this, &UnifiedOutputConfig::slotResolutionChanged);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        connect(mResolution, &ResolutionSlider::resolutionChanged,
                this, &UnifiedOutputConfig::slotScaleIndex);
    #endif

    // 方向下拉框
    mRotation = new QComboBox(this);

    mRotation->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mRotation->setMinimumSize(402, 30);
    mRotation->setMaximumSize(16777215, 30);

    QLabel *rotateLabel = new QLabel(this);
    rotateLabel->setText(tr("orientation"));
    rotateLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rotateLabel->setMinimumSize(118, 30);
    rotateLabel->setMaximumSize(118, 30);

    mRotation->addItem(tr("arrow-up"), KScreen::Output::None);
    mRotation->addItem(tr("90° arrow-right"), KScreen::Output::Right);
    mRotation->addItem(tr("arrow-down"), KScreen::Output::Inverted);
    mRotation->addItem(tr("90° arrow-left"), KScreen::Output::Left);

    int index = mRotation->findData(mOutput->rotation());
    mRotation->setCurrentIndex(index);

    connect(mRotation, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &UnifiedOutputConfig::slotRotationChangedDerived);

    QHBoxLayout *roatateLayout = new QHBoxLayout();
    roatateLayout->addWidget(rotateLabel);
    roatateLayout->addWidget(mRotation);

    QFrame *rotateFrame = new QFrame(this);
    rotateFrame->setFrameShape(QFrame::Shape::Box);
    rotateFrame->setLayout(roatateLayout);

    rotateFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rotateFrame->setMinimumSize(552, 50);
    rotateFrame->setMaximumSize(960, 50);

    vbox->addWidget(rotateFrame);

    // 统一输出刷新率下拉框
    mRefreshRate = new QComboBox(this);
    mRefreshRate->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mRefreshRate->setMinimumSize(402, 30);
    mRefreshRate->setMaximumSize(16777215, 30);

    QLabel *freshLabel = new QLabel(this);
    freshLabel->setText(tr("frequency"));
    freshLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    freshLabel->setMinimumSize(118, 30);
    freshLabel->setMaximumSize(118, 30);

    mRefreshRate->addItem(tr("auto"), -1);

    QHBoxLayout *freshLayout = new QHBoxLayout();
    freshLayout->addWidget(freshLabel);
    freshLayout->addWidget(mRefreshRate);

    QFrame *freshFrame = new QFrame(this);
    freshFrame->setFrameShape(QFrame::Shape::Box);
    freshFrame->setLayout(freshLayout);

    vbox->addWidget(freshFrame);

    freshFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    freshFrame->setMinimumSize(552, 50);
    freshFrame->setMaximumSize(960, 50);

    slotResolutionChanged(mResolution->currentResolution());
    connect(mRefreshRate, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
        this, &UnifiedOutputConfig::slotRefreshRateChanged);

    QObject::connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
                         [&](KScreen::ConfigOperation *op) {
        KScreen::ConfigPtr sConfig = qobject_cast<KScreen::GetConfigOperation *>(op)->config();
        KScreen::OutputPtr sOutput = sConfig -> primaryOutput();

        for (int i = 0; i < mRefreshRate->count(); ++i) {
            if (mRefreshRate->itemText(i) == tr("%1 Hz").arg(QLocale().toString(sOutput->currentMode()->refreshRate()))) {
                mRefreshRate->setCurrentIndex(i);
            }
        }
    });
    initscale(vbox);
}

void UnifiedOutputConfig::slotScaleIndex(const QSize &size)
{
    mScaleCombox->clear();
    mScaleCombox->addItem("100%", 1.0);

    if (k150Scale.contains(size)) {
        mScaleCombox->addItem("125%", 1.25);
        mScaleCombox->addItem("150%", 1.5);
    }
    if (k175Scale.contains(size)) {
        mScaleCombox->addItem("175%", 1.75);
    }
    if (k200Scale.contains(size)) {
        mScaleCombox->addItem("200%", 2.0);
    }
}

void UnifiedOutputConfig::initscale(QVBoxLayout *vbox)
{
    mScaleCombox = new QComboBox(this);
    mScaleCombox->setObjectName("scaleCombox");

    double scale = getScreenScale();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    slotScaleIndex(mResolution->currentResolution());
#else
    int maxReslu = mResolution->getMaxResolution().width();
    mScaleCombox->addItem("100%", 1.0);
    if (maxReslu >= 2000) {
        mScaleCombox->addItem("200%", 2.0);
    }

    mScaleCombox->setCurrentIndex(0);
    if (mScaleCombox->findData(scale) == -1) {
        mScaleCombox->addItem("200%", 2.0);
    }
#endif
    mScaleCombox->setCurrentText(QString::number(scale * 100) + "%");

    if (mScaleCombox->findData(scale) == -1) {
        mScaleCombox->addItem(QString::number(scale * 100) + "%", scale);
        mScaleCombox->setCurrentText(QString::number(scale * 100) + "%");
    }

    connect(mScaleCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &UnifiedOutputConfig::slotScaleChanged);

    QLabel *scaleLabel = new QLabel(this);
    //~ contents_path /display/screen zoom
    scaleLabel->setText(tr("screen zoom"));
    scaleLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    scaleLabel->setFixedSize(118, 30);

    QHBoxLayout *scaleLayout = new QHBoxLayout();
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(mScaleCombox);

    QFrame *scaleFrame = new QFrame(this);
    scaleFrame->setFrameShape(QFrame::Shape::Box);
    scaleFrame->setLayout(scaleLayout);

    scaleFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    scaleFrame->setMinimumSize(550, 50);
    scaleFrame->setMaximumSize(960, 50);
    vbox->addWidget(scaleFrame);
}

void UnifiedOutputConfig::slotScaleChanged(int index)
{
    Q_EMIT scaleChanged(mScaleCombox->itemData(index).toDouble());
}

KScreen::OutputPtr UnifiedOutputConfig::createFakeOutput()
{
    // Find set of common resolutions
    QMap<QSize, int> commonSizes;
    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        QList<QSize> processedSizes;
        Q_FOREACH (const KScreen::ModePtr &mode, clone->modes()) {
            // Make sure we don't count some modes multiple times because of different
            // refresh rates
            if (processedSizes.contains(mode->size())) {
                continue;
            }

            processedSizes << mode->size();

            if (commonSizes.contains(mode->size())) {
                commonSizes[mode->size()]++;
            } else {
                commonSizes.insert(mode->size(), 1);
            }
        }
    }

    KScreen::OutputPtr fakeOutput(new KScreen::Output);

    // This will give us list of resolution that are shared by all outputs
    QList<QSize> commonResults = commonSizes.keys(mClones.count());
    // If there are no common resolution, fallback to smallest preferred mode
    if (commonResults.isEmpty()) {
        QSize smallestMode;
        Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
            if (!smallestMode.isValid() || clone->preferredMode()->size() < smallestMode) {
                smallestMode = clone->preferredMode()->size();
            }
        }
        commonResults << smallestMode;
    }
    std::sort(commonResults.begin(), commonResults.end());

    KScreen::ModeList modes;
    Q_FOREACH (const QSize &size, commonResults) {
        KScreen::ModePtr mode(new KScreen::Mode);
        mode->setSize(size);
        mode->setId(Utils::sizeToString(size));
        mode->setName(mode->id());
        modes.insert(mode->id(), mode);
    }
    fakeOutput->setModes(modes);
    if (!mOutput->currentModeId().isEmpty()) {
        fakeOutput->setCurrentModeId(Utils::sizeToString(mOutput->currentMode()->size()));
    } else {
        fakeOutput->setCurrentModeId(Utils::sizeToString(commonResults.last()));
    }

    return fakeOutput;
}

void UnifiedOutputConfig::slotResolutionChanged(const QSize &size)
{
    // Ignore disconnected outputs
    if (!size.isValid()) {
        return;
    }
    QVector<QString>Vrefresh;
    for (int i = mRefreshRate->count(); i >= 0; --i) {
            mRefreshRate->removeItem(i);
    }
    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        const QString &id = findBestMode(clone, size);
        if (id.isEmpty()) {
            // FIXME: Error?
            return;
        }

        clone->setCurrentModeId(id);
        clone->setPos(QPoint(0, 0));

        QList<KScreen::ModePtr> modes;
        Q_FOREACH (const KScreen::ModePtr &mode, clone->modes()) {
            if (mode->size() == size) {
                modes << mode;
            }
        }

        QVector<QString>VrefreshTemp;
        for (int i = 0, total = modes.count(); i < total; ++i) {
           const KScreen::ModePtr mode = modes.at(i);

           bool alreadyExisted = false; //判断该显示器的刷新率是否有重复的，确保同一刷新率在一个屏幕上只出现一次
           for (int j = 0; j < VrefreshTemp.size(); ++j) {
               if (tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())) == VrefreshTemp[j]) {
                   alreadyExisted = true;
                   break;
               }
           }
           if (alreadyExisted == false) {   //不添加重复的项
               VrefreshTemp.append(tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())));
           }
        }

        for (int i = 0; i < VrefreshTemp.size(); ++i) {
            Vrefresh.append(VrefreshTemp[i]);
        }
    }

    for (int i = 0; i < Vrefresh.size(); ++i) {
        if (Vrefresh.count(Vrefresh[i]) == mClones.size()) { //该刷新率出现次数等于屏幕数，即每个屏幕都有该刷新率
            bool existFlag = false;
            for (int j = 0; j < mRefreshRate->count(); ++j) {  //已经存在就不再添加
                if (Vrefresh[i] == mRefreshRate->itemText(j)) {
                    existFlag = true;
                    break;
                }
            }
            if (existFlag == false) {  //不存在添加到容器中
                mRefreshRate->addItem(Vrefresh[i]);
            }
        }
    }
    if (mRefreshRate->count() == 0) {
        mRefreshRate->addItem(tr("auto"), -1);    
    }
    Q_EMIT changed();
}

void UnifiedOutputConfig::slotRefreshRateChanged(int index)
{
    if (index == 0) {
        index = 1;
    }
    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        Q_FOREACH (const KScreen::ModePtr &mode, clone->modes()) {
            if (mode->size() == mResolution->currentResolution() && \
                    tr("%1 Hz").arg(QLocale().toString(mode->refreshRate())) == mRefreshRate->itemText(index)) {
                clone->setCurrentModeId(mode->id());
            }
        }
    }
    Q_EMIT changed();
}

QString UnifiedOutputConfig::findBestMode(const KScreen::OutputPtr &output, const QSize &size)
{
    float refreshRate = 0;
    QString id;
    Q_FOREACH (const KScreen::ModePtr &mode, output->modes()) {
        if (mode->size() == size && mode->refreshRate() > refreshRate) {
            refreshRate = mode->refreshRate();
            id = mode->id();
        }
    }
    return id;
}

// 统一输出方向信号改变
void UnifiedOutputConfig::slotRotationChangedDerived(int index)
{
    KScreen::Output::Rotation rotation = static_cast<KScreen::Output::Rotation>(mRotation->itemData(index).toInt());
    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        if (clone->isConnected() && clone->isEnabled()) {
            clone->setRotation(rotation);
            clone->setPos(QPoint(0, 0));
        }
    }
    Q_EMIT changed();
}

void UnifiedOutputConfig::slotRestoreResoltion()
{
    if (!(mResolution->currentResolution() == mOutput->currentMode()->size())) {
        mResolution->setResolution(mOutput->currentMode()->size());
    }
}

void UnifiedOutputConfig::slotRestoreRatation()
{
    mRotation->setCurrentIndex(mRotation->findData(mOutput->rotation()));
}
