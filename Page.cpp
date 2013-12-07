#include "Page.h"

#include <iostream>

#include <QtDebug>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintEngine>
#include <QTextDocument>

#include "MabReader.h"
#include "Address.h"

using namespace std;

/*
7cm
34mm von top of page to first horizontal printed line
höhe der etikette 36mm
6.5mm
40mm von top of page to bottom of first row
8.5mm between bottom of second row and my first horizontal line
10.5mm

18mm

Print: 47 189 QRect(47,189 10106x12822) QRect(0,0 10200x13200) QRect(0,0 10200x13200)
PrintMM: 216 279
29.55
*/


Page::Page(QWidget* pParent)
	: QWidget(pParent)
{
	// FIXME: setPaletteBackgroundColor(Qt::white);
	//setPalette(QPalette(Qt::white, Qt::black));
	//setAutoFillBackground(true);
	setStyleSheet("background: white; color: black");
	
	m_addresses = NULL;
	
	m_nWidthPageMM = 210;
	m_nHeightPageMM = 297;
	m_nMarginLeftMM = 1;
	m_nMarginRightMM = 1;
	m_nMarginTopMM = 9;
	m_nMarginBottomMM = 2;
	
	m_nCols = 3;
	m_nRows = 8;
}

void Page::setAddresses(const QList<Address>* addresses) {
	m_addresses = addresses;
	
	m_aiPageMabObjects.clear();
	int n = 0;
	for (int i = 0; i < m_addresses->size(); i++) {
		const Address& addr = m_addresses->at(i);
		if (addr.bValid) {
			n++;
			if (n == 1)
				m_aiPageMabObjects << i;
			else if (n == m_nRows * m_nCols)
				n = 0;
		}
	}
	m_nPages = m_aiPageMabObjects.size();
	m_iPage = 0;
	
	update();
}

void Page::setPageIndex(int i)
{
	if (i >= 0 && i < m_nPages)
	{
		m_iPage = i;
		update();
	}
}

#include <QPrintEngine>

void Page::print() {
	if (m_addresses->size() == 0)
		return;
	
	QPrinter printer(QPrinter::HighResolution);
	
	printer.setFullPage(true);
	printer.setPageSize(QPrinter::A4);
//printer.setOrientation(QPrinter::Landscape);
	//printer.setOutputFileName("print.ps");
	//printer.setOutputFormat(QPrinter::PostScriptFormat);
	
	QPrintDialog dlg(&printer, this);
	dlg.setFromTo(1, m_nPages);
	dlg.setMinMax(1, m_nPages);
	
	QPrintEngine* engine = printer.printEngine();
	/*
	qDebug() << "engine->type() =" << engine;
	for (int i = 0; i < 30; i++)
	{
		engine->setProperty(QPrintEngine::PPK_PageSize, i);
	        qDebug() << "i:" << i << "PageSize:" << engine->property(QPrintEngine::PPK_PageSize) << endl
                         << "PaperRect:" << engine->property(QPrintEngine::PPK_PaperRect) << endl;
	}
	*/
	qDebug() << "PageSize:" << engine->property(QPrintEngine::PPK_PageSize) << endl
		<< "PageRect:" << engine->property(QPrintEngine::PPK_PageRect) << endl
		<< "PaperRect:" << engine->property(QPrintEngine::PPK_PaperRect) << endl
		<< "FullPage:" << engine->property(QPrintEngine::PPK_FullPage) << endl
		<< "Resolution:" << engine->property(QPrintEngine::PPK_Resolution) << endl;
	if (dlg.exec() == QDialog::Accepted) {
	engine = printer.printEngine();
	qDebug() << "PageSize:" << engine->property(QPrintEngine::PPK_PageSize) << endl
		<< "PageRect:" << engine->property(QPrintEngine::PPK_PageRect) << endl
		<< "PaperRect:" << engine->property(QPrintEngine::PPK_PaperRect) << endl
		<< "FullPage:" << engine->property(QPrintEngine::PPK_FullPage) << endl
		<< "Resolution:" << engine->property(QPrintEngine::PPK_Resolution) << endl;
		
		QPainter painter(&printer);
		QFontMetrics fm = painter.fontMetrics();
		QRect rcCanvas = painter.viewport();
		
	engine = printer.printEngine();
	qDebug() << "PageSize:" << engine->property(QPrintEngine::PPK_PageSize) << endl
		<< "PageRect:" << engine->property(QPrintEngine::PPK_PageRect) << endl
		<< "PaperRect:" << engine->property(QPrintEngine::PPK_PaperRect) << endl
		<< "FullPage:" << engine->property(QPrintEngine::PPK_FullPage) << endl
		<< "Resolution:" << engine->property(QPrintEngine::PPK_Resolution) << endl;
		
		// FIXME: for debug only
		//return;
		
		// Allocate margins
		//QPaintDeviceMetrics pdm(&printer);
		int xBorder = (int) (printer.width() * m_nMarginLeftMM / printer.widthMM());
		int yBorderTop = (int) (printer.height() * m_nMarginTopMM / printer.heightMM());
		int yBorderBot = (int) (printer.height() * m_nMarginBottomMM / printer.heightMM());
		rcCanvas.adjust(xBorder, yBorderTop, -xBorder, -yBorderBot);
		qDebug() << "Print:" << xBorder << yBorderTop << yBorderBot << rcCanvas << painter.viewport() << painter.window() << m_nPages;
		qDebug() << "PrintMM:" << printer.pageRect() << printer.widthMM() << printer.heightMM();
		qDebug() << "PrintEng:" << printer.printEngine()->property(QPrintEngine::PPK_PageSize);
		
		QFont font("Helvetica [Cronyx]", 12);
		painter.setFont(font);
		
		int iFrom = dlg.fromPage() - 1;
		int iTo = dlg.toPage();
		if (iTo == 0) {
			iFrom = 0;
			iTo = m_nPages;
		}
		
		for (int iPage = iFrom; iPage < iTo; iPage++) {
			paintPage(iPage, painter, rcCanvas, false);
			if (iPage < iTo - 1)
				printer.newPage();
		}

		painter.end();
	}
}

