#ifndef FRONTENDSERVICES_H
#define FRONTENDSERVICES_H

#include "service.h"
#include "datacontracts/frontendStatus.h"
#include "datacontracts/frontendActionList.h"

class SERVICE_PUBLIC FrontendServices : public Service
{
    Q_OBJECT
    Q_CLASSINFO("version", "1.0");

  public:
    FrontendServices(QObject *parent = 0) : Service(parent)
    {
        DTC::FrontendStatus::InitializeCustomTypes();
        DTC::FrontendActionList::InitializeCustomTypes();
    }

  public slots:
    virtual DTC::FrontendStatus* GetStatus(void) = 0;
    virtual bool                 SendMessage(const QString &Message) = 0;
    virtual bool                 SendAction(const QString &Action,
                                            const QString &Value,
                                            uint Width, uint Height) = 0;
    virtual QStringList          GetContextList(void) = 0;
    virtual DTC::FrontendActionList* GetActionList(const QString &Context) = 0;


};

#endif // FRONTENDSERVICES_H
