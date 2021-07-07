#include "resolutionslider.h"
#include "utils.h"
#include "scalesize.h"

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QFile>
#include <QStyledItemDelegate>

#include <QDBusInterface>

#include <KF5/KScreen/kscreen/output.h>

static bool sizeLessThan(const QSize &sizeA, const QSize &sizeB)
{
    return sizeA.width() * sizeA.height() < sizeB.width() * sizeB.height();
}

ResolutionSlider::ResolutionSlider(const KScreen::OutputPtr &output, QWidget *parent) :
    QWidget(parent),
    mOutput(output)
{
    QString sessionType = getenv("XDG_SESSION_TYPE");
    if (sessionType.compare("wayland", Qt::CaseSensitive)) {
       mExcludeModes.push_back(QSize(1152, 864));
    }

    connect(output.data(), &KScreen::Output::currentModeIdChanged,
            this, &ResolutionSlider::slotOutputModeChanged);

    connect(output.data(), &KScreen::Output::modesChanged,
            this, &ResolutionSlider::init);
    init();
}

ResolutionSlider::~ResolutionSlider()
{
}

void ResolutionSlider::init()
{
    this->setMinimumSize(402, 30);
    this->setMaximumSize(1677215, 30);
    mModes.clear();
    Q_FOREACH (const KScreen::ModePtr &mode, mOutput->modes()) {
        if (mModes.contains(mode->size())
            || mode->size().width() * mode->size().height() < 1024 * 768
            || mExcludeModes.contains(mode->size())
            || mode->size().width() < 1024) {
            continue;
        }
        mModes << mode->size();
    }
    std::sort(mModes.begin(), mModes.end(), sizeLessThan);

    delete layout();
    delete mComboBox;
    mComboBox = nullptr;

    QGridLayout *layout = new QGridLayout(this);
    // Avoid double margins
    layout->setContentsMargins(0, 0, 0, 0);
    if (!mModes.empty()) {
        std::reverse(mModes.begin(), mModes.end());
        mComboBox = new QComboBox(this);
        mComboBox->setMinimumSize(402, 30);
        mComboBox->setMaximumSize(1677215, 30);

        int currentModeIndex = -1;
        int preferredModeIndex = -1;
        Q_FOREACH (const QSize &size, mModes) {
#ifdef __sw_64__
            if (size.width() < int(1920)) {
                continue;
            }
#endif
            mComboBox->addItem(Utils::sizeToString(size));

            if (mOutput->currentMode() && (mOutput->currentMode()->size() == size)) {
                currentModeIndex = mComboBox->count() - 1;
            } else if (mOutput->preferredMode() && (mOutput->preferredMode()->size() == size)) {
                preferredModeIndex = mComboBox->count() - 1;
            }
        }
        if (currentModeIndex != -1) {
            mComboBox->setCurrentIndex(currentModeIndex);
        } else if (preferredModeIndex != -1) {
            mComboBox->setCurrentIndex(preferredModeIndex);
        }

        layout->addWidget(mComboBox, 0, 0, 1, 1);
        connect(mComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &ResolutionSlider::slotValueChanged, Qt::UniqueConnection);
        Q_EMIT resolutionChanged(mModes.at(mComboBox->currentIndex()), false);
    }
}

QSize ResolutionSlider::currentResolution() const
{
    if (mModes.isEmpty()) {
        return QSize();
    }

    if (mModes.size() < 2) {
        return mModes.first();
    }

    const int i = mComboBox->currentIndex();
    return i > -1 ? mModes.at(i) : QSize();
}

QSize ResolutionSlider::getMaxResolution() const
{
    if (mModes.isEmpty()) {
        return QSize();
    }
    return mModes.first();
}

void ResolutionSlider::setResolution(const QSize &size)
{
    mComboBox->blockSignals(true);
    mComboBox->setCurrentIndex(mModes.indexOf(size));
    mComboBox->blockSignals(false);
}

void ResolutionSlider::slotOutputModeChanged()
{
    if (!mOutput->currentMode()) {
        return;
    }

    if (mComboBox) {
        mComboBox->blockSignals(true);
        mComboBox->setCurrentIndex(mModes.indexOf(mOutput->currentMode()->size()));
        mComboBox->blockSignals(false);
    }
    Q_EMIT resolutionsave(mModes.at(mComboBox->currentIndex()));
}

void ResolutionSlider::slotValueChanged(int value)
{
    const QSize &size = mModes.at(value);
    Q_EMIT resolutionChanged(size);
}
