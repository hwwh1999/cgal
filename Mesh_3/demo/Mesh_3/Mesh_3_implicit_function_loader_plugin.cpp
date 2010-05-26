// Copyright (c) 2010 INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s)     : Stephane Tayeb
//
//******************************************************************************
// File Description : 
//******************************************************************************

#include "Polyhedron_demo_plugin_interface.h"
#include "Polyhedron_demo_plugin_helper.h"
#include "Implicit_function_interface.h"
#include "Scene_implicit_function_item.h"
#include "ui_Function_dialog.h"

#include <iostream>

#include <QAction>
#include <QMainWindow>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>


class Mesh_3_implicit_function_loader_plugin :
  public QObject,
  protected Polyhedron_demo_plugin_helper
{
  Q_OBJECT
  Q_INTERFACES(Polyhedron_demo_plugin_interface);
  
public:
  Mesh_3_implicit_function_loader_plugin();
  virtual ~Mesh_3_implicit_function_loader_plugin() {}
  
  virtual void init(QMainWindow* mainWindow, Scene_interface* scene_interface);
  
  QList<QAction*> actions() const
  {
    return QList<QAction*>() << actionLoadFunction_;
  }
  
public slots:
  void load_function() const;
  
private:
  void load_function_plugins();
  
private:
  QAction* actionLoadFunction_;
  QList<Implicit_function_interface*> functions_;
};



Mesh_3_implicit_function_loader_plugin::
Mesh_3_implicit_function_loader_plugin()
  : actionLoadFunction_(NULL)
{
  load_function_plugins();
}


void
Mesh_3_implicit_function_loader_plugin::
init(QMainWindow* mainWindow, Scene_interface* scene_interface)
{
  this->scene = scene_interface;
  this->mw = mainWindow;
  
  actionLoadFunction_ = new QAction("Load Implicit Function", mw);
  if( NULL != actionLoadFunction_ )
  {
    connect(actionLoadFunction_, SIGNAL(triggered()), this, SLOT(load_function()));
  }
}


void
Mesh_3_implicit_function_loader_plugin::
load_function() const
{
  QDialog dialog(mw);
  Ui::FunctionDialog ui;
  ui.setupUi(&dialog);
  
  connect(ui.buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  connect(ui.buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
  
  // Add loaded functions to the dialog
  int i=0;
  Q_FOREACH( Implicit_function_interface* f, functions_ )
  {
    ui.functionList->insertItem(i++,f->name());
  }

  // Open window
  int return_code = dialog.exec();
  if(return_code == QDialog::Rejected) { return; }
  
  // Get selected function
  i = ui.functionList->currentIndex();
  Implicit_function_interface* function = functions_[i];
  
  // Create Scene_implicit_function object and add it to the framework
  Scene_implicit_function_item* item =
    new Scene_implicit_function_item(function);
  
  item->setName(tr("%1").arg(function->name()));
  item->setRenderingMode(FlatPlusEdges);

  const Scene_interface::Item_id index = scene->mainSelectionIndex();
  scene->itemChanged(index);
    
  Scene_interface::Item_id new_item_id = scene->addItem(item);
  scene->setSelectedItem(new_item_id);
}

void
Mesh_3_implicit_function_loader_plugin::
load_function_plugins()
{
  QDir pluginsDir(qApp->applicationDirPath());
  pluginsDir.cd("implicit_functions");
  
  Q_FOREACH (QString fileName, pluginsDir.entryList(QDir::Files))
  {
    if ( fileName.contains("plugin") && QLibrary::isLibrary(fileName) )
    {
      qDebug("    + Loading Function \"%s\"...", fileName.toUtf8().data());
      QPluginLoader loader;
      loader.setFileName(pluginsDir.absoluteFilePath(fileName));
      QObject *function_plugin = loader.instance();
      if ( NULL != function_plugin )
      {
        Implicit_function_interface* function =
          qobject_cast<Implicit_function_interface*>(function_plugin);
        
        if ( NULL != function )
        {
          functions_ << function;
        }
      }
    }
  }
}


#include <QtPlugin>
Q_EXPORT_PLUGIN2(Mesh_3_implicit_function_loader_plugin, Mesh_3_implicit_function_loader_plugin);
#include "Mesh_3_implicit_function_loader_plugin.moc"