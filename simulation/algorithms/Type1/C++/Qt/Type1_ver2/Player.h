#ifndef PLAYER_H
#define PLAYER_H

#include <random>

class Environment;

class Player
{
	public:
		Player(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta);
		virtual ~Player();
		virtual void init_play() = 0;	//プレイの初期化
		virtual void action() = 0;	//アームを引く

	public:
		const int m_ID;	//識別ID
		std::mt19937 *m_uniform_random_generator;	//一様乱数生成器
		Environment *m_env_pointer;	//環境へのポインタ。環境とのやりとりはこれを通じて行う。
		const int m_K;	//アームの総数
		double *m_prior_alpha, *m_prior_beta;	//事前分布のα,β

};

#endif // PLAYER_H
