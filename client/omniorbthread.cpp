#include "omniorbthread.h"
#include "../echo.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <termios.h>

using namespace std;

///----------------------------------------------------------------------------
static char getch()
{
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

///----------------------------------------------------------------------------
static void hello(Echo_ptr e)
{
#if 0
    std::string in_str;
    cin >> in_str;
    CORBA::String_var src = (const char*)in_str.c_str();
    CORBA::String_var dest = e->echoString(src);
#else
    char c;
    char buf[2]; memset(buf, 0, 2);
    c = getch();
    memcpy(buf, &c, 1);
    CORBA::String_var src = (const char*)(buf);
    CORBA::String_var dest = e->echoString(src);
#endif
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
