#ifndef __ADDRESS_H
#define __ADDRESS_H

#include <QString>


struct Address {
	/// Name to print
	QString sName;
	/// First name, for sorting purposes
	QString sNameLast;
	/// Last name, for sorting purposes
	QString sNameFirst;
	QString sWhere;
	QString sAddress[2];
	QString sZipCode;
	QString sCity;
	QString sCountry;
	bool bValid;
	
	Address()
	{
		bValid = false;
	}
};

#endif
