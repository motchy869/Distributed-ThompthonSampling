#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <random>

class Environment
{
	public:
		Environment(std::mt19937 *uniform_random_generator, int K, int N, double *mean_rewards);
		~Environment();
		void init_play();	//プレイの初期化
		int make_reward(int agent_ID, int k);	//選択されたアームに応じて報酬を返す
		double get_regret(int agent_ID);	//Regretを返す

	public:
		std::mt19937 *m_uniform_random_generator;	//一様乱数生成器
		const int m_K;	//アームの数
		const int m_N;	//エージェントの数

	protected:
		double *m_mean_rewards;	//アームの期待報酬
		std::bernoulli_distribution *m_arms;	//アーム
		double m_regret_loner;	//一匹狼のRegret
		double *m_regret_agent;	//エージェントのRegret
};

#endif // ENVIRONMENT_H
