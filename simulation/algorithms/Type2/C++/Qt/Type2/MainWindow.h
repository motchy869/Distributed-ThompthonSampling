#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QProgressBar;
class QThread;
class Simulator;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	protected:
		QLabel *m_label_basic_info;	//基本情報
		QProgressBar *m_pb_current_play, *m_pb_sim;	//現在のプレイとシミュレーション全体の進行状況
		Simulator *m_simulator;
		QThread *m_simThread;

	public:
		MainWindow(QWidget *parent);
		~MainWindow();

	protected:
		void closeEvent(QCloseEvent*);

	public slots:
		void update_basic_info_label(QString s);	//基本情報ラベルの内容を更新
		void update_progress_bar(int prog_current, int prog_whole);	//GameThreadから進捗を受け取ってプログレスバーを更新
};

#endif // MAINWINDOW_H
