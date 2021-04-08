#ifndef NETDETAIL_H
#define NETDETAIL_H

#include <QWidget>
#include <QFrame>
#include <QFormLayout>
#include <QLabel>

class NetDetail : public QFrame
{
public:
    NetDetail(bool isWlan, QWidget *parent = nullptr);

    void setSSID(const QString &ssid);
    void setProtocol(const QString &protocol);
    void setSecType(const QString &secType);
    void setHz(const QString &hz);
    void setChan(const QString &chan);
    void setBandWidth(const QString &chan);
    void setIPV4(const QString &ipv4);
    void setIPV4Dns(const QString &ipv4Dns);
    void setIPV4Mask(const QString &netMask);
    void setIPV4Gateway(const QString &gateWay);
    void setIPV6(const QString &ipv6);
    void setIPV6Prefix(const QString &prefix);
    void setIPV6Gt(const QString &gateWay);
    void setMac(const QString &mac);

private:
    void initUI();

public:
    QLabel *mSSID;
    QLabel *mProtocol;
    QLabel *mSecType;
    QLabel *mHz;
    QLabel *mChan;
    QLabel *mBandWidth;
    QLabel *mIPV4;
    QLabel *mIPV4Gt;
    QLabel *mIPV4Dns;
    QLabel *mIPV4Mask;

    QLabel *mIPV6;
    QLabel *mIPV6Prefix;
    QLabel *mIPV6Gt;

    QLabel *mMac;

private:
    QFormLayout *mDetailLayout;
    bool mIsWlan;
};

#endif // NETDETAIL_H
