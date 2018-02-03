#ifndef LONER_H
#define LONER_H

#include "Player.h"

class Loner : public Player
{
	protected:
		int *m_n, *m_m;	//アームの選択回数と当たり回数

	public:
		Loner(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta);
		~Loner();
		void init_play();	//プレイの初期化
		void action();	//アームを引く
};

#endif // LONER_H
