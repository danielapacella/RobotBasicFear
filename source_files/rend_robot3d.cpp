/* 
 * Evorobot* - rend_robot3d.cpp
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
//Thanks to Bred Porter for the drawing mesh method

//3d Robot models designed by Onofrio Gigliotta (c)


#include "public.h"

#ifdef EVOGRAPHICS
#ifdef EVO3DGRAPHICS
#include "rend_robot3d.h"
//#include "simpleViewer.h"
#include "epuck.h"
#include "environment.h"

#include <gl/glu.h>
#include "vec.h"
#include <qmessagebox.h>


Viewer *view;
wMesh *edubot_mesh; //edubot 3d model built upon the real lego edubot
wMesh *epuck_mesh;  //epuck 3d mesh
wMesh *khepera_mesh;//khepera mesh

extern int selmode;
extern int cindividual;

// Draws a parallepiped
void Viewer::drawWall(float x, float y, float w, float h, float thin, float thinup)
{

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  this->bindTexture(*floorText);
    
  glBegin(GL_QUADS);
  //front    
  glNormal3f(0.0, 0.0, 1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x,  y,  thin);
  glTexCoord2f(1.0, 0.0); glVertex3f(x,  y+h,thin);
  glTexCoord2f(1.0, 1.0); glVertex3f(x+w,y+h,thin);
  glTexCoord2f(0.0, 1.0); glVertex3f(x+w,y,  thin);
  // top   
  glNormal3f(0.0, 1.0, 0.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x+w, y+h,  thin);
  glTexCoord2f(1.0, 0.0); glVertex3f(x+w, y+h,  thinup);
  glTexCoord2f(1.0, 1.0); glVertex3f(x,   y+h,  thinup);
  glTexCoord2f(0.0, 1.0); glVertex3f(x,   y+h,  thin);
  // back
  glNormal3f(0.0, 0.0, -1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x,  y,  thin);
  glTexCoord2f(1.0, 0.0); glVertex3f(x,  y+h,thin);
  glTexCoord2f(1.0, 1.0); glVertex3f(x+w,y+h,thin);
  glTexCoord2f(0.0, 1.0); glVertex3f(x+w,y,  thin);
  // bottom
  glNormal3f(0.0, -1.0, 0.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x,  y, thinup);
  glTexCoord2f(1.0, 0.0); glVertex3f(x,  y, thin);
  glTexCoord2f(1.0, 1.0); glVertex3f(x+w,y, thin);
  glTexCoord2f(0.0, 1.0); glVertex3f(x+w,y, thinup);
  // left
  glNormal3f(-1.0, 0.0, 0.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x, y,  thinup);
  glTexCoord2f(1.0, 0.0); glVertex3f(x, y+h,thinup);
  glTexCoord2f(1.0, 1.0); glVertex3f(x, y+h,thin);
  glTexCoord2f(0.0, 1.0); glVertex3f(x, y,  thin);  
  // right
  glNormal3f(1.0, 0.0, 0.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(x+w, y,   thinup);
  glTexCoord2f(1.0, 0.0); glVertex3f(x+w, y+h, thinup);
  glTexCoord2f(1.0, 1.0); glVertex3f(x+w, y+h, thin);
  glTexCoord2f(0.0, 1.0); glVertex3f(x+w, y,   thin);

  glEnd();
  glDisable(GL_TEXTURE_2D);

}

void Viewer::draw()
{
  const float nbSteps = 200.0;
  float  dx,dy,dz;
  int    dd;
  int    ww, hh;
  struct individual *pind;
  int    team;
  struct envobject *obj;
  float  scolor;
  

  //to adapt coordinate system
  // 3dX=x; 3dY=-y;  3dDirection=-dir
  //I added a rotation to synchronize textures 

  dx=dz=dy=2;
  glClearColor(1.0,1.0,1.0,0.0),
  glClear(GL_COLOR_BUFFER_BIT);
  GLUquadricObj *qobj;
  qobj=gluNewQuadric();
  gluQuadricNormals(qobj, GLU_SMOOTH);
  ///*
  //drawing the floor
  glColor3f(1.0,1.0,1.0);
  drawWall(0,0,envdx,-envdy,-30,-40); 

  //drawing grounds area
  obj = *(envobjs + GROUND);
    for (dd= (envnobjs[GROUND] - 1), obj =(obj + (envnobjs[GROUND] - 1)); dd>= 0; dd--, obj--)
       {
        scolor = (1.0 - obj->c[0]) * 155.0 + 100;
    if (scolor > 255)
      scolor = 255;
    if (scolor < 0)
      scolor = 0;
       // painter.setPen(QColor(0,scolor,scolor,255));
        //painter.setBrush(QBrush(QColor(0,scolor,scolor,255), Qt::SolidPattern));
        if (obj->subtype == 0)
        {
            //in cas of ellipsoid a circle area
        }
          //painter.drawEllipse((obj->x - obj->r) * scale, (obj->y - obj->r) * scale, obj->r * 2.0  * scale, obj->r * 2.0  * scale);
    else
    {
             
                ww=obj->X;
                 hh=-obj->Y;
                      scolor = (1.0 - obj->c[0]) * 155.0 + 100;
                    if (scolor > 255)
                      scolor = 255;
                    if (scolor < 0)
                      scolor = 0;
                //glColor3f(obj->c[0],obj->c[1],obj->c[2]);
                glColor3f(0.0,(float)scolor/255.0,(float)scolor/255.0);
                
                drawWall(obj->x, -obj->y, ww, hh, -28, -29);
                //drawWall(1000.0, -3000.0, -1000, 2000, 140, -30);
        dd=dd;  
        }
    //painter.drawRect(obj->x * scale, obj->y * scale, obj->X * scale, obj->Y * scale); 
       }
    //end ground area


  //drawing wall
  for(dd=0, obj = *(envobjs + 0);dd < envnobjs[0];dd++, obj++)
  {
    ww=(obj->X - obj->x);
    hh=-(obj->Y - obj->y);
    glColor3f(obj->c[0],obj->c[1],obj->c[2]);
    if (ww==0) ww = 10;
    if (hh==0) hh = 10;
      drawWall(obj->x, -obj->y, ww, hh, obj->h, -30);
  }
  //drawing small round obstacles
  for(dd=0, obj = *(envobjs + 1);dd < envnobjs[1]; dd++, obj++)
  {
      glColor3f(obj->c[0],obj->c[1],obj->c[2]);
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        
      glTranslatef(obj->x, - obj->y, -30); 
      gluQuadricTexture(qobj, true); 
      this->bindTexture(*floorText);
      gluCylinder(qobj,obj->r,obj->r,obj->h,40,40); 
      glTranslatef(0,0,60);
      glRotatef(180,0.0,0.0,1.0);
      gluDisk(qobj,0,obj->r,40,4); 
      glDisable(GL_TEXTURE_2D);
      glPopMatrix();
  }
  //drawing target areas 
  for(dd=0, obj = *(envobjs + 3); dd < envnobjs[3]; dd++, obj++)
  {
     scolor = (1.0 - obj->c[0]) * 0.6 + 0.4;
     glColor3f(0.0,scolor,scolor);
     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        
     glTranslatef(obj->x,- obj->y, -28);        
     gluQuadricTexture(qobj, true); 
     this->bindTexture(*floorText);
     gluDisk(qobj,0,obj->r,40,4);        
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();
  }
 //drawing lights
 glColor3f(2.5,2.5,0.3);
 for(dd=0, obj = *(envobjs + 4);dd < envnobjs[4];dd++, obj++)
  {
     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        
     glTranslatef(obj->x,- obj->y,28);        
     gluQuadricTexture(qobj, true); 
     this->bindTexture(*floorText);        
     gluSphere(qobj,15,40,40);
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();
  }
  //drawing robot


  glColor3f(1.5,1.5,1.5);
    for(team=0,pind=ind;team<nteam;team++,pind++)
   {
      //glPushMatrix();
      //glEnable(GL_TEXTURE_2D);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // drawMark(pind->pos[0],-pind->pos[1],robottype, team);
       drawMark(pind->pos[0],-pind->pos[1],robottype,team);
       /*glPushMatrix();
       glTranslatef(pind->pos[0],-pind->pos[1],60);
       gluDisk(qobj,0,100,40,4);
       glPopMatrix();*/

      switch(robottype)
      {
      case 0:
          //drawing khepera robot
          if (selmode==1 && team==cindividual)
          drawMesh(khepera_mesh,pind->pos[0],-pind->pos[1],-18,-pind->direction+180, team,1);
          else
          drawMesh(khepera_mesh,pind->pos[0],-pind->pos[1],-18,-pind->direction+180, team);
          break;
      case 1:

      //drawing epuck robots
      if (selmode==1 && team==cindividual)
      drawMesh(epuck_mesh,pind->pos[0],-pind->pos[1],-26,-pind->direction+180, team,1);
      else
      drawMesh(epuck_mesh,pind->pos[0],-pind->pos[1],-26,-pind->direction+180, team);
      
      /*glTranslatef(pind->pos[0],-pind->pos[1],-30);
      glRotatef(-pind->direction+90,0.0,0.0,1.0);
      gluQuadricTexture(qobj, true); 
      this->bindTexture(*bodyText);
      gluCylinder(qobj,35,35,48,40,40);      
      glTranslatef(0,0,47);
      this->bindTexture(*headText);
      glRotatef(180,0.0,0.0,1.0);
      gluDisk(qobj,0,35,40,4);*/
      break;
      case 2:
            if (selmode==1 && team==cindividual)
          drawMesh(edubot_mesh,pind->pos[0],-pind->pos[1],-30,-pind->direction+180, team,1);
            else
          drawMesh(edubot_mesh,pind->pos[0],-pind->pos[1],-30,-pind->direction+180, team);
      /*
      glTranslatef(pind->pos[0],-pind->pos[1],-30);
      glRotatef(-pind->direction+90,0.0,0.0,1.0);
      gluQuadricTexture(qobj, true); 
      this->bindTexture(*bodyText);
      gluCylinder(qobj,100,100,200,40,40);      
      glTranslatef(0,0,200);
      this->bindTexture(*headText);
      glRotatef(180,0.0,0.0,1.0);
      gluDisk(qobj,0,100,40,4);
      */

      //drawMesh(edubot_mesh,pind->pos[0],-pind->pos[1],-400,-pind->direction);
     
      
     break;

      }

    
      //glDisable(GL_TEXTURE_2D);
      //glPopMatrix();
    
     // drawMesh(edubot_mesh,pind->pos[0]+200,-pind->pos[1],-30,-pind->direction+180);
     // drawMesh(khepera_mesh,pind->pos[0]+400,-pind->pos[1],-18,-pind->direction+180);
     //drawMesh(epuck_mesh,pind->pos[0],-pind->pos[1],-26,-pind->direction+180);

    } 
    //*/

     // drawMesh(edubot_mesh,0,0,0,0);
}

