#include "common.h"
#include "Environment.h"
#include "Loner.h"
#include "Agent.h"
#include "Simulator.h"

#include <chrono>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextStream>
#include <random>

extern QString env_file_path;
extern QString prior_file_path;
extern QString graph_file_path;
extern QString save_dir_path;
extern int T;
extern int repetition_num;

Simulator::Simulator(QObject *parent) : QObject(parent)
{
	//環境の設定読み込み
	QFile file(env_file_path);
	if (!file.open(QIODevice::ReadOnly)) {
		QTextStream(stderr) << "Couldn't open: " + env_file_path << "\n";
		this->~Simulator(); exit(EXIT_FAILURE);
	}
	QByteArray json_data = file.readAll();
	file.close();
	QJsonObject json(QJsonDocument::fromJson(json_data).object());
	m_envName = json["environment name"].toString();
	m_K = json["K"].toInt();	//アーム数
	m_mean_reward = new double[m_K]; for (int i=0; i<m_K; ++i) {m_mean_reward[i] = json["mu"].toArray()[i].toDouble();}	//期待報酬

	//事前分布の設定読み込み
	file.setFileName(prior_file_path);
	if (!file.open(QIODevice::ReadOnly)) {
		QTextStream(stderr) << "Couldn't open: " + prior_file_path << "\n";
		this->~Simulator(); exit(EXIT_FAILURE);
	}
	json_data = file.readAll();
	file.close();
	json = QJsonObject(QJsonDocument::fromJson(json_data).object());
	if (json["K"].toInt() != m_K) {
		QTextStream(stderr) << "Arm number in prior setting doesnt match to environment setting's one." << "\n";
		this->~Simulator(); exit(EXIT_FAILURE);
	}
	m_priorName = json["prior name"].toString();
	m_prior_alpha = new double[m_K]; m_prior_beta = new double[m_K];
	for (int k=0; k<m_K; ++k) {
		m_prior_alpha[k] = json["alpha"].toArray()[k].toDouble();
		m_prior_beta[k] = json["beta"].toArray()[k].toDouble();
	}
	//エージェントの設定読み込み
	file.setFileName(graph_file_path);
	if (!file.open(QIODevice::ReadOnly)) {
		QTextStream(stderr) << "Couldn't open: " + graph_file_path << "\n";
		this->~Simulator(); exit(EXIT_FAILURE);
	}
	json_data = file.readAll();
	file.close();
	json = QJsonObject(QJsonDocument::fromJson(json_data).object());
	m_graphName = json["graph name"].toString();
	m_N = json["number of agent"].toInt();	//エージェント数
	m_PerronMatrix = new double*[m_N]; for (int i=0; i<m_N; i++) {m_PerronMatrix[i] = new double[m_N];}	//領域確保
	for (int i=0; i<m_N; ++i) {
		for (int j=0; j<m_N; ++j) {m_PerronMatrix[i][j] = json["PerronMatrix"].toArray()[i].toArray()[j].toDouble();}
	}

	//メルセンヌ・ツイスタ
	typedef std::chrono::high_resolution_clock myclock;
	myclock::time_point beginning = myclock::now();
	myclock::duration d = myclock::now() - beginning;
	m_uniform_random_generator = new std::mt19937(d.count());

	m_env = new Environment(m_uniform_random_generator, m_K, m_N, m_mean_reward);	//環境オブジェクト生成

	//一匹狼生成
	m_loner = new Loner(0, m_uniform_random_generator, m_env, m_prior_alpha, m_prior_beta);
	m_regret_loner = new double[T]; std::fill_n(m_regret_loner, T, 0.0);

	//エージェント生成
	m_agents = new Agent*[m_N];
	m_regret_agents = new double*[m_N];
	for (int i=0; i<m_N; ++i) {
		m_agents[i] = new Agent(i+1, m_uniform_random_generator, m_env, m_prior_alpha, m_prior_beta, m_N, m_PerronMatrix[i]);
		m_regret_agents[i] = new double[T]; std::fill_n(m_regret_agents[i], T, 0.0);
	}
	for (int i=0; i<m_N; ++i) {m_agents[i]->set_agents_pointer(m_agents);}

	m_regret_sysAvg = new double[T]; std::fill_n(m_regret_sysAvg, T, 0.0);
}

