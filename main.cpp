#include <QtDebug>
#include <QApplication>

#include "MainWindow.h"

using namespace std;


#include "MabReader.h"
#include "MorkParser.h"
void test()
{
	MabReader reader("/home/ellis/tmp/axel/impab.mab");
	reader.addresses();
}


int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	
	//test();
	//return 0;
	/*
	QStringList filenames;
	if (argc > 1) {
		for (int i = 1; i < argc; i++)
			filenames << argv[i];
	}
	else
		filenames = QStringList() << "01" << "02-new" << "03-changed" << "04-deleted";
	foreach (QString sFilename, filenames) {
		MorkParser parser;
		//parser.open("/home/ellis/tmp/axel/impab.mab");
		qDebug() << parser.open(sFilename);
		MorkTableMap* tables = parser.getTables(0x80);
		if (tables != NULL) {
			foreach (int idRowScope, tables->keys()) {
				cout << "\tidRowScope: " << idRowScope << endl;
				const RowScopeMap& rowScopes = tables->value(idRowScope);
				foreach (int idRow, rowScopes.keys()) {
					cout << "\t\tidRow: " << idRow << endl;
					const MorkRowMap& rows = rowScopes[idRow];
					foreach (int idCell, rows.keys()) {
						cout << "\t\t\tidCell: " << qPrintable(QString::number(idCell, 16)) << endl;
						const MorkCells& cells = rows[idCell];
						foreach (int idColumn, cells.keys()) {
							cout << "\t\t\t\t" << parser.getColumn(idColumn) << ": " << parser.getValue(cells[idColumn]) << endl;
						}
					}
				}
			}
		}
	}
	return 0;
	*/

	/*QString sFind;
	for (int i = 0; i < argc; i++) {
		QString s = argv[i];
		if (s == "find") {
			if (i + 1 < argc) {
				i++;
				sFind = argv[i];
			}
		}
	}

	if (!sFind.isEmpty()) {
		MabReader reader("/home/ellis/tmp/axel/impab.mab");
		QList<MabObject*> found = reader.findObjects(sFind);
		foreach (MabObject* obj, found) {
			reader.printObject(obj);
			cout << endl;
		}
		return 0;
	}*/
	
	MainWindow w;
	if (argc > 1) {
		for (int i = 1; i < argc; i++)
			w.addFilename(argv[i]);
	}
	w.showMaximized();
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	app.exec();
}