void Viewer::drawMesh(wMesh *wmesh, float x, float y,float z, float angle, int id_name, int sl)
{
        
        glPushMatrix();
        glPushName(id_name);
        glScalef(10, 10, 10);
        glTranslatef(x/10.0,y/10.0,z/10.0);
        glRotatef(angle,0.0,0.0,1.0);
        //glEnable(GL_BLEND); //trasparenza
        float kl=3.0;
        int m_numMeshes = wmesh->meshesCount();
        wMesh::Mesh *m_pMeshes = wmesh->meshes();;
        int m_numMaterials = wmesh->materialsCount();
        wMesh::Material *m_pMaterials = wmesh->materials();
        int m_numTriangles = wmesh->trianglesCount();
        wMesh::Triangle *m_pTriangles = wmesh->triangles();
        int m_numVertices = wmesh->verticesCount();
        wMesh::Vertex *m_pVertices = wmesh->vertices();
        for ( int i = 0; i < m_numMeshes; i++ )    
        {
            int materialIndex = m_pMeshes[i].m_materialIndex;
            
            float colorBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
            
            if ( materialIndex >= 0 ) {
                
                //glMaterialfv( GL_FRONT, GL_AMBIENT,   m_pMaterials[materialIndex].m_ambient );
                //glMaterialfv( GL_FRONT, GL_DIFFUSE,   m_pMaterials[materialIndex].m_diffuse );
                //glMaterialfv( GL_FRONT, GL_SPECULAR,  m_pMaterials[materialIndex].m_specular );
                //glMaterialfv( GL_FRONT, GL_EMISSION,  m_pMaterials[materialIndex].m_emissive );
                //glMaterialf(  GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess );
                    //container()->applyTexture( gw, wmesh->texture() );
                    //container()->setupColorTexture( gw, this );                
                }
            
            //with this now it loads the right colors
            if (sl==0)
            glColor3f( m_pMaterials[materialIndex].m_diffuse[0]*kl, m_pMaterials[materialIndex].m_diffuse[1]*kl, m_pMaterials[materialIndex].m_diffuse[2]*kl);
            else
            {
            //glColor4f( m_pMaterials[materialIndex].m_diffuse[0]*kl, m_pMaterials[materialIndex].m_diffuse[1]*kl, m_pMaterials[materialIndex].m_diffuse[2]*kl, 100.0);
            glColor3f(0,2.0,2.0);
            //glBlendFunc(GL_SRC_ALPHA,GL_ONE); //trasparenza
            }
            

            //glMaterialfv( GL_FRONT, GL_AMBIENT,   colorBlue);
            //glMaterialfv( GL_FRONT, GL_DIFFUSE,   colorBlue);
            //glMaterialfv( GL_FRONT, GL_SPECULAR,   colorBlue);
            //glMaterialfv( GL_FRONT, GL_EMISSION,   colorBlue);
            //glMaterialfv( GL_FRONT, GL_SHININESS,   colorBlue);


            glBegin( GL_TRIANGLES );
            
            for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ ) {
                int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
                const wMesh::Triangle* pTri = &m_pTriangles[triangleIndex];
                for ( int k = 0; k < 3; k++ ) {
                    int index = pTri->m_vertexIndices[k];
                    glNormal3fv( pTri->m_vertexNormals[k] );
                    glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
                    glVertex3fv( m_pVertices[index].m_location );
                }
            }
            glEnd();
        }
        //glDisable(GL_BLEND); //trasparenza
        glPopName();
        glPopMatrix();


