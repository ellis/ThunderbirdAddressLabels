#ifndef _PAGE_H
#define _PAGE_H

#include <QList>
#include <QWidget>

#include "Address.h"


class Page : public QWidget
{
	Q_OBJECT
public:
	Page(QWidget* pParent = NULL);
	
	void setAddresses(const QList<Address>* addresses);
	
	int pageCount() const { return m_nPages; }
	
	void setPageIndex(int i);

public slots:
	void print();

protected:
	virtual void paintEvent(QPaintEvent* pEvent);

private:
	int topOfRow(const QRect& rcCanvas, int iRow) const;
	int leftOfCol(const QRect& rcCanvas, int iCol) const;
	QRect rectOfEntry(const QRect& rcCanvas, int iRow, int iCol) const;
	
	void paintPage(int iPage, QPainter& p, const QRect& rcCanvas, bool bShowGrid);
	void paintAddress(const Address&, QPainter&, QRect rc);
	
private:
	const QList<Address>* m_addresses;
	double m_nWidthPageMM;
	double m_nHeightPageMM;
	double m_nMarginLeftMM;
	double m_nMarginRightMM;
	double m_nMarginTopMM;
	double m_nMarginBottomMM;
	int m_nCols;
	int m_nRows;
	int m_nPages;
	int m_iPage;
	/// List of indexes to first objects on a page.
	QList<int> m_aiPageMabObjects;
};

#endif // !_PAGE_H
