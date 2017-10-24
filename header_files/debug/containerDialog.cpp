/***************************************************************************
 *   Copyright (C) 2007 by Tomassino Ferrauto                              *
 *   t_ferrauto@yahoo.it                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "mode.h"

#ifdef EVOGRAPHICS

#include "containerDialog.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>

ContainerDialog::ContainerDialog(QWidget *parent) :
	QDialog(parent),
	m_mainLayout(NULL),
	m_layout(NULL),
	m_toolBar(NULL)
{
	// Creating the main layout. Direction is BottomToTop so that m_layout (added first)
	// is at bottom, while the toolbar (added last) is on top
	m_mainLayout = new QBoxLayout(QBoxLayout::BottomToTop, this);
	//m_mainLayout->setContentsMargins(0, 0, 0, 0);

	// Creating the layout for external widgets
	m_layout = new QGridLayout();
	//m_layout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->addLayout(m_layout);
}

ContainerDialog::~ContainerDialog()
{
}

void ContainerDialog::setWidget(QWidget *w, int r, int c)
{
	m_layout->addWidget(w, r, c);
}

QToolBar* ContainerDialog::toolBar()
{
	if (m_toolBar == NULL) {
		// Creating toolbar and adding it to the main layout
		m_toolBar = new QToolBar(this);
		m_mainLayout->addWidget(m_toolBar);
	}

	return m_toolBar;
}

void ContainerDialog::closeEvent(QCloseEvent *)
{
	emit dialogClosed();
}

void ContainerDialog::keyReleaseEvent(QKeyEvent* event)
{
    /*
	if (event->matches(QKeySequence::Print)) {
		// Taking a screenshow of the widget
		shotWidget();
	} else {
		// Calling parent function
		QDialog::keyReleaseEvent(event);
	}*/
}

void ContainerDialog::shotWidget()
{
    /*
	// Taking a screenshot of this widget
	QPixmap shot(size());
	render(&shot);

	// Asking the user where to save the shot
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Shot"), "./widget.png", tr("Images (*.png *.xpm *.jpg)"));
	if (!fileName.isEmpty()) {
		shot.save(fileName);

		QMessageBox::information(this, QString("File Saved"), QString("The widget shot has been saved"));
	}*/
}

#endif
