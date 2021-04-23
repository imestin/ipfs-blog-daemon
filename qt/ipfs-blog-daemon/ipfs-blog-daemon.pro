HEADERS  += mainclass.h
SOURCES += main.cpp\
        mainclass.cpp

RESOURCES     += daemon.qrc

QT       += widgets
INCLUDEPATH += tmp/moc/release_shared

# install
#target.path = /home/user/Blog/ipfs-blog-daemon/qt/build-ipfs-blog-daemon-Desktop-Debug/ipfs-blog-daemon
#INSTALLS += target

/*----------------------------------/
#HEADERS       = window.h
#SOURCES       = main.cpp \
                window.cpp
#RESOURCES     = systray.qrc

#QT += widgets
#INCLUDEPATH += tmp/moc/release_shared

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
#INSTALLS += target
