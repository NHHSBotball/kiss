#include "GccOutput.h"

#include <QIODevice>

#define ERROR_KEY "error"
#define LINKER_KEY "linker"
#define WARNING_KEY "warning"
#define VERBOSE_KEY "verbose"

CompileResult GccOutput::processCompilerOutput(QIODevice* in)
{
	bool foundError = false;
	bool foundWarning = false;
	
	QStringList errorMessages;
	QStringList warningMessages;
	QStringList verboseMessages;

	while(in->canReadLine()) {
		QString input = QString::fromLocal8Bit(in->readLine());
		QString output;

		input.remove(QRegExp("\\r*\\n$"));

		verboseMessages << input;

		input.remove(QRegExp("^C\\:"));
		input.remove(QRegExp("^/.*/(?=\\S*\\:)"));

		output += input.section(':',0,0) + ":";

		if(input.section(':',2,2).length() > 0) {
			output += input.section(':',1,1) + ":";
			output += input.section(':',2,2).remove(' ') + ":";
			output += input.section(':', 3);
		} else output += input.section(':', 1);

		if(output.section(':', 2, 2) == ERROR_KEY) {
			errorMessages << output;
			foundError = true;
		} else if(output.section(':', 2, 2) == WARNING_KEY) {
			warningMessages << output;
			foundWarning = true;
		} else {
			errorMessages << input;
			warningMessages << input;
		}
	}
	
	if(!foundError && !foundWarning) warningMessages.clear();
	else if(!foundError) errorMessages.clear();
	else if(!foundWarning) warningMessages.clear();
	
	QMap<QString, QStringList> ret;
	ret[ERROR_KEY] = errorMessages;
	ret[WARNING_KEY] = warningMessages;
	ret[VERBOSE_KEY] = verboseMessages;
	
	return CompileResult(true, ret);
}

CompileResult GccOutput::processLinkerOutput(QIODevice* in)
{
	QStringList verboseMessages;
	QStringList linkerMessages;
	
	while(in->canReadLine()) {
		QString line = QString::fromLocal8Bit(in->readLine()).remove(QRegExp("\\r*\\n$"));
		verboseMessages << line;
		linkerMessages << line;
	}
	
	QMap<QString, QStringList> ret;
	ret[LINKER_KEY] = linkerMessages;
	ret[VERBOSE_KEY] = verboseMessages;
	
	return CompileResult(true, ret);
}
