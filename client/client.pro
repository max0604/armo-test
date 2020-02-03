CONFIG       += debug
HEADERS       = omniorbthread.h ../echo.h
SOURCES       = main.cpp omniorbthread.cpp ../echo.cpp
INCLUDEPATH  += /user/include
LIBS += -lssl -lcrypto -L/user/lib/ -lomniORB4 -lomnithread
