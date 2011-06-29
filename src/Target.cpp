/**************************************************************************
 *  Copyright 2007,2008,2009 KISS Institute for Practical Robotics        *
 *                                                                        *
 *  This file is part of KISS (Kipr's Instructional Software System).     *
 *                                                                        *
 *  KISS is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  KISS is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with KISS.  Check the LICENSE file in the project root.         *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#include "Target.h"
#include "TargetInterface.h"

#include <QtGlobal>
#include <QDir>
#include <QApplication>
#include <QStringListIterator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "TargetManager.h"

// Constructor & Destructor
Target::Target(QObject *parent) : QObject(parent) {}
Target::~Target() {}

// Sets the target file and loads the associated plugin file
bool Target::setTargetFile(const QString& filename)
{
	QSettings settings(filename, QSettings::IniFormat);

	/* Determine the plugin's name */
	if(settings.value("base_target").toString().length() > 0)
		m_targetName = settings.value("base_target").toString();
	else
		m_targetName = settings.value("name").toString();

	/* Load the plugin */
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(!targetInterface) return false;

	targetInterface->setTargetFile(filename);
	m_targetFileName = filename;

	return true;
}

QStringList Target::errorMessages()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(targetInterface) return targetInterface->getErrorMessages();
	
	qWarning("Target::getErrorList: Invalid m_targetInterface");
	return QStringList();
}

QStringList Target::warningMessages()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(targetInterface) return targetInterface->getWarningMessages();
		
	qWarning("Target::getWarningList: Invalid m_targetInterface");
	return QStringList();
}

QStringList Target::linkerMessages()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(targetInterface) return targetInterface->getLinkerMessages();
		
	qWarning("Target::getLinkErrorList: Invalid m_targetInterface");
	return QStringList();
}

QStringList Target::verboseMessages()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(targetInterface) return targetInterface->getVerboseMessages();

	qWarning("Target::getVerboseMessages() Invalid m_targetInterface");
	return QStringList();
}

QList<QAction*> Target::actionList()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	if(targetInterface) return targetInterface->getActionList();
	
	qWarning("Target::getActionList() Invalid m_targetInterface");
	return QList<QAction*>();
}

QStringList Target::sourceExtensions()
{
	QSettings settings(m_targetFileName, QSettings::IniFormat);
	QString extensions = settings.value("extensions").toString();
	qWarning() << "Extensions:" << extensions;
	return extensions.split("|");
}

QString Target::defaultExtension()
{
	QSettings settings(m_targetFileName, QSettings::IniFormat);
	QString extension = settings.value("default_extension").toString();
	
	return extension;
}

QString Target::sourceTemplate()
{
	QSettings settings(m_targetFileName, QSettings::IniFormat);
	QString filePath = settings.value("template").toString();
	
	if(!QDir::isAbsolutePath(filePath))
		filePath = QDir::currentPath() + "/" + filePath;
	
	QFile file(filePath);
	
	if(!file.open(QIODevice::ReadOnly))
		return QString();
	QTextStream fileStream(&file);
	QString text = fileStream.readAll(); 
	return text;
}

QString Target::targetManualPath()
{
	QSettings settings(m_targetFileName, QSettings::IniFormat);
	QString filePath = settings.value("manual").toString();
	
	if(filePath == "")
		return QString();
	
	if(!QDir::isAbsolutePath(filePath))
		filePath = QDir::currentPath() + "/" + filePath;
	
	return filePath;
}

bool Target::hasDownload()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasDownload();
}

bool Target::hasCompile()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasCompile();
}

bool Target::hasRun()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasRun();
}

bool Target::hasStop()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasStop();
}

bool Target::hasSimulate()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasSimulate();
}

bool Target::hasDebug()
{
	TargetInterface* targetInterface = TargetManager::ref().get(m_targetName);
	return targetInterface && targetInterface->hasDebug();
}

bool Target::compile(const QString& filename)
{
	if(!hasCompile()) return false;
	return TargetManager::ref().get(m_targetName)->compile(filename, m_port);
}

bool Target::download(const QString& filename)
{
	if(!hasDownload()) return false;
	return TargetManager::ref().get(m_targetName)->download(filename, m_port);
}

bool Target::run(const QString& filename)
{
	if(!hasRun()) return false;
	return TargetManager::ref().get(m_targetName)->run(filename, m_port);
}

void Target::stop()
{
	if(!hasStop()) return;
	TargetManager::ref().get(m_targetName)->stop(m_port);
}

bool Target::simulate(const QString& filename)
{
	if(!hasSimulate()) return false;
	return TargetManager::ref().get(m_targetName)->simulate(filename, m_port);
}

DebuggerInterface* Target::debug(const QString& filename)
{
	if(!hasDebug()) return 0;
	return TargetManager::ref().get(m_targetName)->debug(filename, m_port);
}

void Target::setPort(const QString& port) { m_port = port; }
const QString& Target::port() { return m_port; }