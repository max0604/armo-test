#include "omniorbthread.h"
#include "../echo.h"
#include <iostream>
#include <string>

using namespace std;

///----------------------------------------------------------------------------
static void hello(Echo_ptr e)
{
    std::string in_str;
    cin >> in_str;
    CORBA::String_var src = (const char*)in_str.c_str();
    CORBA::String_var dest = e->echoString(src);
}

///----------------------------------------------------------------------------
OmniORBThread::OmniORBThread(int argc, char **argv, QObject *parent)
                        : QThread(parent), _argc(argc), _argv(argv)
{
}

OmniORBThread::~OmniORBThread()
{
    cout << "OmniORBThread is over!" << endl;
}

void OmniORBThread::run()
{
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(_argc, _argv);

        if( _argc != 2 ) {
            cerr << "usage:  eg2_clt <object reference>" << endl;
            exit(1);
        }

        CORBA::Object_var obj = orb->string_to_object(_argv[1]);

        Echo_var echoref = Echo::_narrow(obj);

        if( CORBA::is_nil(echoref) )
        {
            cerr << "Can't narrow reference to type Echo (or it was nil)." << endl;
            exit(1);
        }

        for (;;)
        {
            hello(echoref);
            usleep(400);
        }

        orb->destroy();
    }
    catch(CORBA::TRANSIENT&)
    {
        cerr << "Caught system exception TRANSIENT -- unable to contact the "
             << "server." << endl;
    }
    catch(CORBA::SystemException& ex)
    {
        cerr << "Caught a CORBA::" << ex._name() << endl;
    }
    catch(CORBA::Exception& ex)
    {
        cerr << "Caught CORBA::Exception: " << ex._name() << endl;
    }
    catch(omniORB::fatalException& fe)
    {
        cerr << "Caught omniORB::fatalException:" << endl;
        cerr << "  file: " << fe.file() << endl;
        cerr << "  line: " << fe.line() << endl;
        cerr << "  mesg: " << fe.errmsg() << endl;
    }

    // emit finished();
}
