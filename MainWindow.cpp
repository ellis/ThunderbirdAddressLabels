#include "MainWindow.h"

#include <iostream>

#include <QtDebug>
#include <QAction>
#include <qcombobox.h>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

#include "MabReader.h"
#include "Page.h"

using namespace std;


MainWindow::MainWindow()
{
	setupWidgets();
	setupActions();
}
	
MainWindow::~MainWindow()
{
}

void MainWindow::addFilename(const QString& sFilename)
{
	m_pSource->addItem(sFilename);
	m_mapNameToFile[sFilename] = sFilename;
}

void MainWindow::setupWidgets()
{
	QWidget* pBoxParent = new QWidget;
	QVBoxLayout* pBox = new QVBoxLayout;
	pBoxParent->setLayout(pBox);
	
	QWidget* preview = new QWidget;
	QGridLayout* previewLayout = new QGridLayout;
	preview->setLayout(previewLayout);
	
	m_pPageLabel = new QLabel(tr("Dude"));
	m_pPageSlider = new QSlider(Qt::Horizontal);
	m_pPageSlider->setEnabled(false);
	connect(m_pPageSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPage(int)));
	m_pPage = new Page();
	QScrollArea* pageScroller = new QScrollArea();
	pageScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pageScroller->setWidgetResizable(true);
	pageScroller->setBackgroundRole(QPalette::Dark);
	pageScroller->setWidget(m_pPage);
	previewLayout->addWidget(m_pPageLabel, 0, 0);
	previewLayout->addWidget(m_pPageSlider, 0, 1);
	previewLayout->addWidget(pageScroller, 1, 0, 1, 2);
	
	QSplitter* pSplitter = new QSplitter(this);
	pSplitter->addWidget(pBoxParent);
	pSplitter->addWidget(preview);
	pSplitter->setSizes(QList<int>() << 150 << 250);
	
	//pSplitter->setOrientation(QSplitter::Vertical);
	pBox->addWidget(new QLabel(tr("Address Book:")));
	m_pSource = new QComboBox(false);
	pBox->addWidget(m_pSource);
	pBox->addWidget(new QLabel("People:"));
	m_pListPeople = new QListWidget();
	connect(m_pListPeople, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(on_list_itemClicked(QListWidgetItem*)));
	pBox->addWidget(m_pListPeople);
	
	QPushButton* btn;
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	pBox->addLayout(buttonLayout);
	btn = new QPushButton("Select All");
	buttonLayout->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(on_btnSelectAll_clicked()));
	btn = new QPushButton("Clear Selection");
	connect(btn, SIGNAL(clicked()), this, SLOT(on_btnSelectNone_clicked()));
	buttonLayout->addWidget(btn);
	//new QComboBox("Adressenquelle:", pSplitter);

	setCentralWidget(pSplitter);
	
	setupSourceList();
	
	setPage(0);
}

void MainWindow::setupActions()
{
	QMenu* menu;
	QAction* act;
	
	menu = menuBar()->addMenu(tr("&File"));
	
	act = new QAction(tr("&Print"), this);
	act->setShortcut(tr("Ctrl+P"));
	connect(act, SIGNAL(triggered()), m_pPage, SLOT(print()));
	menu->addAction(act);
	
	act = new QAction(tr("E&xit"), this);
	act->setShortcut(tr("Ctrl+Q"));
	act->setStatusTip(tr("Exit the application"));
	connect(act, SIGNAL(triggered()), this, SLOT(close()));
	menu->addAction(act);
}

void MainWindow::setupSourceList()
{
	QDir dirThunderbird(QDir::homePath() + "/.thunderbird/");
	if (!dirThunderbird.exists())
		dirThunderbird = QDir(QDir::homePath() + "/.mozilla-thunderbird/");
	if (dirThunderbird.exists("Profiles"))
		dirThunderbird.cd("Profiles");
	
	if (dirThunderbird.exists()) {
		qDebug() << "dirThunderbird: " << dirThunderbird.absolutePath();
		QStringList list = dirThunderbird.entryList(QStringList() << "*.default", QDir::Dirs);
		foreach (QString sFilename, list) {
			QString sSubDir = dirThunderbird.absolutePath() + "/" + sFilename;
			qDebug() << "sSubDir:" << sSubDir << endl;
			QDir dir(sSubDir);
			if (dir.exists()) {
				if (dir.exists("prefs.js")) {
					readPrefs(dir);
				}
				else {
					QStringList listMab = dir.entryList(QStringList() << "*.mab");
					foreach (QString sMab, listMab) {
						QString sFilename = dir.filePath(sMab);
						m_pSource->addItem(sFilename);
						m_mapNameToFile[sFilename] = sFilename;
					}
				}
			}
		}
	}
	
	connect(m_pSource, SIGNAL(activated(int)), this, SLOT(selectSource(int)));
	
	int i = 0;
	m_pSource->setCurrentIndex(i);
	selectSource(i);
	
	// FIXME: for debug only
	//m_pSource->setCurrentIndex(3);
	//selectSource(3);
	// ENDFIX
}

