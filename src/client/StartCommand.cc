#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <errno.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include <string>
#include <sstream>
#include <list>
#include <exception>

#include "src/common/errors/errors.h"
#include "src/common/exception/OpenLTFSException.h"
#include "src/common/messages/Message.h"
#include "src/common/tracing/Trace.h"
#include "src/common/const/Const.h"

#include "src/common/comm/ltfsdm.pb.h"
#include "src/common/comm/LTFSDmComm.h"

#include "OpenLTFSCommand.h"
#include "StartCommand.h"

/** @page ltfsdm_start ltfsdm start
    The ltfsdm start command starts the OpenLTFS service.

    <tt>@LTFSDMC0006I</tt>

    parameters | description
    ---|---
    - | -

    Example:

    @verbatim
    [root@visp ~]# ltfsdm start
    LTFSDMC0099I(0073): Starting the OpenLTFS backend service.
    LTFSDMX0029I(0062): OpenLTFS version: 0.0.624-master.2017-11-09T10.57.51
    LTFSDMC0100I(0097): Connecting.
    LTFSDMC0097I(0141): The OpenLTFS server process has been started with pid  13378.
    @endverbatim

    The responsible class is @ref StartCommand.

    @page start_processing start processing

    The following is a summary of the start command processing:

    @code
    StartCommand::doCommand
       StartCommand::determineServerPath
       StartCommand::startServer
       StartCommand::waitForResponse
           while not connected and retry<10
               LTFSDmCommClient::connect
               sleep 1
           if retry == 10
               exit with failue
           create statusrequest message
           LTFSDmCommClient::send
           LTFSDmCommClient::recv
           evaluate response
    @endcode

    To start the backend executable its path name needs to be detected. This
    is done by the StartCommand::determineServerPath method. Since its path
    is the same like the current executable it is just necessary to read
    the link of the current process to the client executable via procfs.

    The backend is started within the StartCommand::startServer method.
    It is start via popen system call.

    After the backend is started the status information is requested
    within the StartCommand::waitForResponse method. A connection is
    retried 10 times before giving up and reporting a failure.
 */

void StartCommand::printUsage()

{
    INFO(LTFSDMC0006I);
}

void StartCommand::determineServerPath()

{
    char exepath[PATH_MAX];

    TRACE(Trace::normal, Const::SERVER_COMMAND);

    memset(exepath, 0, PATH_MAX);
    if (readlink("/proc/self/exe", exepath, PATH_MAX - 1) == -1) {
        MSG(LTFSDMC0021E);
        THROW(Error::GENERAL_ERROR);
    }

    serverPath << dirname(exepath) << "/" << Const::SERVER_COMMAND;

    TRACE(Trace::normal, serverPath.str());
}

void StartCommand::startServer()

{
    struct stat statbuf;
    FILE *ltfsdmd = NULL;
    char line[Const::OUTPUT_LINE_SIZE];
    int ret;

    if (stat(serverPath.str().c_str(), &statbuf) == -1) {
        MSG(LTFSDMC0021E);
        TRACE(Trace::error, serverPath.str(), errno);
        THROW(Error::GENERAL_ERROR);
    }

    MSG(LTFSDMC0099I);

    ltfsdmd = popen(serverPath.str().c_str(), "r");

    if (!ltfsdmd) {
        MSG(LTFSDMC0022E);
        TRACE(Trace::error, errno);
        THROW(Error::GENERAL_ERROR);
    }

    while (fgets(line, sizeof(line), ltfsdmd)) {
        INFO(LTFSDMC0024I, line);
    }

    ret = pclose(ltfsdmd);

    if (!WIFEXITED(ret) || WEXITSTATUS(ret)) {
        MSG(LTFSDMC0022E);
        TRACE(Trace::error, ret, WIFEXITED(ret), WEXITSTATUS(ret));
        THROW(Error::GENERAL_ERROR);
    }
}

void StartCommand::waitForResponse()

{
    int pid;
    int retry = 0;
    bool success = false;

    MSG(LTFSDMC0100I);
    while (retry < 10) {
        try {
            connect();
            success = true;
            break;
        } catch (const std::exception& e) {
            INFO(LTFSDMC0103I);
            retry++;
            sleep(1);
        }
    }

    INFO(LTFSDMC0104I);

    if (success == false) {
        MSG(LTFSDMC0096E);
        return;
    }

    LTFSDmProtocol::LTFSDmStatusRequest *statusreq =
            commCommand.mutable_statusrequest();
    statusreq->set_key(key);
    statusreq->set_reqnumber(requestNumber);

    try {
        commCommand.send();
    } catch (const std::exception& e) {
        MSG(LTFSDMC0027E);
        THROW(Error::GENERAL_ERROR);
    }

    try {
        commCommand.recv();
    } catch (const std::exception& e) {
        MSG(LTFSDMC0098E);
        THROW(Error::GENERAL_ERROR);
    }

    const LTFSDmProtocol::LTFSDmStatusResp statusresp =
            commCommand.statusresp();

    if (statusresp.success() == true) {
        pid = statusresp.pid();
        MSG(LTFSDMC0097I, pid);
    } else {
        MSG(LTFSDMC0098E);
        THROW(Error::GENERAL_ERROR);
    }

}

void StartCommand::doCommand(int argc, char **argv)

{
    if (argc > 1) {
        printUsage();
        THROW(Error::GENERAL_ERROR);
    }

    determineServerPath();
    startServer();
    waitForResponse();

}
