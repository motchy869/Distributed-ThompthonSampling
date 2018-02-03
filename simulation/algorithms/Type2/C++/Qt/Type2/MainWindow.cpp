#include "common.h"
#include "Simulator.h"
#include "MainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMetaObject>
#include <QProgressBar>
#include <QThread>
#include <QVBoxLayout>

//QThreadによるマルチスレッド処理に関しては次が参考になる: https://qiita.com/hermit4/items/b1eaf6132fb06a30091f

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent)
{
	//UI
	setWindowTitle(tr("Type1 simulation"));
	QVBoxLayout *vBoxLayout = new QVBoxLayout();
		vBoxLayout->addWidget(m_label_basic_info = new QLabel(tr("basic info")));
			m_label_basic_info->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
			m_label_basic_info->setWordWrap(true);
		vBoxLayout->addStretch();
		vBoxLayout->addWidget(new QLabel(tr("current play progress")));
		vBoxLayout->addWidget(m_pb_current_play = new QProgressBar());
		vBoxLayout->addWidget(new QLabel(tr("whole progress")));
		vBoxLayout->addWidget(m_pb_sim = new QProgressBar());
	QWidget *centralWidget = new QWidget();
	centralWidget->setLayout(vBoxLayout);
	setCentralWidget(centralWidget);
	setMinimumWidth(640);

	//スレッド準備
	m_simThread = new QThread(this);
	m_simulator = new Simulator(nullptr);	//moveToThread するために親を null にする必要がある！
	m_simulator->moveToThread(m_simThread);	//所属するイベントループを変更
	connect(m_simThread, SIGNAL(started()), m_simulator, SLOT(run()));
	connect(m_simThread, SIGNAL(finished()), m_simulator, SLOT(deleteLater()));	//親がいないQObjectの派生クラスは自動的にdeleteされないため、お膳立てしてやる。
	connect(m_simulator, SIGNAL(update_basic_info_label(QString)), this, SLOT(update_basic_info_label(QString)));
	connect(m_simulator, SIGNAL(report_progress(int,int)), this, SLOT(update_progress_bar(int,int)));
		//終了の流れ
		connect(m_simulator, SIGNAL(sim_done()), m_simThread, SLOT(quit()));
		connect(m_simThread, SIGNAL(finished()), qApp, SLOT(quit()));
	m_simThread->start();
}

void MainWindow::update_basic_info_label(QString s)
{
	m_label_basic_info->setText(s);
}

void MainWindow::update_progress_bar(int prog_current, int prog_whole)
{
	Q_ASSERT(prog_current>=0 && prog_current<=100 && prog_whole>=0 && prog_whole<=100);
	m_pb_current_play->setValue(prog_current);
	m_pb_sim->setValue(prog_whole);
}

void MainWindow::closeEvent(QCloseEvent*) {	//ウィンドウを閉じた時にシミュレーションスレッドを終わらせる
	m_simThread->quit();
	m_simThread->deleteLater();
}

MainWindow::~MainWindow()
{
	m_simThread->exit();
	m_simThread->wait();
	qDebug() << "~MainWindow()";
}
