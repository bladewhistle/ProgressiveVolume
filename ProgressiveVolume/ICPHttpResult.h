#pragma once

#include <QMetaType>

class ICPHttpClient;

class ICPHttpResult
{
public:
    ICPHttpResult();
    ICPHttpResult(ICPHttpClient* client);

    static const int OK;

    bool getSuccess() const
    {
        return _success;
    }

    QString getResult() const
    {
        return _result;
    }

    QString getError() const
    {
        return _error;
    }

    QString getUrl()  const
    {
        return _url;
    }
    int getReturnCode() const
    {
        return _returnCode;
    }
protected:
    bool _success;
    int _returnCode;
    QString _url;
    QString _result;
    QString _error;
};

Q_DECLARE_METATYPE(ICPHttpResult)
