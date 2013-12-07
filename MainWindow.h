#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <vector>

#include <QList>
#include <QMainWindow>
#include <QMap>

#include "Address.h"


class QComboBox;
class QDir;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QSlider;
class QTabWidget;

class MabObject;
class MabReader;
class Page;


class MainWindow : public QMainWindow {
	Q_OBJECT
	
public:
	MainWindow();
	virtual ~MainWindow();
	
	void addFilename(const QString& sFilename);

private:
	void setupWidgets();
	void setupActions();
	void setupSourceList();
	void readPrefs(const QDir& dir);
	void setAllSelected(bool b);

private slots:
	void selectSource(int i);
	void setPage(int i);
	void on_list_itemClicked(QListWidgetItem* item);
	void on_btnSelectAll_clicked();
	void on_btnSelectNone_clicked();

private:
	//QList<MabObject*>* m_pList;
	QComboBox* m_pSource;
	QListWidget* m_pListPeople;
	QMap<QString, QString> m_mapNameToFile;
	//MabReader* m_pAddresses;
	Page* m_pPage;
	QLabel* m_pPageLabel;
	QSlider* m_pPageSlider;

	QList<Address> m_addresses;
};

#endif // !_MAINWINDOW_H
