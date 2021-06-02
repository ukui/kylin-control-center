#ifndef RESOLUTIONSLIDER_H
#define RESOLUTIONSLIDER_H

#include <QWidget>
#include <QSet>

#include <KF5/KScreen/kscreen/output.h>
#include "Uslider/uslider.h"

class QSlider;
class QLabel;
class QComboBox;
class QStyledItemDelegate;

class ResolutionSlider : public QWidget
{
    Q_OBJECT

public:
    explicit ResolutionSlider(const KScreen::OutputPtr &output, QWidget *parent = nullptr);
    ~ResolutionSlider() override;

    QSize currentResolution() const;
    QSize getMaxResolution() const;

    void setResolution(const QSize &size);

Q_SIGNALS:
    void resolutionChanged(const QSize &size);

public Q_SLOTS:
    void slotValueChanged(int);
    void slotOutputModeChanged();

private:
    void init();

private:
    KScreen::OutputPtr mOutput;

    QList<QSize> mModes;
    QList<QSize> mExcludeModes;

    QLabel *mSmallestLabel = nullptr;
    QLabel *mBiggestLabel = nullptr;
    QLabel *mCurrentLabel = nullptr;
    Uslider *mSlider = nullptr;
    QComboBox *mComboBox = nullptr;

    bool mIsWayland = false;
};

#endif // RESOLUTIONSLIDER_H
