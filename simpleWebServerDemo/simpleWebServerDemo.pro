TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread \
        -lrt

SOURCES += \
        ipc.cpp \
        main.cpp \
        some_busy_job.cpp \
        task_transfer.cpp \
        thread_pool.cpp

HEADERS += \
    ipc.h \
    some_busy_job.h \
    task_transfer.h \
    thread_pool.h
