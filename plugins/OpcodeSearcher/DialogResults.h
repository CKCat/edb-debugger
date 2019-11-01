
#ifndef ROPTOOL_DIALOG_RESULTS_H
#define ROPTOOL_DIALOG_RESULTS_H

#include "ResultsModel.h"
#include "ui_DialogResults.h"
#include <QDialog>
#include <QSortFilterProxyModel>

class QSortFilterProxyModel;

namespace OpcodeSearcherPlugin {

class ResultsModel;

class DialogResults : public QDialog {
	Q_OBJECT

public:
	explicit DialogResults(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public:
	void addResult(const Result &result);

private Q_SLOTS:
	void on_tableView_doubleClicked(const QModelIndex &index);

public:
	int resultCount() const;

private:
	Ui::DialogResults      ui;
	ResultsModel *         model_;
	QSortFilterProxyModel *filterModel_;
};

#endif
}
