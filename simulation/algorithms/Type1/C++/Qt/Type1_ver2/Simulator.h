#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <random>

class Environment;
class Loner;
class Agent;

class Simulator : public QObject
{
	Q_OBJECT

	protected:
		std::mt19937 *m_uniform_random_generator;	//一様乱数生成器
		QString m_envName;	//環境名
		Environment *m_env;	//環境
		int m_K;	//アーム数
		double *m_mean_reward;	//期待報酬
		QString m_priorName;	//事前分布名
		double *m_prior_alpha, *m_prior_beta;	//事前分布のα,β
		Loner *m_loner;	//一匹狼
		QString m_graphName;	//グラフ名
		int m_N;	//エージェント数
		double **m_PerronMatrix;	//ペロン行列
		Agent **m_agents;	//エージェント

		//繰り返し実験の平均値
		double *m_regret_loner;	//一匹狼のRegretの時系列データ。
		double **m_regret_agents;	//エージェントのRegretの時系列データ。[識別ID][時刻]
		double *m_regret_sysAvg;	//システム平均Regretの時系列データ。

	public:
		Simulator(QObject *parent=nullptr);
		~Simulator();
		void save_result();	//結果をファイルに保存

	signals:
		void update_basic_info_label(QString s);	//基本情報ラベルの内容を更新
		void report_progress(int prog_current, int prog_whole);	//UIスレッドに進捗を報告
		void sim_done();

	public slots:
		void run();	//シミュレーション実行
};

#endif // SIMULATOR_H