//end mesh painting
}

void Viewer::init()
{
  setWindowTitle(tr("3D Robot/Environment"));
#ifdef EVOLANGUAGEIT 
  setWindowTitle(tr("3D Robot/Ambiente"));
#endif
  restoreStateFromFile();
  if (edubot_mesh == NULL)
  {
    //creating mesh
    
    edubot_mesh=new wMesh();
    edubot_mesh->loadMS3DModel("../bin/3dmodels/edubot.ms3d");
    //loading mesh
   }

    if (epuck_mesh == NULL)
  {
    //creating mesh
    
    epuck_mesh=new wMesh();
    epuck_mesh->loadMS3DModel("../bin/3dmodels/epuck.ms3d");
    //loading mesh
   }

   if (khepera_mesh == NULL)
  {
    //creating mesh
    
    khepera_mesh=new wMesh();
    khepera_mesh->loadMS3DModel("../bin/3dmodels/khepera.ms3d");
    //loading mesh
   }
 
  switch(robottype)
  {
  case 0:
      break;
  case 1:
          bodyText= new QImage(":/images/epuck3d2.png");
  headText = new QImage(":/images/epuckHead.png");
 

  break;
  case 2:
      //lego
       bodyText= new QImage(":/images/legorostro.png");
       headText = new QImage(":/images/legohead.png");

 
      break;

  }

  floorText= new QImage(":/images/txfabric.jpg");
  
  qglviewer::Vec minv(-1000.0f,-1000.0f,-1000.0f);
  qglviewer::Vec maxv(1000.0f,1000.0f,1000.0f);
  setSceneBoundingBox(minv,maxv);

  glLineWidth(30.0);
  glPointSize(100.0);
  //camera()->centerScene();
  
  //showEntireScene();
  //Opens help window
  //help();
}

