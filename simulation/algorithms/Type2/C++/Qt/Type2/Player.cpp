#include "common.h"
#include "Environment.h"
#include "Player.h"

Player::Player(int ID, std::mt19937 *uniform_random_generator, Environment *env_pointer, double *prior_alpha, double *prior_beta) : m_ID(ID), m_uniform_random_generator(uniform_random_generator), m_env_pointer(env_pointer), m_K(m_env_pointer->m_K), m_prior_alpha(prior_alpha), m_prior_beta(prior_beta) {}

Player::~Player() {}
