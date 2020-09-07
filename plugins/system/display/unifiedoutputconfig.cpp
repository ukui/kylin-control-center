#include "unifiedoutputconfig.h"
#include "resolutionslider.h"
#include "utils.h"
//#include "kcm_screen_debug.h"

#include <QComboBox>
#include <QIcon>
//#include <KLocalizedString>

#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStyledItemDelegate>
#include <QFile>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/config.h>


bool operator<(const QSize &s1, const QSize &s2)
{
    return s1.width() * s1.height() < s2.width() * s2.height();
}

template<>
bool qMapLessThanKey(const QSize &s1, const QSize &s2)
{
    return s1 < s2;
}


UnifiedOutputConfig::UnifiedOutputConfig(const KScreen::ConfigPtr &config, QWidget *parent)
    : OutputConfig(parent)
    , mConfig(config)
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
    vbox->setContentsMargins(0,0,0,0);
    //mTitle = new QLabel(this);
  //  mTitle->setAlignment(Qt::AlignHCenter);
//    vbox->addWidget(mTitle);

 //   setTitle(i18n("统一输出"));
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
     vbox->setSpacing(2);//间距问题与outputconfig相同

    //vbox->addStretch(0);

    KScreen::OutputPtr fakeOutput = createFakeOutput();
    mResolution = new ResolutionSlider(fakeOutput, this);



    //统一输出分辨率下拉框

    mResolution->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mResolution->setMinimumSize(402,30);
//    mResolution->setMaximumSize(402,30);

    QLabel *resLabel = new QLabel(this);
    resLabel->setText(tr("resolution"));
    resLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    resLabel->setMinimumSize(118,30);
    resLabel->setMaximumSize(118,30);

    QHBoxLayout *resLayout = new QHBoxLayout();
    resLayout->addWidget(resLabel);
    resLayout->addWidget(mResolution);
//    resLayout->addStretch();

    QFrame *resFrame = new QFrame(this);
    resFrame->setFrameShape(QFrame::Shape::Box);
    resFrame->setLayout(resLayout);
//    resWidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    mResolution->setStyleSheet("background-color:#F8F9F9");

    resFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    resFrame->setMinimumSize(552,50);
    resFrame->setMaximumSize(960,50);


    vbox->addWidget(resFrame);
    connect(mResolution, &ResolutionSlider::resolutionChanged,
            this, &UnifiedOutputConfig::slotResolutionChanged);


    slotResolutionChanged(mResolution->currentResolution());

    //方向下拉框
    mRotation = new QComboBox();
    connect(mRotation, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &UnifiedOutputConfig::slotRotationChanged);

    mRotation->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mRotation->setMinimumSize(402,30);
    mRotation->setMaximumSize(16777215,30);
//    mRotation->setStyleSheet(qss);
//    mRotation->setItemDelegate(itemDelege);
//    mRotation->setMaxVisibleItems(5);


    QLabel *rotateLabel = new QLabel(this);
    rotateLabel->setText(tr("orientation"));
    rotateLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateLabel->setMinimumSize(118,30);
    rotateLabel->setMaximumSize(118,30);

    connect(mRotation, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &UnifiedOutputConfig::slotRotationChangedDerived);

    mRotation->addItem(tr("arrow-up"), KScreen::Output::None);
    mRotation->addItem(tr("90° arrow-right"), KScreen::Output::Right);
    mRotation->addItem(tr("arrow-down"), KScreen::Output::Inverted);
    mRotation->addItem(tr("90° arrow-left"), KScreen::Output::Left);


    QHBoxLayout *roatateLayout = new QHBoxLayout();
    roatateLayout->addWidget(rotateLabel);
    roatateLayout->addWidget(mRotation);


    QFrame *rotateFrame = new QFrame(this);
    rotateFrame->setFrameShape(QFrame::Shape::Box);
    rotateFrame->setLayout(roatateLayout);
