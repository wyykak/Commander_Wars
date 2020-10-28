#ifndef PROXYAI_H
#define PROXYAI_H

#include <QObject>
#include <QList>
#include <QMutex>
#include "game/gameaction.h"
#include "coreai.h"
#include "coreengine/mainapp.h"

#include "network/NetworkInterface.h"

class ProxyAi : public CoreAI
{
    Q_OBJECT
public:
    ProxyAi();

    virtual void init() override;
    /**
     * @brief serialize stores the object
     * @param pStream
     */
    virtual void serializeObject(QDataStream& pStream) const override;
    /**
     * @brief deserialize restores the object
     * @param pStream
     */
    virtual void deserializeObject(QDataStream& pStream) override;
    /**
     * @brief getVersion version of the file
     * @return
     */
    virtual qint32 getVersion() const override
    {
        return 1;
    }
    /**
     * @brief process empty impl
     */
    virtual void process() override {}
    /**
     * @brief setInterface
     * @param pNetworkInterface
     */
    void connectInterface(NetworkInterface* pNetworkInterface);
    void disconnectInterface();
public slots:
    virtual void nextAction() override;
    void recieveData(quint64 socket, QByteArray data, NetworkInterface::NetworkSerives service);

protected:

private:
    QList<spGameAction> m_ActionBuffer;
    bool actionRunning{false};
    QMutex m_ActionMutex;
};

#endif // PROXYAI_H
