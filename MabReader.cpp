#include "MabReader.h"

#include <iostream>

#include <QtDebug>
#include <QRegExp>

#include "MorkParser.h"

using namespace std;


MabReader::MabReader(const QString& sFile)
{
	load(sFile);
}

void MabReader::printObject(int idObject)
{
	if (!mapObjects.contains(idObject))
		return;
	MabObject* obj = mapObjects[idObject];
	printObject(obj);
}

void MabReader::printObject(const MabObject* obj)
{
	QList<QString> asKeys = obj->mapFields.keys();
	qSort(asKeys);
	cout << "Object #" << qPrintable(QString::number(obj->idObject, 16).toUpper()) << endl;
	foreach (QString sKey, asKeys) {
		cout << qPrintable(sKey) << ": " << qPrintable(obj->mapFields[sKey]) << endl;
	}
}

QList<MabObject*> MabReader::findObjects(const QString& s)
{
	QList<MabObject*> list;
	foreach (int idObject, mapObjects.keys()) {
		MabObject* obj = mapObjects[idObject];
		foreach (QString sValue, obj->mapFields.values()) {
			if (sValue.contains(s)) {
				list << obj;
				break;
			}
		}
	}
	return list;
}

bool MabReader::load(const QString& sFile)
{
	MorkParser parser;
	if (!parser.open(sFile))
		return false;

	MorkTableMap* tables = parser.getTables(0x80);
	for (int iTable = 1; iTable >= 0; iTable--) {
		const RowScopeMap& rowScopes = tables->value(iTable);
		const MorkRowMap& rows = rowScopes[0x80];
		// FIXME: for debug only
		//qDebug() << "5EA: cells: " << rows[0x5EA];
		foreach (int idCell, rows.keys()) {
			const MorkCells& cells = rows[idCell];
			// FIXME: for debug only
			//if (idCell == 0x5EA) {
			//	qDebug() << "5EA: cells: " << cells;
			//}
			MabObject* obj = new MabObject(idCell);
			foreach (int idColumn, cells.keys()) {
				QString sColumn = QString::fromUtf8(parser.getColumn(idColumn).c_str());
				QString sValue = QString::fromUtf8(parser.getValue(cells[idColumn]).c_str());
				// FIXME: for debug only
				//if (idCell == 0x5EA)
				//	cout << "\t\t\t\t" << parser.getColumn(idColumn) << ": " << parser.getValue(cells[idColumn]) << endl;
				obj->mapFields[sColumn] = sValue;
			}
			if (obj->mapFields.isEmpty())
				delete obj;
			else
				mapObjects[idCell] = obj;
		}
	}
	
	return true;
}

QList<Address> MabReader::addresses()
{
	QList<Address> list;
	
	QMap<QString, MabObject*> valid;
	QMap<QString, MabObject*> invalid;
	
	foreach (MabObject* pObj, mapObjects.values())
	{
		Address addr = objectToAddress(pObj);
		QString sSort = addr.sNameLast + "," + addr.sNameFirst;
		sSort = sSort.toLower();
		if (addr.bValid)
			valid[sSort] = pObj;
		else
			invalid[sSort] = pObj;
	}
	
	foreach (MabObject* pObj, invalid.values())
		list << objectToAddress(pObj);
	foreach (MabObject* pObj, valid.values())
		list << objectToAddress(pObj);
	
	return list;
}

#define GET(sKey, sVar) if (map.contains(sKey)) sVar = map[sKey];
Address MabReader::objectToAddress(const MabObject* pObj)
{
	Address a;
	
	const QMap<QString, QString>& map = pObj->mapFields;
	
	//cout << map.count() << map["FirstName"] + " " + map["LastName"] << endl;
	if (map.contains("FirstName") && map.contains("LastName"))
	{
		a.sNameFirst = map["FirstName"];
		a.sNameLast = map["LastName"];
		a.sName = a.sNameFirst + " " + a.sNameLast;
	}
	else if (map.contains("DisplayName"))
	{
		a.sNameFirst = map["DisplayName"];
		a.sNameLast = map["DisplayName"];
		a.sName = map["DisplayName"];
	}
	
	// FIXME: for debug only
	//if (a.sNameLast == "Reichert")
	//	qDebug() << map;
	
	if (map.contains("WorkAddress"))
		a.sWhere = "Work";
	else if (map.contains("HomeAddress"))
		a.sWhere = "Home";
	
	if (!a.sWhere.isEmpty())
	{
		GET(a.sWhere + "Address", a.sAddress[0]);
		GET(a.sWhere + "Address2", a.sAddress[1]);
		GET(a.sWhere + "ZipCode", a.sZipCode);
		GET(a.sWhere + "City", a.sCity);
		GET(a.sWhere + "Country", a.sCountry);
	}
	
	a.bValid = (!a.sName.isEmpty() && !a.sAddress[0].isEmpty() && !a.sCity.isEmpty());
	
	return a;
}
