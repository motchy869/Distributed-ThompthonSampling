#ifndef AGENT_H
#define AGENT_H

#include "Player.h"

class Environment;

class Agent : public Player
{
	public:
		const int m_N;	//エージェントの総数

	protected:
		double *m_neighbor_weight;	//隣人の重み。ペロン行列の、自分に対応する行。
		Agent **m_agents_pointer;	//他のエージェントへのポインタ。
		double *m_n_hat, *m_n_hat_transmission_buffer;	//アームの選択回数の推定値と、その送信用バッファ
		double *m_m_hat, *m_m_hat_transmission_buffer;	//アームの累積報酬の推定値、その送信用バッファ

	public:
		Agent(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta, int N, double *neighbor_weight);
		~Agent();
		void set_agents_pointer(Agent **agents_pointer);
		void init_play();	//プレイの初期化
		void action();	//アームを引く
		void buffering();	//現在値を送信バッファに退避
		double get_m_n_hat(int k); double get_m_m_hat(int k);	//選択回数,累積報酬の推定値(送信バッファの値)を返す
		void communicate();	//情報交換
};

#endif // AGENT_H
