#include "ICPHttpResult.h"
#include "ICPHttpClient.h"

const int ICPHttpResult::OK = 200;

ICPHttpResult::ICPHttpResult()
{

}

ICPHttpResult::ICPHttpResult(ICPHttpClient* client)
{
    _success = true;
    _result = client->getResult();
    _error = client->getError();
    _url = client->getUrl();
    _returnCode = client->getHttpReturnCode();

    if(client->getHasError()) {
        _error = client->getError();
        _success = false;
    } else {
        if(!client->isOk()) {
            _success = false;
            _error = "return http code: " + client->getHttpReturnCode();
        } else {
            _success = true;
        }
    }
}