int Page::topOfRow(const QRect& rcCanvas, int iRow) const
{
	int nHeightRow = rcCanvas.height() / m_nRows;
	return rcCanvas.top() + iRow * nHeightRow;
}

int Page::leftOfCol(const QRect& rcCanvas, int iCol) const
{
	int nWidthCol = rcCanvas.width() / m_nCols;
	return rcCanvas.left() + iCol * nWidthCol;
}

QRect Page::rectOfEntry(const QRect& rcCanvas, int iRow, int iCol) const
{
	int xLeft = leftOfCol(rcCanvas, iCol);
	int xRight = leftOfCol(rcCanvas, iCol + 1);
	int yTop = topOfRow(rcCanvas, iRow);
	int yBot = topOfRow(rcCanvas, iRow + 1);
	
	QRect rc(QPoint(xLeft, yTop), QPoint(xRight, yBot));
	return rc;
}

void Page::paintPage(int iPage, QPainter& p, const QRect& rcCanvas, bool bShowGrid)
{
	int iObj = m_aiPageMabObjects[iPage];
	int iRow = 0;
	int iCol = 0;
	while (iObj < m_addresses->size() && iRow < m_nRows) {
		const Address& addr = m_addresses->at(iObj++);
		
		if (addr.bValid) {
			QRect rc = rectOfEntry(rcCanvas, iRow, iCol);
			paintAddress(addr, p, rc);
			
			iCol++;
			if (iCol >= m_nCols) {
				iRow++;
				iCol = 0;
			}
		}
	}

	if (bShowGrid)
	{
		// Draw the grid
		QPen penOrig = p.pen();
		QPen grid(QColor(0, 0, 0, 80));
		p.setPen(grid);
		int xLeft = leftOfCol(rcCanvas, 0);
		int xRight = leftOfCol(rcCanvas, m_nCols);
		for (int iRow = 0; iRow <= m_nRows; iRow++)
		{
			int y = topOfRow(rcCanvas, iRow);
			p.drawLine(xLeft, y, xRight, y);
		}
		int yTop = topOfRow(rcCanvas, 0);
		int yBot = topOfRow(rcCanvas, m_nRows);
		for (int iCol = 0; iCol <= m_nCols; iCol++)
		{
			int x = leftOfCol(rcCanvas, iCol);
			p.drawLine(x, yTop, x, yBot);
		}
		p.setPen(penOrig);
	}
}

