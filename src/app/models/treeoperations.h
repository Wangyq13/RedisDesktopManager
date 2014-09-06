#pragma once

#include <QSharedPointer>
#include <QObject>
#include "redisclient/connection.h"
#include "connections-tree/operations.h"

class TreeOperations : public QObject, public ConnectionsTree::Operations
{
    Q_OBJECT
public:
    TreeOperations(QSharedPointer<RedisClient::Connection> connection);

    void getDatabases(std::function<void(DatabaseList)>) override;

    void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>) override;

    void disconnect() override;

    QString getNamespaceSeparator() override;    

    virtual void openKeyTab() override;

    virtual void openNewKeyTab() override;

    virtual void openConsoleTab() override;

private:
     QSharedPointer<RedisClient::Connection> m_connection;
};
