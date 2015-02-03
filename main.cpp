#include <QtCore/QCoreApplication>
#include "ripper.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	Ripper rip;
	rip.RunMe(argc, argv);

	return 0;
}
