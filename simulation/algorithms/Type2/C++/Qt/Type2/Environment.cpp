#include "common.h"
#include "Environment.h"

#include <QDebug>
#include <QTextStream>

Environment::Environment(std::mt19937 *uniform_random_generator, int K, int N, double *mean_rewards) : m_uniform_random_generator(uniform_random_generator), m_K(K), m_N(N), m_mean_rewards(mean_rewards)
{
	m_arms = new std::bernoulli_distribution[m_K];
	for (int k=0; k<m_K; k++) {m_arms[k] = std::bernoulli_distribution(m_mean_rewards[k]);}
	m_regret_agent = new double[m_N];
}

void Environment::init_play()
{
	m_regret_loner = 0.0;
	std::fill_n(m_regret_agent, m_N, 0.0);
}

int Environment::make_reward(int agent_ID, int k)
{
	Q_ASSERT(agent_ID >= 0 && agent_ID <= m_N);
	Q_ASSERT(k>=1 && k<=m_K);
	if (k > 1) {	//非最適アームを引いた時
		if (agent_ID == 0) {
			m_regret_loner += m_mean_rewards[0]-m_mean_rewards[k-1];
		} else {m_regret_agent[agent_ID-1] += m_mean_rewards[0]-m_mean_rewards[k-1];}
	}
	//if (agent_ID != 0) {QTextStream(stdout) << "regret = " << m_regret_agent[agent_ID-1] << "\n";} else {QTextStream(stdout) << "regret = " << m_regret_loner << "\n";}
	return m_arms[k-1](*m_uniform_random_generator);
}

double Environment::get_regret(int agent_ID)
{
	Q_ASSERT(agent_ID >= 0 && agent_ID <= m_N);
	if (agent_ID==0) {return m_regret_loner;} else {return m_regret_agent[agent_ID-1];}
}

Environment::~Environment() {
	safe_array_delete(m_arms);
	safe_array_delete(m_regret_agent);
	qDebug() << "~Environment()";
}
