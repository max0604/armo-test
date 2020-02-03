#include "omniorbthread.h"
#include "../echo.h"

#include <iostream>
using namespace std;

// This is the object implementation.

///----------------------------------------------------------------------------
class Echo_i : public POA_Echo
{
public:
    inline Echo_i() {}
    virtual ~Echo_i() {}
    virtual char* echoString(const char* mesg);
};

///----------------------------------------------------------------------------
char* Echo_i::echoString(const char* mesg)
{
    ///cout << "Upcall " << mesg << endl;
    cout << mesg << endl;
    return CORBA::string_dup(mesg);
}

///----------------------------------------------------------------------------
OmniORBThread::OmniORBThread(int argc, char **argv, QObject *parent)
    : QThread(parent), _argc(argc), _argv(argv)
{
}

///----------------------------------------------------------------------------
OmniORBThread::~OmniORBThread()
{
    cout << "OmniORBThread is over!" << endl;
}

///----------------------------------------------------------------------------
void OmniORBThread::run()
{
    try
    {
        CORBA::ORB_var orb = CORBA::ORB_init(_argc, _argv);

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

        Echo_i* myecho = new Echo_i();

        PortableServer::ObjectId_var myechoid = poa->activate_object(myecho);

        // Obtain a reference to the object, and print it out as a
        // stringified IOR.
        obj = myecho->_this();
        CORBA::String_var sior(orb->object_to_string(obj));
        cout << (char*)sior << endl;

        myecho->_remove_ref();

        PortableServer::POAManager_var pman = poa->the_POAManager();
        pman->activate();

        orb->run();
    }
    catch(CORBA::SystemException& ex)
    {
        cerr << "Caught CORBA::" << ex._name() << endl;
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
    //emit finished();
}
