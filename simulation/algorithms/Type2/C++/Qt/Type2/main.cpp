/*
 * 分散強調型Banditのシミュレーションプログラム。
 * コンソールから呼び出して使う。
 * 呼び出し例 : Type2 --env "env.json" --prior "prior.json" --graph "graph.json" --T 1000 --rep 10 --save "./save/"
 * 起動後、自動的にシミュレーションが始まり、プレイ時間が終了したら結果をセーブして終了する。
 * セーブするファイル名の規則 : result_yyyy-MM-dd-hh:mm:ss:zzz.json
*/

#include "common.h"
#include "Simulator.h"
#include "MainWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>
#include <QtGlobal>

//ファイル,ディレクトリのパス
QDir app_dir;	//実行ファイルのパス
QString env_file_path;		//環境
QString prior_file_path;	//事前分布
QString graph_file_path;	//グラフ
QString save_dir_path;		//出力保存先ディレクトリ

int T;	//ゲーム時間
int repetition_num;	//実験繰り返し回数

void cmdLineAnalysis(int argc, char *_argv[]);

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app_dir = QCoreApplication::applicationDirPath();
	cmdLineAnalysis(argc, argv);
	MainWindow w(nullptr);
	w.show();
	return app.exec();
}

void cmdLineAnalysis(int argc, char *_argv[]) {	//コマンドライン引数の解析。(参考: http://kengosawa2.hatenablog.com/entry/2014/12/16/135101)
	QStringList argv; for (int i=0; i<argc; ++i) {argv.append(_argv[i]);}
	QCommandLineParser parser;
	parser.addHelpOption();	//--help,-hをQCommandLineParserで自動で処理するように依頼

	//オプションの情報設定
	QCommandLineOption envOpt(QStringList() << "env",	//オプション文字列
							  "Environment setting file path.",			//ヘルプ表示の際のオプション説明
							  "env.json",								//オプション指定例
							  "");										//デフォルトの指定
	QCommandLineOption priorOpt(QStringList() << "prior", "Prior distribusion setting file path.", "prior.json", "");
	QCommandLineOption graphOpt(QStringList() << "graph", "Graph setting file path.", "graph.json", "");
	QCommandLineOption TOpt(QStringList() << "T", "game length.", "1000", "");
	QCommandLineOption repOpt(QStringList() << "rep", "Number of repetition.", "30", "1");
	QCommandLineOption saveOpt(QStringList() << "save", "Result save destination.", "./save/", "");

	//設定したオプション情報をparserに追加
	parser.addOption(envOpt);
	parser.addOption(priorOpt);
	parser.addOption(graphOpt);
	parser.addOption(TOpt);
	parser.addOption(repOpt);
	parser.addOption(saveOpt);

	//コマンドラインの事前解析
	//実際のパース処理を行うprocess()では情報設定済以外のオプションが存在すると内部でexit()をコールしてしまうため、事前にparse()してチェックしておく。
	if (!parser.parse(argv)) {	//不明なオプションが設定されている場合
		QString error_str("Unknown option --");
		for (int i=0; i<parser.unknownOptionNames().count(); ++i) {
			error_str.append("\"" + parser.unknownOptionNames().at(i) + "\"");
		}
		QTextStream(stderr) << error_str << endl;
		exit(EXIT_FAILURE);
	}

	parser.process(argv);	//解析実行

	//各オプションの取得
	QFileInfo fi;
	if (parser.isSet(envOpt)) {
		fi.setFile(app_dir.absoluteFilePath(parser.value(envOpt)));
		env_file_path = fi.absoluteFilePath();
	} else {
		QTextStream(stderr) << "Option --env must be set." << endl; exit(EXIT_FAILURE);
	}
	if (parser.isSet(priorOpt)) {
		fi.setFile(app_dir.absoluteFilePath(parser.value(priorOpt)));
		prior_file_path = fi.absoluteFilePath();
	} else {
		QTextStream(stderr) << "Option --prior must be set." << endl; exit(EXIT_FAILURE);
	}
	if (parser.isSet(graphOpt)) {
		fi.setFile(app_dir.absoluteFilePath(parser.value(graphOpt)));
		graph_file_path = fi.absoluteFilePath();
	} else {
		QTextStream(stderr) << "Option --graph must be set." << endl; exit(EXIT_FAILURE);
	}
	if (parser.isSet(TOpt)) {
		T = parser.value(TOpt).toInt();
	} else {
		QTextStream(stderr) << "Option --T must be set." << endl; exit(EXIT_FAILURE);
	}
	if (parser.isSet(repOpt)) {
		repetition_num = parser.value(repOpt).toInt();
	} else {repetition_num = 1;}
	if (parser.isSet(saveOpt)) {
		fi.setFile(app_dir.absoluteFilePath(parser.value(saveOpt)) + "/");	//念の為"/"を付けて万全にしておく
		save_dir_path = fi.absolutePath();
	} else {
		QTextStream(stderr) << "Option --save must be set." << endl; exit(EXIT_FAILURE);
	}
}
