#ifndef _MABREADER_H
#define _MABREADER_H

#include <map>
#include <fstream>
#include <iostream>
#include <vector>

#include <QFile>
#include <QMap>
#include <QList>
//#include <QTextStream>

#include "Address.h"


/// Thunderbird Address Book Object
struct MabObject
{
	int idObject;
	QMap<QString, QString> mapFields;

	MabObject(int idObject) : idObject(idObject) {}
};


/// Thunderbird Address Book Reader
class MabReader
{
public:
	MabReader(const QString& sFile);
	
	~MabReader()
	{
	}

	//QList<MabObject*>& list() { return lMabObjects; }
	QList<Address> addresses();
	
	void printObject(int idObject);
	void printObject(const MabObject* obj);

	/// Find the objects which contain values matching @ref s
	QList<MabObject*> findObjects(const QString& s);

private:
	bool load(const QString& sFile);
	void getLine();
	/// Fill map of value IDs to value strings
	void getMappings(QMap<int, QString>& map);
	void getMabObject();
	
	Address objectToAddress(const MabObject* pObj);
	
	/// Decode raw MAB string into a UTF8 representation
	QString decode(const QString& sValueRaw);
	void parseDataAndObjects();
	void parseObject(int idObject, const QString& sObject);

private:
	QMap<int, MabObject*> mapObjects;
	QFile file;
	QMap<int, QString> mapLabels;
	QMap<int, QString> mapData;
	QString sLine;
	int iChar;
};


#endif // !_ABOOK_H
