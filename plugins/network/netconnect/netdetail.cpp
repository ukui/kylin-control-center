#include "netdetail.h"

NetDetail::NetDetail(bool isWlan, QWidget *parent) : mIsWlan(isWlan), QFrame(parent) {
    this->setFrameShape(QFrame::Shape::Box);
    this->setMaximumWidth(960);
    initUI();
}

void NetDetail::setSSID(const QString &ssid) {
    this->mSSID->setText(ssid);
}

void NetDetail::setProtocol(const QString &protocol) {
    this->mProtocol->setText(protocol);
}

void NetDetail::setSecType(const QString &secType) {
    this->mSecType->setText(secType);
}

void NetDetail::setHz(const QString &hz) {
    this->mHz->setText(hz);
}

void NetDetail::setChan(const QString &chan) {
    this->mChan->setText(chan);
}

void NetDetail::setSpeed(const QString &speed) {
    this->mSpeed->setText(speed);
}

void NetDetail::setBandWidth(const QString &bd) {
    this->mBandWidth->setText(bd);
}

void NetDetail::setIPV4(const QString &ipv4) {
    this->mIPV4->setText(ipv4);
}

void NetDetail::setIPV4Dns(const QString &ipv4Dns) {
    this->mIPV4Dns->setText(ipv4Dns);
}

void NetDetail::setIPV4Mask(const QString &netMask) {
    this->mIPV4Mask->setText(netMask);
}

void NetDetail::setIPV4Gateway(const QString &gateWay) {
    this->mIPV4Gt->setText(gateWay);
}

void NetDetail::setIPV6(const QString &ipv6) {
    this->mIPV6->setText(ipv6);
}

void NetDetail::setIPV6Prefix(const QString &prefix) {
    this->mIPV6Prefix->setText(prefix);
}

void NetDetail::setIPV6Gt(const QString &gateWay) {
    this->mIPV6Gt->setText(gateWay);
}

void NetDetail::setMac(const QString &mac) {
    this->mMac->setText(mac);
}

void NetDetail::initUI() {

    mDetailLayout = new QFormLayout(this);
    mDetailLayout->setContentsMargins(41, 0, 0, 0);

    mSSID      = new QLabel(this);
    mProtocol  = new QLabel(this);
    mSecType   = new QLabel(this);
    mHz        = new QLabel(this);
    mChan      = new QLabel(this);
    mSpeed     = new QLabel(this);

    mBandWidth = new QLabel(this);
    mIPV4      = new QLabel(this);
    mIPV4Dns   = new QLabel(this);
    mIPV4Gt    = new QLabel(this);
    mIPV4Mask  = new QLabel(this);

    mIPV6      = new QLabel(this);
    mIPV6Prefix= new QLabel(this);
    mIPV6Gt    = new QLabel(this);
    mMac       = new QLabel(this);


    mDetailLayout->addRow(tr("SSID:"), mSSID);
    mDetailLayout->addRow(tr("Protocol"), mProtocol);
    if (mIsWlan) {
        mDetailLayout->addRow(tr("Security Type:"), mSecType);
        mDetailLayout->addRow(tr("Hz:"), mHz);
        mDetailLayout->addRow(tr("Chan:"), mChan);
        mDetailLayout->addRow(tr("Link Speed(rx/tx):"), mSpeed);
    }

    mDetailLayout->addRow(tr("BandWidth:"), mBandWidth);
    mDetailLayout->addRow(tr("IPV4:"), mIPV4);
    mDetailLayout->addRow(tr("IPV4 Dns:"), mIPV4Dns);
    mDetailLayout->addRow(tr("IPV4 GateWay:"), mIPV4Gt);
    mDetailLayout->addRow(tr("IPV4 Prefix:"), mIPV4Mask);
    mDetailLayout->addRow(tr("IPV6:"), mIPV6);
    mDetailLayout->addRow(tr("IPV6 Prefix:"), mIPV6Prefix);
    mDetailLayout->addRow(tr("IPV6 GateWay:"), mIPV6Gt);
    mDetailLayout->addRow(tr("Mac:"), mMac);

}