QString Viewer::helpString() const
{
  QString text("<h2>S i m p l e V i e w e r</h2>");
  /*
  text += "Use the mouse to move the camera around the object. ";
  text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
  text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
  text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
  text += "Simply press the function key again to restore it. Several keyFrames define a ";
  text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
  text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
  text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
  text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
  text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
  text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
  text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
  text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
  text += "Press <b>Escape</b> to exit the viewer.";
  */
  return text;
}


void Viewer::postSelection(const QPoint& point)
{
  // Compute orig and dir, used to draw a representation of the intersecting line
  camera()->convertClickToLine(point, orig, dir);

  // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
  bool found;
  selectedPoint = camera()->pointUnderPixel(point, found);
  selectedPoint -= 0.01f*dir; // Small offset to make point clearly visible.
  // Note that "found" is different from (selectedObjectId()>=0) because of the size of the select region.

  if (selectedName() == -1)
  {
    //QMessageBox::information(this, "No selection",
    //             "No object selected under pixel " + QString::number(point.x()) + "," + QString::number(point.y()));
  selmode=0;
  }
  else
  {
    
      //QMessageBox::information(this, "Selection",
        //         "Robot number " + QString::number(selectedName()) + " selected under pixel " +
        //         QString::number(point.x()) + "," + QString::number(point.y()));
    selmode=1;
    cindividual=selectedName(); 
  }
}
void Viewer::keyPressEvent(QKeyEvent *e)
{
  // Defines the Alt+R shortcut.
  struct individual *pind;
  if (e->key() == Qt::Key_R && selmode==1 && (e->modifiers() == Qt::ShiftModifier))
  {
     

   pind = (ind + cindividual);
   pind->direction += 2.0;
   if (pind->direction > 360)
     pind->direction -= 360;
   
  }
  else if (e->key() == Qt::Key_Right && (e->modifiers() == Qt::ShiftModifier) && selmode==1)
  {
      pind = (ind + cindividual);
      pind->pos[0]+=10;

  }
    else if (e->key() == Qt::Key_Left && (e->modifiers() == Qt::ShiftModifier) && selmode==1)
  {
      pind = (ind + cindividual);
      pind->pos[0]-=10;

  }
      else if (e->key() == Qt::Key_Up && (e->modifiers() == Qt::ShiftModifier) && selmode==1)
  {
      pind = (ind + cindividual);
      pind->pos[1]-=10;

  }
          else if (e->key() == Qt::Key_Down && (e->modifiers() == Qt::ShiftModifier) && selmode==1)
  {
      pind = (ind + cindividual);
      pind->pos[1]+=10;

  }
  else
    QGLViewer::keyPressEvent(e);
    update3d();


//end
}
void Viewer::drawWithNames()
{
  // Draw spirals, pushing a name (id) for each of them
 // exit(0);
    struct individual *pind;
    int    team;
    for(team=0,pind=ind;team<nteam;team++,pind++)
   {
      
      switch(robottype)
      {
      case 0:
          //drawing khepera robot
          drawMesh(khepera_mesh,pind->pos[0],-pind->pos[1],-18,-pind->direction+180, team);
          break;
      case 1:

         
      drawMesh(epuck_mesh,pind->pos[0],-pind->pos[1],-26,-pind->direction+180, team);
      

      break;
      case 2:
     drawMesh(edubot_mesh,pind->pos[0],-pind->pos[1],-30,-pind->direction+180, team);

     
      
     break;

      }


   

    } 
    
}


