/* 
 * Evorobot* - rend_robot3d.h
 * Copyright (C) 2009, Onofrio Gigliotta 
 * LARAL, Institute of Cognitive Science and Technologies, CNR, Roma, Italy 
 * http://laral.istc.cnr.it

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <QKeyEvent>
#include "qglviewer.h"
#include <QImage>
#include "mesh.h"




class Viewer : public QGLViewer
{
public:
	QImage* bodyText;
	QImage* floorText;
	QImage* headText;
	Viewer( QWidget* parent = 0 ) : QGLViewer(parent) { };
	void update3d();
	
	
protected :
 
  virtual void draw();
  virtual void postSelection(const QPoint& point);
  virtual void drawWithNames();
  virtual void init();
  virtual void keyPressEvent(QKeyEvent *e);
  void drawMark(float x, float y, int rbtype, int rbtid);
  
  

  virtual QString helpString() const;
  void drawWall(float x, float y, float w, float h, float thin, float thinup);
  void drawMesh(wMesh* wmesh, float x, float y,float z, float angle, int id_name, int sl=0);

  private :
  qglviewer::Vec orig, dir, selectedPoint;
  
};

