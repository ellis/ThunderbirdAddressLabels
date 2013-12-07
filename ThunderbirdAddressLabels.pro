TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on debug

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

HEADERS += MorkParser.h MabReader.h Address.h Page.h MainWindow.h
SOURCES += MorkParser.cpp MabReader.cpp Page.cpp MainWindow.cpp main.cpp
