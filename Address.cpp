#include "Address.h"

#include "MabReader.h"


#define GET(sKey, sVar) if (map.contains(sKey)) sVar = map[sKey];


Address::Address(MabObject* pObj)
{
	QMap<QString, QString>& map = pObj->mapFields;
	
	//cout << map.count() << map["FirstName"] + " " + map["LastName"] << endl;
	if (map.contains("FirstName") && map.contains("LastName"))
		sName = map["FirstName"] + " " + map["LastName"];
	else if (map.contains("DisplayName"))
		sName = map["DisplayName"];
	
	if (map.contains("WorkAddress"))
		sWhere = "Work";
	else if (map.contains("HomeAddress"))
		sWhere = "Home";
	
	if (!sWhere.isEmpty())
	{
		GET(sWhere + "Address", sAddress[0]);
		GET(sWhere + "Address2", sAddress[0]);
		GET(sWhere + "ZipCode", sZipCode);
		GET(sWhere + "City", sCity);
		GET(sWhere + "Country", sCountry);
	}
	
	bValid = (!sName.isEmpty() && !sAddress[0].isEmpty() && !sCity.isEmpty());
}
