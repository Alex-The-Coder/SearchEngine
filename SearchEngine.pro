TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    DocumentLoader.cpp \
    InputProcessor.cpp \
    Porter2_Stemmer.cpp \
    WordDocumentData.cpp

HEADERS += \
    AVLNode.h \
    AVLTree.h \
    DocumentLoader.h \
    HashTable.h \
    HashTableNode.h \
    InputProcessor.h \
    json.hpp \
    Porter2_Stemmer.h \
    WordDocumentData.h \
    IndexInterface.h