void Viewer::update3d()
{
  update(0,0,1000,1000);
}

void Viewer::drawMark(float x, float y, int rbtype, int rbtid)
{
 //here we draw the robot marker, actually a colored disk on the top of each robot
    GLUquadricObj *mon;
    int ht, wt; //h height from floor, w disk radius
    
    //algorithm used to produce automaticallly different color marker for each robot
    int r,g,b;
    int rk, gk,bk;
    float rr, gg, bb;
    r=255;
    g=0;
    b=0;
    rk=125; //125,50,200
    gk=50;
    bk=200;
    rr=255-(rk*rbtid+r)%255;
    gg=(gk*rbtid+g)%255;
    bb=(bk*rbtid+b)%255;



    ht=120;
    wt=30;
    // in the 2d case we should compute int rcolor=(int)(rr/255.0) and so forth for the green and the blue component
    //end algorithm

    mon=gluNewQuadric();
    glColor3f((float)rr/128.0,(float)gg/128.0,(float)bb/128.0);
    //glColor3f(1.0,0,0);
    switch(rbtype)
    {
    case 0:
        ht=30;
        wt=15;
        break;
    case 1:
        ht=50;
        wt=15;
        break;
        
        
    case 2:
        ht=120;
        wt=30;
        break;

    }
        
    glPushMatrix();
       glTranslatef(x,y,ht);
       gluDisk(mon,0,wt,40,4);
       glPopMatrix();

}


void
update_rend3drobot()
{
   if (view!= NULL)
    view->update3d();
}

void 
init_rend3drobot()

{

  if (view != NULL) 
      view->show();

  if (view == NULL)
     {
     view=new Viewer();
     view->show();
     }
}





#endif
#endif