void Page::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	
	double nWidthColMM = (m_nWidthPageMM - m_nMarginLeftMM - m_nMarginRightMM) / m_nCols;
	double nHeightRowMM = (m_nHeightPageMM - m_nMarginTopMM - m_nMarginBottomMM) / m_nRows;
	
	int nLeftBorder = int(double(width()) * m_nMarginLeftMM / m_nWidthPageMM);
	int nRightBorder = int(double(width()) * m_nMarginRightMM / m_nWidthPageMM);
	int nBorderTop = int(double(height()) * m_nMarginTopMM / m_nHeightPageMM);
	
	//qDebug() << "paintEvent:" << nLeftBorder << nRightBorder << nBorderTop;
	
	int nWidth = width() - nLeftBorder - nRightBorder;
	int nWidthCol = nWidth / m_nCols;
	
	int nHeightRow = int(nWidthCol * nHeightRowMM / nWidthColMM);
	
	QRect rcWhite = rect();
	rcWhite.setHeight(2 * nBorderTop + nHeightRow * m_nRows);
	painter.fillRect(rcWhite, Qt::white);
	
	QRect rcCanvas(nLeftBorder, nBorderTop, width() - nLeftBorder - nRightBorder, height() - nBorderTop * 2);
	
	if (m_addresses == NULL || m_addresses->size() == 0 || m_iPage >= m_nPages)
		return;
	
	paintPage(m_iPage, painter, rcCanvas, true);
	
	setMinimumHeight(nBorderTop * 2 + nHeightRow * m_nRows);
}

void Page::paintAddress(const Address& addr, QPainter& painter, QRect rc) {
	int nLines = 3;
	QString s =
		addr.sName + "\n" +
		addr.sAddress[0] + "\n";
	if (!addr.sAddress[1].isEmpty()) {
		s += addr.sAddress[1] + "\n";
		nLines++;
	}
	// City
	QString sCity = addr.sCity;
	if (!addr.sZipCode.isEmpty())
		sCity = addr.sZipCode + " " + sCity;
	s += sCity;
	// Country
	if (!addr.sCountry.isEmpty() && addr.sCountry != "Deutschland") {
		s += "\n" + addr.sCountry;
		nLines++;
	}
	
	// 5% border on all sides
	int xBorder = rc.width() / 20;
	int yBorder = rc.height() / 20;
	rc.adjust(xBorder, yBorder, -xBorder, -yBorder);

	int nFontSize = 12;
	bool bWidthOk = false;
	bool bHeightOk = false;
	QRect rcPrint = rc;
	do {
		QFont font("Helvetica [Cronyx]", nFontSize);
		QFontMetrics fm(font);
		painter.setFont(font);
		QRect rcText = painter.boundingRect(rc.x(), rc.y(), rc.width() * 2, rc.height() * 2, 0, s);
		//qDebug() << "ENTRY:" << rcText.width() << " " << rc.width() << " " << rcText.height() << " " << rc.height() << " " << addr.sName << endl;
		int nExtraWidth = 0;
		int nExtraHeight = 0;
		if (rcText.width() <= rc.width()) {
			bWidthOk = true;
			nExtraWidth = rc.width() - rcText.width();
		}
		if (rcText.height() <= rc.height()) {
			bHeightOk = true;
			nExtraHeight = rc.height() - rcText.height();
		}
		rcPrint = rc;
		rcPrint.adjust(nExtraWidth / 3, nExtraHeight / 2, 0, 0);
		
		nFontSize--;
	} while (nFontSize > 8 && (bWidthOk == false || bHeightOk == false));
	
	painter.drawText(rcPrint, 0, s);
	/*
	if (bPrint) {
		//qDebug() << "Print " << addr.sName << " with " << nFontSize << endl;
		painter.drawText(rcPrint, 0, s);
	}
	else {
		s.replace("\n", "<br/>");
		QTextDocument text;
		text.setHtml(s);
		
		// Find a font size that will hopefully fit all the text into
		// the allocated area.
		nFontSize = 12;
		bool bFontOk = false;
		do {
		
			QFont font("Helvetica [Cronyx]", nFontSize);
			text.setDefaultFont(font);
			//text.setTextWidth(&painter, rc.width());
			if (text.pageSize().width() <= rc.size().width() && text.pageSize().height() <= rc.size().height())
				bFontOk = true;
			nFontSize--;
		} while (nFontSize >= 8 && !bFontOk);
	
		int nExtraWidth = int(rc.width() - text.pageSize().width());
		int nExtraHeight = int(rc.height() - text.pageSize().height());
		if (nExtraWidth < 0)
			nExtraWidth = 0;
		QPoint pt = rc.topLeft() + QPoint(nExtraWidth / 3, nExtraHeight / 2);
		//qDebug() << "paintAddress:" << rc << pt;
		
		// Draw the address
		painter.translate(pt.x(), pt.y());
		QRect rcText = rc;
		rcText.moveTo(0, 0);
		text.drawContents(&painter, rcText);
		painter.translate(-pt.x(), -pt.y());
	}
	*/
}
