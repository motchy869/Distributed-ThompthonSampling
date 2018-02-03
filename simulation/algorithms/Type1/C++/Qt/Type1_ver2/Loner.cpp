#include "common.h"
#include "Environment.h"
#include "Loner.h"

#include <QDebug>

Loner::Loner(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta) : Player(ID, uniform_random_generator, env_pointer, prior_alpha, prior_beta)
{
	m_n = new int[m_K]; m_m = new int[m_K];
}

void Loner::init_play()
{
	std::fill_n(m_n, m_K, 0); std::fill_n(m_m, m_K, 0);
}

void Loner::action() {
	//QTextStream(stdout) << "player_ID = " << m_ID << endl;
	//乱数生成, 選択アーム決定
	int k_select = 0;
	double mu_tilde, mu_tilde_max = -1.0, x,y;
	for (int k=1; k<=m_K; ++k) {
		std::gamma_distribution<double> gamma_alpha(m_prior_alpha[k-1] + m_n[k-1], 1.0);
		std::gamma_distribution<double> gamma_beta(m_prior_beta[k-1] + m_n[k-1]-m_m[k-1], 1.0);
		x = gamma_alpha(*m_uniform_random_generator);
		y = gamma_beta(*m_uniform_random_generator);
		mu_tilde = x/(x+y);
		//QTextStream(stdout) << "k = " << k << ", m_n = " << m_n[k-1] << ", m_m = " << m_m[k-1] << ", mu_hat = " << static_cast<double>(m_m[k-1])/qMax(m_n[k-1],1) << ", mu_tilde = " << mu_tilde << endl;
		if (mu_tilde > mu_tilde_max) {k_select = k; mu_tilde_max = mu_tilde;}
	}
	++m_n[k_select-1];
	m_m[k_select-1] += m_env_pointer->make_reward(m_ID, k_select);
}

Loner::~Loner() {
	safe_array_delete(m_n); safe_array_delete(m_m);
	qDebug() << "~Loner()";
}
