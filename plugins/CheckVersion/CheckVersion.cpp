/*
Copyright (C) 2006 - 2015 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CheckVersion.h"
#include "OptionsPage.h"
#include "edb.h"

#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>

namespace CheckVersionPlugin {

//------------------------------------------------------------------------------
// Name: CheckVersion
// Desc:
//------------------------------------------------------------------------------
CheckVersion::CheckVersion(QObject *parent) : QObject(parent) {
}

//------------------------------------------------------------------------------
// Name: private_init
// Desc:
//------------------------------------------------------------------------------
void CheckVersion::privateInit() {
	QSettings settings;
	if(settings.value("CheckVersion/check_on_start.enabled", true).toBool()) {
		doCheck();
	}
}

//------------------------------------------------------------------------------
// Name: options_page
// Desc:
//------------------------------------------------------------------------------
QWidget *CheckVersion::optionsPage() {
	return new OptionsPage;
}

//------------------------------------------------------------------------------
// Name: menu
// Desc:
//------------------------------------------------------------------------------
QMenu *CheckVersion::menu(QWidget *parent) {

	Q_ASSERT(parent);

	if(!menu_) {
		menu_ = new QMenu(tr("CheckVersion"), parent);
		menu_->addAction(tr("&Check For Latest Version"), this, SLOT(showMenu()));
	}

	return menu_;
}

//------------------------------------------------------------------------------
// Name: do_check
// Desc:
//------------------------------------------------------------------------------
void CheckVersion::doCheck() {

	if(!network_) {
		network_ = new QNetworkAccessManager(this);
		connect(network_, &QNetworkAccessManager::finished, this, &CheckVersion::requestFinished);
	}

	//network_->abort(); //?

	const QUrl update_url("http://codef00.com/projects/debugger-latest");
	const QNetworkRequest request(update_url);

	setProxy(update_url);

	network_->get(request);
}

//------------------------------------------------------------------------------
// Name: set_proxy
// Desc:
//------------------------------------------------------------------------------
void CheckVersion::setProxy(const QUrl &url) {

	QNetworkProxy proxy;

#ifdef Q_OS_LINUX
	Q_UNUSED(url)
	auto proxy_str = QString::fromUtf8(qgetenv("HTTP_PROXY"));
	if(proxy_str.isEmpty()) {
		proxy_str = QString::fromUtf8(qgetenv("http_proxy"));
	}

	if(!proxy_str.isEmpty()) {
		const QUrl proxy_url = QUrl::fromUserInput(proxy_str);
		proxy = QNetworkProxy(
			QNetworkProxy::HttpProxy,
			proxy_url.host(),
			proxy_url.port(80),
			proxy_url.userName(),
			proxy_url.password());
	}

#else
	QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(url));
	if(proxies.size() >= 1) {
		proxy = proxies.first();
	}
#endif
	network_->setProxy(proxy);
}

//------------------------------------------------------------------------------
// Name: showMenu
// Desc:
//------------------------------------------------------------------------------
void CheckVersion::showMenu() {
	initialCheck_ = false;
	doCheck();
}

//------------------------------------------------------------------------------
// Name: requestFinished
// Desc:
//------------------------------------------------------------------------------
void CheckVersion::requestFinished(QNetworkReply *reply) {

    if(reply->error() != QNetworkReply::NoError) {
		if(!initialCheck_) {
			QMessageBox::critical(
                nullptr,
				tr("An Error Occured"),
				reply->errorString());
		}
	} else {
		const QByteArray result = reply->readAll();
		const QString s = result;

		qDebug("comparing versions: [%d] [%d]", edb::v1::int_version(s), edb::v1::edb_version());

		if(edb::v1::int_version(s) > edb::v1::edb_version()) {
			QMessageBox::information(
                nullptr,
				tr("New Version Available"),
				tr("There is a newer version of edb available: <strong>%1</strong>").arg(s));
		} else {
			if(!initialCheck_) {
				QMessageBox::information(
                    nullptr,
					tr("You are up to date"),
					tr("You are running the latest version of edb"));
			}
		}
	}

    reply->deleteLater();
	initialCheck_ = false;
}

}