//    rotateWidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    mRotation->setStyleSheet("background-color:#F8F9F9");

    rotateFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rotateFrame->setMinimumSize(552,50);
    rotateFrame->setMaximumSize(960,50);


    vbox->addWidget(rotateFrame);


    //统一输出刷新率下拉框
    mRefreshRate = new QComboBox(this);
    mRefreshRate->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mRefreshRate->setMinimumSize(402,30);
    mRefreshRate->setMaximumSize(16777215,30);

    QLabel *freshLabel = new QLabel(this);
    freshLabel->setText(tr("refresh rate"));
    freshLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    freshLabel->setMinimumSize(118,30);
    freshLabel->setMaximumSize(118,30);

    mRefreshRate->addItem(tr("auto"), -1);
    mRefreshRate->addItem(tr("aa"), -1);


    QHBoxLayout *freshLayout = new QHBoxLayout();
    freshLayout->addWidget(freshLabel);
    freshLayout->addWidget(mRefreshRate);

    QFrame *freshFrame = new QFrame(this);
    freshFrame->setFrameShape(QFrame::Shape::Box);
    freshFrame->setLayout(freshLayout);
//    freshFrame->setStyleSheet("background-color:#F4F4F4;border-radius:6px");

    vbox->addWidget(freshFrame);

    freshFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    freshFrame->setMinimumSize(552,50);
    freshFrame->setMaximumSize(960,50);

    mRefreshRate->setEnabled(false);


    //缩放
       //================================================
       scaleCombox = new QComboBox();
   //    mRefreshRate->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
       scaleCombox->setMinimumSize(402,30);
       scaleCombox->setMaximumSize(16777215,30);



       int maxReslu = mResolution->getMaxResolution().width();

   #if QT_VERSION < QT_VERSION_CHECK(5,7,0)
       if (maxReslu >= 2000) {
           scaleCombox->addItem(tr("200%"));
       } else {
           scaleCombox->addItem(tr("100%"));
       }
   #else
       scaleCombox->addItem(tr("100%"));
       if (maxReslu >= 2000 && maxReslu <= 3800) {
           scaleCombox->addItem(tr("200%"));
       } else if (maxReslu >= 3800 || maxReslu >= 4000) {
           scaleCombox->addItem(tr("200%"));
           scaleCombox->addItem(tr("300%"));
       }
   #endif

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
   //    scaleWidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");

       scaleFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
       scaleFrame->setMinimumSize(550,50);
       scaleFrame->setMaximumSize(960,50);
       vbox->addWidget(scaleFrame);

       int scale = getScreenScale();
   #if QT_VERSION < QT_VERSION_CHECK(5,7,0)

   #else
       scaleCombox->setCurrentIndex(0);
       if (scale <= scaleCombox->count() && scale > 0) {
   //        qDebug()<<"scale is----->"<<scale<<endl;
           scaleCombox->setCurrentIndex(scale - 1);
       }
       slotScaleChanged(scale - 1);
   #endif

       connect(scaleCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
               this, &OutputConfig::slotScaleChanged);

       //========================================
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
            //qCDebug(KSCREEN_KCM) << smallestMode << clone->preferredMode()->size();
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
    fakeOutput->setCurrentModeId(Utils::sizeToString(commonResults.last()));
    return fakeOutput;
}

void UnifiedOutputConfig::slotResolutionChanged(const QSize &size)
{
   // Ignore disconnected outputs
    if (!size.isValid()) {
        return;
    }

    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        const QString &id = findBestMode(clone, size);
        if (id.isEmpty()) {
            // FIXME: Error?
            return;
        }

        clone->setCurrentModeId(id);
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

//统一输出方向信号改变
void UnifiedOutputConfig::slotRotationChangedDerived(int index){
    KScreen::Output::Rotation rotation =
        static_cast<KScreen::Output::Rotation>(mRotation->itemData(index).toInt());
    auto mainOutput = mConfig->primaryOutput();
    //qDebug()<<"首屏幕输出--->"<<mainOutput<<endl;
    Q_FOREACH (const KScreen::OutputPtr &clone, mClones) {
        if(clone->isConnected()&&clone->isEnabled()){
            //mOutput = clone;

            mainOutput->setRotation(rotation);
          //  clone->setRotation(rotation);
            //qDebug()<<"是不是首屏------->"<<clone->isPrimary()<<endl;
            if(!clone->isPrimary()){
               // qDebug()<<"非首屏------>"<<clone<<endl;
//                KScreen::OutputPtr output = clone;
                clone->setRotation(rotation);
                //output->apply(mainOutput);
                //clone->apply(mainOutput);
            }
        }
    }
    Q_EMIT changed();
}


