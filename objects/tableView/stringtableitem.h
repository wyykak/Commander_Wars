#pragma once

#include "objects/tableView/basetableitem.h"
#include <QObject>

class StringTableItem : public BaseTableItem
{
    Q_OBJECT
public:
    explicit StringTableItem(const QString& value, qint32 itemWidth, QObject *parent = nullptr);

    virtual bool operator>(const BaseTableItem& rhs) const override
    {
        return m_value > static_cast<const StringTableItem&>(rhs).m_value;
    }
private:
    QString m_value;
};

