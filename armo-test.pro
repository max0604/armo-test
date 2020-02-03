TEMPLATE = subdirs
SUBDIRS = client server
INCLUDEPATH  += /user/include
LIBS += -lssl -lcrypto -L/user/lib/ -lomniORB4 -lomnithread