void MainWindow::readPrefs(const QDir& dir)
{
	QFile file(dir.filePath("prefs.js"));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		cerr << "Could not open prefs.js" << endl;
		return;
	}
	
	QStringList asAll = QString::fromUtf8(file.readAll()).split("\n").filter(".servers.");
	QStringList asFilenameLines = asAll.filter(".filename\", \"");
	QStringList asNameLines = asAll.filter(".description\", \"");
	
	foreach (QString sFilenameLine, asFilenameLines)
	{
		int i = sFilenameLine.indexOf(".filename");
		QString sPrefix = sFilenameLine.left(i);
		QStringList list = asNameLines.filter(sPrefix);
		if (list.size() != 1)
			continue;
		QString sNameLine = list[0];
		
		QRegExp rxValue(", \"(.*)\"");
		
		if (rxValue.indexIn(sNameLine) == -1)
			continue;
		QString sName = rxValue.cap(1);
		
		if (rxValue.indexIn(sFilenameLine) == -1)
			continue;
		QString sFilename = rxValue.cap(1);
		
		m_mapNameToFile[sName] = dir.filePath(sFilename);
	}
	
	foreach (QString sName, m_mapNameToFile.keys())
	{
		m_pSource->addItem(sName);
	}
}

void MainWindow::selectSource(int i)
{
	if (i > -1)
	{
		QString sName = m_pSource->currentText();
		QString sSource = m_mapNameToFile[sName];
		//qDebug() << "sName:" << sName << "sSource:" << sSource;
		
		MabReader reader(sSource);
		m_addresses = reader.addresses();
		m_pPage->setAddresses(&m_addresses);
		m_pPage->update();
		
		m_pListPeople->clear();
		for (int iAddress = 0; iAddress < m_addresses.size(); iAddress++)
		{
			const Address& addr = m_addresses[iAddress];
			
			QListWidgetItem* pItem = new QListWidgetItem(addr.sName);
			pItem->setData(Qt::UserRole, iAddress);
			
			if (addr.bValid)
			{
				pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				pItem->setCheckState(Qt::Checked);
			}
			else
			{
				pItem->setFlags(Qt::ItemIsUserCheckable);
				pItem->setCheckState(Qt::Unchecked);
			}
			
			m_pListPeople->addItem(pItem);
		}
		
		m_pPageSlider->setValue(0);
		m_pPageSlider->setMaximum(m_pPage->pageCount() - 1);
		m_pPageSlider->setEnabled(true);
		setPage(0);
	}
}

void MainWindow::setPage(int i)
{
	m_pPage->setPageIndex(i);
	
	QString s = tr("Page %0 of %1").arg(i + 1).arg(m_pPage->pageCount());
	m_pPageLabel->setText(s);
}

void MainWindow::on_list_itemClicked(QListWidgetItem* item)
{
	int iAddress = item->data(Qt::UserRole).toInt();
	Address& addr = m_addresses[iAddress];
	bool bVisible = (item->checkState() == Qt::Checked);
	
	if (addr.bValid != bVisible)
	{
		addr.bValid = bVisible;
		m_pPage->setAddresses(&m_addresses);
	}
}

void MainWindow::setAllSelected(bool b)
{
	for (int iRow = 0; iRow < m_addresses.size(); iRow++)
	{
		QListWidgetItem* item = m_pListPeople->item(iRow);
		bool bEnabled = item->flags().testFlag(Qt::ItemIsEnabled);
		
		if (bEnabled)
		{
			int iAddress = item->data(Qt::UserRole).toInt();
			Address& addr = m_addresses[iAddress];
			addr.bValid = b;
			item->setCheckState(b ? Qt::Checked : Qt::Unchecked);
		}
	}
	m_pPage->setAddresses(&m_addresses);
}

void MainWindow::on_btnSelectAll_clicked()
{
	setAllSelected(true);
}

void MainWindow::on_btnSelectNone_clicked()
{
	setAllSelected(false);
}
