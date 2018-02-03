#include "common.h"
#include "Environment.h"
#include "Agent.h"

#include <QDebug>
#include <QTextStream>
#include <QtMath>

Agent::Agent(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta, int N, double *neighbor_weight, double delta) : Player(ID, uniform_random_generator, env_pointer, prior_alpha, prior_beta), m_N(N), m_neighbor_weight(neighbor_weight), m_delta(delta)
{
	m_n_hat = new double[m_K]; m_n_hat_transmission_buffer = new double[m_K];
	m_m_hat = new double[m_K]; m_m_hat_transmission_buffer = new double[m_K];
	m_n_hat_dag = new double[m_K]; m_m_hat_dag = new double[m_K];
}

void Agent::set_agents_pointer(Agent **agents_pointer) {
	m_agents_pointer = agents_pointer;
}

void Agent::init_play()
{
	std::fill_n(m_n_hat, m_K, 0.0); std::fill_n(m_m_hat, m_K, 0.0);
	std::fill_n(m_n_hat_dag, m_K, 0.0); std::fill_n(m_m_hat_dag, m_K, 0.0);
}

void Agent::action()
{
	//QTextStream(stdout) << "player_ID = " << m_ID << endl;
	//乱数生成, 選択アーム決定
	int k_select = 0;
	double mu_tilde, mu_tilde_max = -1.0, x,y;
	for (int k=1; k<=m_K; ++k) {
		std::gamma_distribution<double> gamma_alpha(m_prior_alpha[k-1] + m_N*m_n_hat_dag[k-1], 1.0);
		std::gamma_distribution<double> gamma_beta(m_prior_beta[k-1] + m_N*(m_n_hat_dag[k-1]-m_m_hat_dag[k-1]), 1.0);
		x = gamma_alpha(*m_uniform_random_generator);
		y = gamma_beta(*m_uniform_random_generator);
		mu_tilde = x/(x+y);
		//QTextStream(stdout) << "k = " << k << ", m_n_hat = " << m_n_hat[k-1] << ", m_m_hat = " << m_m_hat[k-1] << ", mu_hat = " << m_m_hat[k-1]/qMax(m_n_hat[k-1],1.0) << ", mu_tilde = " << mu_tilde << endl;
		if (mu_tilde > mu_tilde_max) {k_select = k; mu_tilde_max = mu_tilde;}
	}
	//QTextStream(stdout) << "k_select = " << k_select << endl;
	m_n_hat[k_select-1] += 1.0;
	m_m_hat[k_select-1] += m_env_pointer->make_reward(m_ID, k_select);
}

void Agent::buffering()
{
	memcpy(m_n_hat_transmission_buffer, m_n_hat, m_K*sizeof(double));
	memcpy(m_m_hat_transmission_buffer, m_m_hat, m_K*sizeof(double));
}

double Agent::get_m_n_hat(int k) {
	Q_ASSERT(k>=1 && k<=m_K);
	return m_n_hat_transmission_buffer[k-1];
}

double Agent::get_m_m_hat(int k) {
	Q_ASSERT(k>=1 && k<=m_K);
	return m_m_hat_transmission_buffer[k-1];
}

void Agent::communicate()
{
	for (int k=1; k<=m_K; ++k) {
		m_n_hat[k-1] = 0.0, m_m_hat[k-1] = 0.0;
		for (int j=1; j<=m_N; ++j) {
			m_n_hat[k-1] += m_neighbor_weight[j-1]*(m_agents_pointer[j-1]->get_m_n_hat(k));
			m_m_hat[k-1] += m_neighbor_weight[j-1]*(m_agents_pointer[j-1]->get_m_m_hat(k));
		}
	}
}

void Agent::update_dist_parameter()
{
	for (int k=0; k<m_K; ++k) {
		if (qFabs(m_n_hat_dag[k] - m_n_hat[k]) > 2.0*m_delta) {
			m_n_hat_dag[k] = m_n_hat[k];
			m_m_hat_dag[k] = m_m_hat[k];
		}
	}
}

Agent::~Agent()
{
	safe_array_delete(m_n_hat); safe_array_delete(m_n_hat_transmission_buffer);
	safe_array_delete(m_m_hat); safe_array_delete(m_m_hat_transmission_buffer);
	safe_array_delete(m_n_hat_dag); safe_array_delete(m_m_hat_dag);
	qDebug() << "agent_ID =" << m_ID << ", ~Agent()";
}
