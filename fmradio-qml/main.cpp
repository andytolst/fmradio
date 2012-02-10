/*
 *  fmradio-qml - UI for FM radio
 *  Copyright (C) 2012 Andrey Kozhanov <andy.tolst@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <QtGui/QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include "tunermodel.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#ifdef HARMATTAN_BOOSTER
    QScopedPointer<QApplication> app(MDeclarativeCache::qApplication(argc, argv));
#else
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
#endif

    TunerModel tunerModel;

    QDeclarativeView viewer;
    QDeclarativeContext *ctxt = viewer.rootContext();
    ctxt->setContextProperty("tunerModel", &tunerModel);

#if defined(MEEGO_OS)
    viewer.setSource(QUrl("qrc:/qml/MainWindowDevice.qml"));
    viewer.showFullScreen();
#else
    viewer.setSource(QUrl("qrc:/qml/MainWindowDesktop.qml"));
    viewer.show();
#endif
    return app->exec();
}