void Simulator::run()
{
	//UI準備
	QString msg; QTextStream ts(&msg);
	ts << "[" + tr("environment") + "]: <strong>" << m_envName << "</strong><br/>"
	   << "[" + tr("prior") + "]: <strong>" << m_priorName << "</strong><br/>"
	   << "[" + tr("graph") + "]: <strong>" << m_graphName << "</strong><br/>"
	   << "[" + tr("save directory path") + "]:<br/>" << save_dir_path << "<br/><br/>"
	   << "K=<font color=\"red\">" << m_K << "</font>, N=<font color=\"red\">" << m_N << "</font>, T=<font color=\"red\">" << T << "</font>" << ", rep=<font color=\"red\">" << repetition_num << "</font>";
	emit(update_basic_info_label(msg));

	msg.clear();
	ts << "[" + tr("environment") + "]: " << m_envName << "\n"
	   << "[" + tr("prior") + "]: " << m_priorName << "\n"
	   << "[" + tr("graph") + "]: " << m_graphName << "\n"
	   << "[" + tr("save directory path") + "]: " << save_dir_path << "\n\n"
	   << "K=" << m_K << ", N=" << m_N << ", T=" << T << ", rep=" << repetition_num << "\n\n";
	QTextStream(stdout) << msg;

	//繰り返し実験
	int progress_whole = 0;
	QElapsedTimer timer_ui_update; timer_ui_update.start();
	for (int rep=0; rep < repetition_num; ++rep) {
		//QTextStream(stdout) << QString("-").repeated(80) << "\n" << "repetition: " << rep+1 << "\n\n";
		//記憶をリセット
		m_env->init_play();
		m_loner->init_play();
		for (int i=0; i<m_N; ++i) {m_agents[i]->init_play();}

		//1ラウンド
		int progress_current_play = 0;
		for (int t=0; t<T; ++t) {
			//QTextStream(stdout) << "\nt = " << t << "\n";
			//意思決定
			m_loner->action();
			for (int i=0; i<m_N; ++i) {m_agents[i]->action();}

			//情報交換
			for (int i=0; i<m_N; ++i) {m_agents[i]->buffering();}
			for (int i=0; i<m_N; ++i) {m_agents[i]->communicate();}

			//Regret記録。繰り返し実験の平均を取るために予め実験回数で割っておく。
			m_regret_loner[t] += m_env->get_regret(0)/repetition_num;
			for (int i=1; i<=m_N; ++i) {
				m_regret_agents[i-1][t] += m_env->get_regret(i)/repetition_num;
				m_regret_sysAvg[t] += m_env->get_regret(i)/repetition_num/m_N;
			}

			//UI
			if (timer_ui_update.elapsed() > 100) {
				progress_current_play = int(100.0*(t+1)/T);
				progress_whole = int(100.0*(rep*T+(t+1))/(repetition_num*T));
				emit(report_progress(progress_current_play, progress_whole));
				timer_ui_update.restart();
			}
		}
	}

	//結果を出力
	QDir save_dir(save_dir_path);
	if (!save_dir.exists()) {save_dir.mkpath(save_dir_path);}
	QDateTime dt(QDateTime::currentDateTimeUtc());
	QString save_file_path(save_dir_path + "/result_" + dt.date().toString("yyyy-MM-dd-") + dt.time().toString("hh:mm:ss:zzz") + ".json");
	QFile file(save_file_path);
	if (!file.open(QIODevice::ReadWrite)) {
		QTextStream(stderr) << "Couldn't create: " + save_file_path << "\n";
		this->~Simulator(); exit(EXIT_FAILURE);
	}
	QJsonObject jsonObj;
	jsonObj["algorithm"] = "Type1";
	jsonObj["date"] = dt.date().toString("yyyy-MM-dd-") + dt.time().toString("hh:mm:ss:zzz");
	jsonObj["environment name"] = m_envName;
	jsonObj["prior name"] = m_priorName;
	jsonObj["graph name"] = m_graphName;
	jsonObj["K"] = m_K;
	jsonObj["N"] = m_N;
	jsonObj["T"] = T;
	jsonObj["repetition number"] = repetition_num;
	QJsonObject jsonObj_regret;
	QJsonArray jsonArray_regret_loner, jsonArray_regret_sysAvg;
	for (int t=0; t<T; ++t) {
		jsonArray_regret_loner.append(m_regret_loner[t]);
		jsonArray_regret_sysAvg.append(m_regret_sysAvg[t]);

	}
	jsonObj_regret["loner"] = jsonArray_regret_loner;
	jsonObj_regret["system average"] = jsonArray_regret_sysAvg;
	for (int i=0; i<m_N; ++i) {
		QJsonArray jsonArray_regret_agent;
		for (int t=0; t<T; ++t) {jsonArray_regret_agent.append(m_regret_agents[i][t]);}
		jsonObj_regret[std::to_string(i+1).c_str()] = jsonArray_regret_agent;
	}
	jsonObj["regret"] = jsonObj_regret;
	QJsonDocument jsonDoc(jsonObj);
	QByteArray ba(jsonDoc.toJson());
	file.write(ba);
	file.close();

	QTextStream(stdout) << QString("-").repeated(80) << "\n";
	QTextStream(stdout) << "Simulation done.\n";
	emit(sim_done());
}

Simulator::~Simulator()
{
	safe_delete(m_uniform_random_generator);
	safe_array_delete(m_mean_reward);
	safe_array_delete(m_prior_alpha); safe_array_delete(m_prior_beta);
	for (int i=0; i<m_N; ++i) {safe_array_delete(m_PerronMatrix[i]); safe_delete(m_agents[i]);}
	safe_array_delete(m_PerronMatrix);
	safe_delete(m_env);
	safe_delete(m_loner);
	safe_array_delete(m_agents);
	safe_delete(m_regret_loner);
	qDebug() << "~Simulator()";
}
